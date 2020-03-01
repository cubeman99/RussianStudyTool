#include "CardSetPackage.h"

CardSetPackage::CardSetPackage()
{
}

CardSetPackage::CardSetPackage(const AccentedText& name) :
	m_name(name)
{
}

CardSetPackage::~CardSetPackage()
{
}

const Array<Card::sptr>& CardSetPackage::GetCards() const
{
	m_cards.clear();
	for (auto package : m_packages)
	{
		for (Card::sptr card : package->GetCards())
		{
			if (!cmg::container::Contains(m_cards, card))
				m_cards.push_back(card);
		}
	}
	for (auto cardSet : m_cardSets)
	{
		for (Card::sptr card : cardSet->GetCards())
		{
			if (!cmg::container::Contains(m_cards, card))
				m_cards.push_back(card);
		}
	}
	return m_cards;
}

void CardSetPackage::AddCardSet(CardSet::sptr cardSet)
{
	m_cardSets.push_back(cardSet);
}

void CardSetPackage::RemoveCardSet(CardSet::sptr cardSet)
{
	cmg::container::EraseIfFound(m_cardSets, cardSet);
	cardSet->SetParent(nullptr);
}

void CardSetPackage::AddPackage(CardSetPackage::sptr package)
{
	m_packages.push_back(package);
}

void CardSetPackage::SetParent(CardSetPackage::sptr parent)
{
	m_parent = parent;
}
