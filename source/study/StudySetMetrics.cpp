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
	return m_cardCount;
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

	if (cardStudyData.IsEncountered())
	{
		uint32 bin = Math::Min(
			(uint32) (score * Config::k_studyScoreHistogramBinCount),
			Config::k_studyScoreHistogramBinCount - 1);
		m_scoreCounts[bin]++;
	}
}
