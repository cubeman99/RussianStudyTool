#include "StudyDatabase.h"
#include "Config.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"

DEFINE_ENUM(ProficiencyLevel, ENUM_MACRO_PROFICIENCY_LEVEL)


StudyDatabase::StudyDatabase()
{
}

StudyDatabase::~StudyDatabase()
{
	Clear();
}

CardStudyData StudyDatabase::GetCardStudyData(Card::sptr card)
{
	return GetCardStudyData(card->GetKey());
}

CardStudyData StudyDatabase::GetCardStudyData(const CardKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardStudyData.find(key);
	if (it != m_cardStudyData.end())
		return it->second;
	return CreateCardStudyData(key);
}

CardStudyData StudyDatabase::CreateCardStudyData(const CardKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CardStudyData data;
	m_cardStudyData[key] = data;
	return data;
}

void StudyDatabase::Clear()
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	m_cardStudyData.clear();
}

void StudyDatabase::MarkCard(Card::sptr card, bool knewIt)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto studyData = GetCardStudyData(card);
	studyData.m_lastEncounterTime = 0.0f;
	studyData.m_history.insert(studyData.m_history.begin(), knewIt);
	if (studyData.m_history.size() > Config::MAX_CARD_HISTORY_SIZE)
		studyData.m_history.resize(Config::MAX_CARD_HISTORY_SIZE);

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
		}
	}
	else
	{
		if (studyData.m_proficiencyLevel != ProficiencyLevel::k_hard)
		{
			studyData.m_proficiencyLevel = (ProficiencyLevel)
				((uint32) studyData.m_proficiencyLevel - 1);
		}
	}
}

Error StudyDatabase::LoadStudyData(const Path & path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CMG_LOG_INFO() << "Loading study data";

	// Open the json file
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

Error StudyDatabase::SaveStudyData(const Path & path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	return CMG_ERROR_NOT_IMPLEMENTED;
}

Error StudyDatabase::DeserializeCardStudyData(rapidjson::Value& data,
	CardKey& outKey, CardStudyData& outCardStudyData)
{
	TryStringToEnum(data[0].GetString(), outKey.type, false);
	outKey.russian = ConvertFromUTF8(data[1].GetString());
	outKey.english = ConvertFromUTF8(data[2].GetString());
	outCardStudyData.m_proficiencyLevel = (ProficiencyLevel) data[3].GetInt();
	outCardStudyData.m_lastEncounterTime = data[4].IsNull() ? -1.0f : data[4].GetFloat();
	outCardStudyData.m_history.clear();
	auto historyString = data[5].GetString();
	for (uint32 i = 0; i < Config::MAX_CARD_HISTORY_SIZE &&
		historyString[i] != '\0'; i++)
	{
		bool knewIt = (historyString[i] == 'T');
		outCardStudyData.m_history.push_back(knewIt);
	}
	return CMG_ERROR_SUCCESS;
}
