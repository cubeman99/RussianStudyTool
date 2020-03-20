#pragma once

#include "cards/Card.h"
#include "core/JsonUtility.h"


class CardStudyData
{
public:
	CardStudyData();

	AppTimestamp GetLastEncounterTime() const;
	bool IsEncountered() const;
	float GetHistoryScore() const;
	bool GetHistoryKnewIt(uint32 index) const;
	AppTimestamp GetHistoryTimestamp(uint32 index) const;
	uint32 GetHistorySize() const;

	void AddToHistory(bool knewIt, AppTimestamp timestamp);

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator, const CardRuKey& key) const;
	Error Deserialize(rapidjson::Value& data, CardRuKey& outKey);

	static float CalcHistoryScore(const Array<bool>& history);

private:
	// First = most recent
	Array<bool> m_history;
	Array<AppTimestamp> m_historyTimestamps;
	AppTimestamp m_lastEncounterTime = -1.0;
};
