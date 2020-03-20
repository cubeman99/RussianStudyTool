#include "CardStudyData.h"
#include "Config.h"

CardStudyData::CardStudyData()
{
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
	return CalcHistoryScore(m_markings);
}

bool CardStudyData::GetHistoryKnewIt(uint32 index) const
{
	return m_markings[index].knewIt;
}

AppTimestamp CardStudyData::GetHistoryTimestamp(uint32 index) const
{
	return m_markings[index].timestamp;
}

uint32 CardStudyData::GetHistorySize() const
{
	return m_markings.size();
}

void CardStudyData::AddToHistory(bool knewIt, AppTimestamp timestamp, Language shownSide)
{
	m_lastEncounterTime = timestamp;

	CardStudyMark mark;
	mark.timestamp = timestamp;
	mark.knewIt = knewIt;
	mark.shownSide = shownSide;
	m_markings.insert(m_markings.begin(), mark);
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
	
	// Markings (round timestamps to the nearest second)
	rapidjson::Value historyDataList(rapidjson::kArrayType);
	for (uint32 i = 0; i < m_markings.size(); i++)
	{
		const auto& mark = m_markings[i];
		rapidjson::Value markData(rapidjson::kObjectType);
		if (mark.shownSide == Language::k_russian)
			markData.AddMember("side", "target", allocator);
		else
			markData.AddMember("side", "source", allocator);
		markData.AddMember("time", (int) mark.timestamp, allocator);
		markData.AddMember("knew_it", mark.knewIt, allocator);
		historyDataList.PushBack(markData, allocator);
	}
	value.AddMember("history", historyDataList, allocator);
}

Error CardStudyData::Deserialize(rapidjson::Value& data, CardRuKey& outKey)
{
	// Deserialize card key
	TryStringToEnum(data["type"].GetString(), outKey.type, false);
	outKey.russian = ConvertFromUTF8(data["ru"].GetString());
	
	// Markings
	rapidjson::Value& historyDataList = data["history"];
	m_markings.clear();
	for (auto it = historyDataList.Begin(); it != historyDataList.End(); it++)
	{
		CardStudyMark mark;
		mark.timestamp = static_cast<AppTimestamp>((*it)["time"].GetInt());
		if (mark.timestamp <= 0)
			return CMG_ERROR_FAILURE;
		mark.knewIt = (*it)["knew_it"].GetBool();
		String sideName = (*it)["side"].GetString();
		mark.shownSide = (sideName == "source" ? Language::k_english : Language::k_russian);
		m_markings.push_back(mark);
	}

	m_lastEncounterTime = -1;
	if (!m_markings.empty())
		m_lastEncounterTime = m_markings[0].timestamp;

	return CMG_ERROR_SUCCESS;
}

float CardStudyData::CalcHistoryScore(const Array<CardStudyMark>& history)
{
	if (history.empty())
		return 0.0f;
	float score = 1.0f;
	for (uint32 i = 0; i < history.size(); i++)
	{
		if (!history[i].knewIt)
			score -= 0.5f / (i + 2);
	}
	uint32 minLength = 6;
	if (history.size() < minLength)
		score /= (minLength - history.size() + 1.0f);
	return score;
}
