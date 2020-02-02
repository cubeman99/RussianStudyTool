#include "StudySet.h"

void StudySet::AddCard(Card::sptr card)
{
	if (std::find(m_cards.begin(), m_cards.end(), card) == m_cards.end())
		m_cards.push_back(card);
}
