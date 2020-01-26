#include "CardSetPackage.h"

const Set<Card::sptr>& CardSetPackage::GetCards() const
{
	m_cards.clear();
	for (auto package : m_packages)
	{
		for (Card::sptr card : package->GetCards())
			m_cards.insert(card);
	}
	for (auto cardSet : m_cardSets)
	{
		for (Card::sptr card : cardSet->GetCards())
			m_cards.insert(card);
	}
	return m_cards;
}

void CardSetPackage::AddCardSet(CardSet::sptr cardSet)
{
	m_cardSets.push_back(cardSet);
}

void CardSetPackage::AddPackage(CardSetPackage::sptr package)
{
	m_packages.push_back(package);
}

void CardSetPackage::SetParent(CardSetPackage::sptr parent)
{
	m_parent = parent;
}
