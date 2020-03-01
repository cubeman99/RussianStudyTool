#include "StudySet.h"

void StudySet::ClearCards()
{
	m_cards.clear();
}

void StudySet::AddCard(Card::sptr card)
{
	if (!cmg::container::Contains(m_cards, card))
		m_cards.push_back(card);
}

void StudySet::RemoveCard(Card::sptr card)
{
	cmg::container::EraseIfFound(m_cards, card);
}
