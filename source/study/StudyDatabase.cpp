#include "StudyDatabase.h"
#include "Config.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"

StudyDatabase::StudyDatabase(CardDatabase& cardDatabase) :
	m_cardDatabase(cardDatabase)
{
}

StudyDatabase::~StudyDatabase()
{
	Clear();
}

CardStudyData& StudyDatabase::GetCardStudyData(Card::sptr card)
{
	return GetCardStudyData(card->GetKey());
}

CardStudyData& StudyDatabase::GetCardStudyData(const CardKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardStudyData.find(key);
	if (it != m_cardStudyData.end())
		return m_cardStudyData[key];
	return CreateCardStudyData(key);
}

CardStudyData& StudyDatabase::CreateCardStudyData(const CardKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CardStudyData data;
	m_cardStudyData[key] = data;
	return m_cardStudyData[key];
}

StudySetMetrics StudyDatabase::GetStudyMetrics(CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardSetMetrics.find(cardSet);
	if (it != m_cardSetMetrics.end())
		return m_cardSetMetrics[cardSet];
	return CalcStudyMetrics(cardSet);
}

StudySetMetrics StudyDatabase::GetStudyMetrics(CardSetPackage::sptr package)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_packageMetrics.find(package);
	if (it != m_packageMetrics.end())
		return m_packageMetrics[package];
	return CalcStudyMetrics(package);
}

StudySetMetrics StudyDatabase::GetStudySetMetrics(const IStudySet* studySet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	StudySetMetrics metrics;
	metrics.m_historyScore = 0.0f;
	uint32 totalCount = 0;
	for (Card::sptr card : studySet->GetCards())
	{
		auto& cardStudyData = GetCardStudyData(card);
		metrics.m_historyScore += cardStudyData.GetHistoryScore();
		metrics.AddCount(cardStudyData.GetProficiencyLevel(), 1);
		totalCount++;
	}
	if (totalCount > 0)
		metrics.m_historyScore /= totalCount;
	return metrics;
}

void StudyDatabase::RecalcStudySetMetrics(CardSet::sptr cardSet)
{
	m_cardSetMetrics.erase(cardSet);
	if (cardSet->GetParent())
		RecalcStudySetMetrics(cardSet->GetParent());
}

void StudyDatabase::RecalcStudySetMetrics(CardSetPackage::sptr package)
{
	m_packageMetrics.erase(package);
	if (package->GetParent())
		RecalcStudySetMetrics(package);
}

StudySetMetrics& StudyDatabase::CalcStudyMetrics(CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	StudySetMetrics metrics;
	metrics.m_historyScore = 0.0f;
	uint32 totalCount = 0;
	for (Card::sptr card : cardSet->GetCards())
	{
		auto& cardStudyData = GetCardStudyData(card);
		metrics.m_historyScore += cardStudyData.GetHistoryScore();
		metrics.AddCount(cardStudyData.GetProficiencyLevel(), 1);
		totalCount++;
	}
	if (totalCount > 0)
		metrics.m_historyScore /= totalCount;
	m_cardSetMetrics[cardSet] = metrics;
	return m_cardSetMetrics[cardSet];
}

StudySetMetrics& StudyDatabase::CalcStudyMetrics(CardSetPackage::sptr package)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	StudySetMetrics metrics;
	for (auto cardSet : package->GetCardSets())
	{
		const StudySetMetrics& cardSetMetrics = CalcStudyMetrics(cardSet);
		metrics += cardSetMetrics;
	}
	for (auto subPackage : package->GetPackages())
	{
		const StudySetMetrics& subPackageMetrics = CalcStudyMetrics(subPackage);
		metrics += subPackageMetrics;
	}
	m_packageMetrics[package] = metrics;
	return m_packageMetrics[package];
}

void StudyDatabase::Clear()
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	m_cardStudyData.clear();
}

void StudyDatabase::MarkCard(Card::sptr card, bool knewIt)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto& studyData = GetCardStudyData(card);
	studyData.m_lastEncounterTime = GetAppTimestamp();
	studyData.m_history.insert(studyData.m_history.begin(), knewIt);
	if (studyData.m_history.size() > Config::k_maxCardHistorySize)
		studyData.m_history.resize(Config::k_maxCardHistorySize);

	// Update proficiencly level
	if (studyData.m_proficiencyLevel == ProficiencyLevel::k_new)
	{
		studyData.m_proficiencyLevel = (knewIt ?
			ProficiencyLevel::k_easy : ProficiencyLevel::k_hard);
	}
	else if (knewIt)
	{
		if (studyData.m_proficiencyLevel != ProficiencyLevel::k_learned)
		{
			studyData.m_proficiencyLevel = (ProficiencyLevel)
				((uint32) studyData.m_proficiencyLevel + 1);
			OnCardStudyDataChanged(card);
		}
	}
	else
	{
		if (studyData.m_proficiencyLevel != ProficiencyLevel::k_hard)
		{
			studyData.m_proficiencyLevel = (ProficiencyLevel)
				((uint32) studyData.m_proficiencyLevel - 1);
			OnCardStudyDataChanged(card);
		}
	}
}

