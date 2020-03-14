#pragma once

#include "cards/StudySet.h"
#include "study/CardStudyData.h"
#include "Config.h"


class StudySetMetrics
{
public:
	StudySetMetrics();

	float GetHistoryScore() const;
	uint32 GetTotalCount() const;
	uint32 GetCount(ProficiencyLevel level) const;
	uint32 GetCountForScoreBin(uint32 bin) const;
	uint32 GetNumScoreBins() const;

	void Add(const CardStudyData& cardStudyData);

private:
	uint32 m_cardCount = 0;
	float m_historyScore = 0.0f;
	float m_historyScoreSum = 0.0f;
	uint32 m_scoreCounts[Config::k_studyScoreHistogramBinCount];
	Map<ProficiencyLevel, uint32> m_proficiencyCounts;
};
