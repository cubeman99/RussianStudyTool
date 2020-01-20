#include "CardSetPackage.h"

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
