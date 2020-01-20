#include "StudySet.h"

void StudySet::AddCard(Card::sptr card)
{
	m_cards.insert(card);
}
