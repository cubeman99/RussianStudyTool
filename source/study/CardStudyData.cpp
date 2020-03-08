#include "CardStudyData.h"
#include "Config.h"

CardStudyData::CardStudyData()
{
}

ProficiencyLevel CardStudyData::GetProficiencyLevel() const
{
	return m_proficiencyLevel;
}

AppTimestamp CardStudyData::GetLastEncounterTime() const
{
	return m_lastEncounterTime;
}

bool CardStudyData::IsEncountered() const
{
	return (m_lastEncounterTime > 0.0);
}

float CardStudyData::GetHistoryScore() const
{
	return CalcHistoryScore(m_history);
}

bool CardStudyData::GetHistoryKnewIt(uint32 index) const
{
	return m_history[index];
}

AppTimestamp CardStudyData::GetHistoryTimestamp(uint32 index) const
{
	return m_historyTimestamps[index];
}

uint32 CardStudyData::GetHistorySize() const
{
	return m_history.size();
}

void CardStudyData::SetProficiencyLevel(ProficiencyLevel proficiencyLevel)
{
	m_proficiencyLevel = proficiencyLevel;
}

void CardStudyData::AddToHistory(bool knewIt, AppTimestamp timestamp)
{
	m_lastEncounterTime = timestamp;
	m_history.insert(m_history.begin(), knewIt);
	m_historyTimestamps.insert(m_historyTimestamps.begin(), timestamp);
	if (m_history.size() > Config::k_maxCardHistorySize)
	{
		m_history.resize(Config::k_maxCardHistorySize);
		m_historyTimestamps.resize(Config::k_maxCardHistorySize);
	}
}

void CardStudyData::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator, const CardRuKey& key) const
{
	// Key
	String keyType = EnumToString(key.type);
	String keyRussian = ConvertToUTF8(key.russian);
	value.AddMember("type", rapidjson::Value(
		keyType.c_str(), allocator).Move(), allocator);
	value.AddMember("ru", rapidjson::Value(
		keyRussian.c_str(), allocator).Move(), allocator);

	// Proficiency
	value.AddMember("level", (int32_t) m_proficiencyLevel, allocator);

	// History (round timestamps to the nearest second)
	rapidjson::Value historyDataList(rapidjson::kArrayType);
	AppTimestamp lastTimestamp = 0;
	for (uint32 i = 0; i < m_history.size(); i++)
	{
		bool knewIt = m_history[i];
		AppTimestamp timestamp = m_historyTimestamps[i];
		if (i == 0)
		{
			int intTimestamp = (int) (timestamp + 0.5);
			historyDataList.PushBack(
				knewIt ? intTimestamp : -intTimestamp, allocator);
		}
		else
		{
			int elapsedSeconds = Math::Max(
				1, (int) (lastTimestamp - timestamp + 0.5));
			historyDataList.PushBack(
				knewIt ? elapsedSeconds : -elapsedSeconds, allocator);
		}
		lastTimestamp = timestamp;
	}
	value.AddMember("history", historyDataList, allocator);
}

Error CardStudyData::Deserialize(rapidjson::Value& data, CardRuKey& outKey)
{
	// Deserialize card key
	TryStringToEnum(data["type"].GetString(), outKey.type, false);
	outKey.russian = ConvertFromUTF8(data["ru"].GetString());

	// Proficiency
	m_proficiencyLevel = (ProficiencyLevel) data["level"].GetInt();

	// History
	//   - Positive timestmap = knew it
	//   - Negative timestamp = didn't know it.
	m_lastEncounterTime = -1;
	rapidjson::Value& historyDataList = data["history"];
	AppTimestamp lastTimestamp = 0;
	for (auto it = historyDataList.Begin(); it != historyDataList.End(); it++)
	{
		int timestamp = it->GetInt();
		if (timestamp == 0)
			return CMG_ERROR_FAILURE;
		bool knewIt = (timestamp > 0);
		timestamp = Math::Abs(timestamp);
		if (m_lastEncounterTime < 0)
		{
			// First history entry is the absolute timestamp
			m_lastEncounterTime = static_cast<AppTimestamp>(timestamp);
			m_historyTimestamps.push_back(m_lastEncounterTime);
		}
		else
		{
			// Other history entries are elapsed seconds since last entry
			m_historyTimestamps.push_back(lastTimestamp -
				static_cast<AppTimestamp>(timestamp));
		}
		m_history.push_back(knewIt);
		lastTimestamp = m_historyTimestamps.back();
	}

	return CMG_ERROR_SUCCESS;
}

float CardStudyData::CalcHistoryScore(const Array<bool>& history)
{
	if (history.empty())
		return 0.0f;
	float score = 1.0f;
	for (uint32 i = 0; i < history.size(); i++)
	{
		if (!history[i])
			score -= 0.5f / (i + 2);
	}
	uint32 minLength = 6;
	if (history.size() < minLength)
		score /= (minLength - history.size() + 1.0f);
	return score;
}
