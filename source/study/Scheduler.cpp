#include "Scheduler.h"


Scheduler::Scheduler(StudyDatabase& studyDatabase) :
	m_studyDatabase(studyDatabase)
{
}

void Scheduler::Begin(IStudySet * studySet, const StudyParams & params)
{
	SchedulerBase::Begin(studySet, params);
}

void Scheduler::MarkCard(Card::sptr card, bool knewIt)
{
	m_studyDatabase.MarkCard(card, knewIt);
	MarkItem(card);
}

bool Scheduler::IsItemEncountered(Card::sptr card)
{
	const auto& studyData = m_studyDatabase.GetCardStudyData(card);
	return studyData.IsEncountered();
}

int32 Scheduler::GetRepeatInterval(Card::sptr card)
{
	const auto& studyData = m_studyDatabase.GetCardStudyData(card);

	float minScore = 1.0f;
	float maxScore = 0.0f;
	for (auto x : m_studySet->GetCards())
	{
		float score =  m_studyDatabase.GetCardStudyData(x).GetHistoryScore();
		minScore = Math::Min(minScore, score);
		maxScore = Math::Max(maxScore, score);
	}

	float minSpan = 0.3f;
	if (maxScore - minScore < minSpan)
	{
		float center = (maxScore + minScore) * 0.5f;
		minScore = center - (minSpan * 2.0f);
		maxScore = center + (minSpan * 2.0f);
		if (minScore < 0.0f)
		{
			minScore = 0.0f;
			maxScore = minScore + minSpan;
		}
		else if (maxScore > 1.0f)
		{
			maxScore = 1.0f;
			minScore = maxScore - minSpan;
		}
	}

	float percent = (studyData.GetHistoryScore() - minScore) /
		(maxScore - minScore);
	percent = Math::Pow(percent, 2.0f);
	float interval = percent * m_studySet->GetCards().size() * 1.5f;

	return Math::Max((int32) interval,
		(int32) m_params.GetMinRepeatInterval());
}
