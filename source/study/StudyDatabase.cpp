#include "StudyDatabase.h"
#include "Config.h"
#include "core/JsonUtility.h"

StudyDatabase::StudyDatabase(CardDatabase& cardDatabase) :
	m_cardDatabase(cardDatabase)
{
	m_cardDatabase.CardKeyChanged().Connect(this, &StudyDatabase::OnCardKeyChanged);
	m_cardDatabase.CardDeleted().Connect(this, &StudyDatabase::OnCardDeleted);
	m_cardDatabase.CardAddedToSet().Connect(this, &StudyDatabase::OnCardAddedToSet);
	m_cardDatabase.CardRemovedFromSet().Connect(this, &StudyDatabase::OnCardRemovedFrom);
}

StudyDatabase::~StudyDatabase()
{
	Clear();
}

const Path& StudyDatabase::GetStudyDataPath() const
{
	return m_path;
}

CardStudyData& StudyDatabase::GetCardStudyData(Card::sptr card)
{
	return GetCardStudyData(card->GetRuKey());
}

CardStudyData& StudyDatabase::GetCardStudyData(const CardRuKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardStudyData.find(key);
	if (it != m_cardStudyData.end())
		return m_cardStudyData[key];
	return CreateCardStudyData(key);
}

CardStudyData& StudyDatabase::CreateCardStudyData(const CardRuKey& key)
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
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_cardSetMetrics.erase(cardSet);
	if (cardSet->GetParent())
		RecalcStudySetMetrics(cardSet->GetParent());
}

void StudyDatabase::RecalcStudySetMetrics(CardSetPackage::sptr package)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_packageMetrics.erase(package);
	if (package->GetParent())
		RecalcStudySetMetrics(package->GetParent());
}

StudySetMetrics& StudyDatabase::CalcStudyMetrics(CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_cardSetMetrics[cardSet] = GetStudySetMetrics((IStudySet*) cardSet.get());
	return m_cardSetMetrics[cardSet];
}

StudySetMetrics& StudyDatabase::CalcStudyMetrics(CardSetPackage::sptr package)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_packageMetrics[package] = GetStudySetMetrics((IStudySet*) package.get());
	return m_packageMetrics[package];
}

void StudyDatabase::Clear()
{
	std::lock_guard<std::recursive_mutex> guardData(m_mutexStudyData);
	std::lock_guard<std::recursive_mutex> guardMetrics(m_mutexStudyMetrics);
	m_cardStudyData.clear();
	m_cardSetMetrics.clear();
	m_packageMetrics.clear();
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

void StudyDatabase::SetStudyDataPath(const Path& path)
{
	m_path = path;
}

Error StudyDatabase::LoadStudyData()
{
	return LoadStudyData(m_path);
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
	rapidjson::Document document;
	Error loadError = json::LoadDocumentFromFile(path, document);
	if (loadError.Failed())
		return loadError.Uncheck();

	// Gather all card data
	rapidjson::Value& cardDataList = document["cards"];
	for (auto it = cardDataList.Begin(); it != cardDataList.End(); it++)
	{
		CardRuKey key;
		CardStudyData studyData;
		DeserializeCardStudyData(*it, key, studyData);
		m_cardStudyData[key] = studyData;
	}

	MarkDirty(false);
	return CMG_ERROR_SUCCESS;
}

Error StudyDatabase::SaveStudyData()
{
	return SaveStudyData(m_path);
}

Error StudyDatabase::SaveStudyData(const Path& path)
{
	if (m_readOnly)
		return CMG_ERROR_SUCCESS;

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
		const CardRuKey& key = it.first;
		const CardStudyData& studyData = it.second;
		if (it.second.IsEncountered())
		{
			historyString.resize(studyData.m_history.size() + 1);
			for (uint32 i = 0; i < studyData.m_history.size(); i++)
				historyString[i] = studyData.m_history[i] ? 'T' : 'F';
			historyString[studyData.m_history.size()] = '\0';

			rapidjson::Value jsonStudyData(rapidjson::kArrayType);
			String keyType = EnumToString(key.type);
			String keyRussian = ConvertToUTF8(key.russian);
			jsonStudyData.PushBack(rapidjson::Value(keyType.c_str(), allocator).Move(), allocator);
			jsonStudyData.PushBack(rapidjson::Value(keyRussian.c_str(), allocator).Move(), allocator);
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
	MarkDirty(false);
	return json::SaveDocumentToFile(path, document);
}

Error StudyDatabase::SaveChanges()
{
	bool saveStudyData = false;

	// Retreive dirty state
	{
		std::lock_guard<std::mutex> guardDirty(m_mutexDirty);
		saveStudyData = m_isDirty;
	}

	// Save study data
	if (saveStudyData)
	{
		return SaveStudyData();
	}

	return CMG_ERROR_SUCCESS;
}

Error StudyDatabase::DeserializeCardStudyData(rapidjson::Value& data,
	CardRuKey& outKey, CardStudyData& outCardStudyData)
{
	TryStringToEnum(data[0].GetString(), outKey.type, false);
	outKey.russian = ConvertFromUTF8(data[1].GetString());
	outCardStudyData.m_proficiencyLevel = (ProficiencyLevel) data[2].GetInt();
	outCardStudyData.m_lastEncounterTime = data[3].IsNull() ? -1.0 :
		static_cast<AppTimestamp>(data[3].GetDouble());
	outCardStudyData.m_history.clear();
	auto historyString = data[4].GetString();
	for (uint32 i = 0; i < Config::k_maxCardHistorySize &&
		historyString[i] != '\0'; i++)
	{
		bool knewIt = (historyString[i] == 'T');
		outCardStudyData.m_history.push_back(knewIt);
	}
	return CMG_ERROR_SUCCESS;
}

void StudyDatabase::OnCardKeyChanged(Card::sptr card, CardRuKey oldKey)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardStudyData.find(oldKey);
	if (it != m_cardStudyData.end())
	{
		m_cardStudyData[card->GetRuKey()] = it->second;
		m_cardStudyData.erase(oldKey);
	}
	MarkDirty();
}

void StudyDatabase::OnCardDeleted(Card::sptr card)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CardRuKey key = card->GetRuKey();
	auto it = m_cardStudyData.find(key);
	if (it != m_cardStudyData.end())
		m_cardStudyData.erase(key);
	MarkDirty();
}

void StudyDatabase::OnCardAddedToSet(Card::sptr card, CardSet::sptr cardSet)
{
	RecalcStudySetMetrics(cardSet);
}

void StudyDatabase::OnCardRemovedFrom(Card::sptr card, CardSet::sptr cardSet)
{
	RecalcStudySetMetrics(cardSet);
}

void StudyDatabase::OnCardStudyDataChanged(Card::sptr card)
{
	MarkDirty();
	for (CardSet::sptr cardSet : m_cardDatabase.GetCardSetsWithCard(card))
		RecalcStudySetMetrics(cardSet);
	m_cardStudyDataChanged.Emit(card);
}

void StudyDatabase::MarkDirty(bool dirty)
{
	std::lock_guard<std::mutex> guard(m_mutexDirty);
	m_isDirty = dirty;
}
