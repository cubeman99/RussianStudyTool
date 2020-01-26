#pragma once

#include "cards/StudySet.h"

class StudySetMetrics
{
public:
	friend class StudyDatabase;

	StudySetMetrics();

	float GetHistoryScore() const;
	uint32 GetTotalCount() const;
	uint32 GetCount(ProficiencyLevel level) const;

	StudySetMetrics& operator+=(const StudySetMetrics& other);

	void AddCount(ProficiencyLevel level, uint32 amount = 1);

private:
	float m_historyScore = 0.0f;
	Map<ProficiencyLevel, uint32> m_proficiencyCounts;
};
