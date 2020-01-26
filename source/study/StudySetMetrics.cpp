#include "StudySetMetrics.h"

StudySetMetrics::StudySetMetrics()
{
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

StudySetMetrics& StudySetMetrics::operator+=(const StudySetMetrics& other)
{
	uint32 myTotal = GetTotalCount();
	uint32 otherTotal = other.GetTotalCount();
	if (otherTotal > 0)
	{
		m_historyScore = ((m_historyScore * myTotal) +
			(other.m_historyScore * otherTotal)) / (myTotal + otherTotal);
		for (auto it : other.m_proficiencyCounts)
			AddCount(it.first, it.second);
	}
	return *this;
}

void StudySetMetrics::AddCount(ProficiencyLevel level, uint32 amount)
{
	auto it = m_proficiencyCounts.find(level);
	if (it == m_proficiencyCounts.end())
		m_proficiencyCounts[level] = amount;
	else
		m_proficiencyCounts[level] += amount;
}
