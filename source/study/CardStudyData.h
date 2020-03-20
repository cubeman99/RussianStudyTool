#pragma once

#include "cards/Card.h"
#include "core/JsonUtility.h"


struct CardStudyMark
{
public:
	bool knewIt = false;
	AppTimestamp timestamp = 0.0;
	Language shownSide = Language::k_russian;
};


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

	void AddToHistory(bool knewIt, AppTimestamp timestamp, Language shownSide);

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator, const CardRuKey& key) const;
	Error Deserialize(rapidjson::Value& data, CardRuKey& outKey);

	static float CalcHistoryScore(const Array<CardStudyMark>& history);

private:
	// First = most recent
	Array<CardStudyMark> m_markings;
	AppTimestamp m_lastEncounterTime = -1.0;
};