Error StudyDatabase::LoadStudyData(const Path& path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CMG_LOG_INFO() << "Loading study data";

	m_cardSetMetrics.clear();
	m_packageMetrics.clear();
	m_cardStudyData.clear();

	// Open the json file
	m_path = path;
	String json;
	rapidjson::Document document;
	File::OpenAndGetContents(path, json);
	document.Parse(json.c_str());
	if (document.HasParseError())
		return CMG_ERROR(CommonErrorTypes::k_file_corrupt);

	// Gather all card data
	rapidjson::Value& cardDataList = document["cards"];
	for (auto it = cardDataList.Begin(); it != cardDataList.End(); it++)
	{
		CardKey key;
		CardStudyData studyData;
		DeserializeCardStudyData(*it, key, studyData);
		m_cardStudyData[key] = studyData;
	}

	return CMG_ERROR_SUCCESS;
}

Error StudyDatabase::SaveStudyData()
{
	return SaveStudyData(m_path);
}

Error StudyDatabase::SaveStudyData(const Path & path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);

	rapidjson::Document document(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// Save the list of card study data
	rapidjson::Value studyDataList(rapidjson::kArrayType);
	document.AddMember("save_time", GetAppTimestamp(), allocator);
	Array<char> historyString;
	uint32_t savedCardCount = 0;
	for (auto it : m_cardStudyData)
	{
		const CardKey& key = it.first;
		const CardStudyData& studyData = it.second;
		if (it.second.IsEncountered())
		{
			historyString.resize(studyData.m_history.size() + 1);
			for (uint32 i = 0; i < studyData.m_history.size(); i++)
				historyString[i] = studyData.m_history[i] ? 'T' : 'F';
			historyString[studyData.m_history.size()] = '\0';

			rapidjson::Value jsonStudyData(rapidjson::kArrayType);
			jsonStudyData.PushBack(rapidjson::Value(
				EnumToString(key.type).c_str(), allocator).Move(), allocator);
			jsonStudyData.PushBack(rapidjson::Value(
				ConvertToUTF8(key.russian).c_str(), allocator).Move(), allocator);
			jsonStudyData.PushBack(rapidjson::Value(
				ConvertToUTF8(key.english).c_str(), allocator).Move(), allocator);
			jsonStudyData.PushBack((int32_t) studyData.m_proficiencyLevel, allocator);
			jsonStudyData.PushBack(studyData.m_lastEncounterTime, allocator);
			jsonStudyData.PushBack(rapidjson::Value(
				historyString.data(), allocator).Move(), allocator);

			studyDataList.PushBack(jsonStudyData.Move(), allocator);
			savedCardCount++;
		}
	}
	document.AddMember("cards", studyDataList, allocator);

	// Save to the file
	FILE* fp;
	fopen_s(&fp, path.c_str(), "wb");
	char writeBuffer[65536];
	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	writer.SetIndent('\t', 1);
	document.Accept(writer);
	fclose(fp);

	return CMG_ERROR_SUCCESS;
}

Error StudyDatabase::DeserializeCardStudyData(rapidjson::Value& data,
	CardKey& outKey, CardStudyData& outCardStudyData)
{
	TryStringToEnum(data[0].GetString(), outKey.type, false);
	outKey.russian = ConvertFromUTF8(data[1].GetString());
	outKey.english = ConvertFromUTF8(data[2].GetString());
	outCardStudyData.m_proficiencyLevel = (ProficiencyLevel) data[3].GetInt();
	outCardStudyData.m_lastEncounterTime = data[4].IsNull() ? -1.0 :
		static_cast<AppTimestamp>(data[4].GetDouble());
	outCardStudyData.m_history.clear();
	auto historyString = data[5].GetString();
	for (uint32 i = 0; i < Config::k_maxCardHistorySize &&
		historyString[i] != '\0'; i++)
	{
		bool knewIt = (historyString[i] == 'T');
		outCardStudyData.m_history.push_back(knewIt);
	}
	return CMG_ERROR_SUCCESS;
}

void StudyDatabase::OnCardStudyDataChanged(Card::sptr card)
{
	for (CardSet::sptr cardSet : m_cardDatabase.GetCardSetsWithCard(card))
	{
		RecalcStudySetMetrics(cardSet);
	}
}
