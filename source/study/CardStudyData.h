#pragma once

#include "cards/Card.h"


class CardStudyData
{
public:
	friend class StudyDatabase;

	CardStudyData() {}

	ProficiencyLevel GetProficiencyLevel() const { return m_proficiencyLevel; }
	AppTimestamp GetLastEncounterTime() const { return m_lastEncounterTime; }
	bool IsEncountered() const { return (m_lastEncounterTime > 0.0); }

	float GetHistoryScore() const
	{
		return CalcHistoryScore(m_history);
	}

	static float CalcHistoryScore(const Array<bool>& history);

private:
	Array<bool> m_history;
	AppTimestamp m_lastEncounterTime = -1.0;
	ProficiencyLevel m_proficiencyLevel = ProficiencyLevel::k_new;
};
