#include "StudySetMetrics.h"


StudySetMetrics::StudySetMetrics()
{
	memset(m_scoreCounts, 0, sizeof(m_scoreCounts));
}

float StudySetMetrics::GetHistoryScore() const
{
	return m_historyScore;
}

uint32 StudySetMetrics::GetTotalCount() const
{
	uint32 total = 0;
	for (auto it : m_proficiencyCounts)
		total += it.second;
	return total;
}

uint32 StudySetMetrics::GetCount(ProficiencyLevel level) const
{
	auto it = m_proficiencyCounts.find(level);
	if (it != m_proficiencyCounts.end())
		return it->second;
	return 0;
}

uint32 StudySetMetrics::GetCountForScoreBin(uint32 bin) const
{
	return m_scoreCounts[bin];
}

uint32 StudySetMetrics::GetNumScoreBins() const
{
	return Config::k_studyScoreHistogramBinCount;
}

void StudySetMetrics::Add(const CardStudyData& cardStudyData)
{
	auto score = cardStudyData.GetHistoryScore();
	m_cardCount++;
	m_historyScoreSum += score;
	m_historyScore = m_historyScoreSum / m_cardCount;

	ProficiencyLevel level = cardStudyData.GetProficiencyLevel();
	auto it = m_proficiencyCounts.find(level);
	if (it == m_proficiencyCounts.end())
		m_proficiencyCounts[level] = 1;
	else
		m_proficiencyCounts[level]++;

	if (cardStudyData.IsEncountered())
	{
		uint32 bin = Math::Min(
			(uint32) (score * Config::k_studyScoreHistogramBinCount),
			Config::k_studyScoreHistogramBinCount - 1);
		m_scoreCounts[bin]++;
	}
}
