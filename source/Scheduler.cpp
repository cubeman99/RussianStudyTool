#include "Scheduler.h"

Scheduler::Scheduler(StudySet::sptr studySet, const StudyParams& params) :
	m_params(params),
	m_studySet(studySet)
{
	auto cards = m_studySet->GetCards();
	for (auto it = cards.begin(); it != cards.end(); it++)
		m_cardList.push_back(*it);
	m_cardIndex = 0;
}

Card::sptr Scheduler::NextCard()
{
	Card::sptr card;

	if (m_cardIndex < m_cardList.size())
	{
		card = m_cardList[m_cardIndex];
		m_cardIndex++;
	}

	return card;
}
