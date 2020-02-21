#include "CardSet.h"


CardSetKey::CardSetKey()
{
}

CardSetKey::CardSetKey(const unistr& name) :
	name(name)
{
}

bool CardSetKey::operator<(const CardSetKey& other) const
{
	return name < other.name;
}


CardSet::CardSet()
{
}

CardSet::CardSet(const AccentedText& name) :
	m_name(name),
	m_key(ru::ToLower(name.GetString()))
{
}

CardSet::~CardSet()
{
}

void CardSet::SetParent(cmg::shared_ptr<CardSetPackage> parent)
{
	m_parent = parent;
}

bool CardSet::AddCard(Card::sptr card)
{
	if (std::find(m_cards.begin(), m_cards.end(), card) == m_cards.end())
	{
		m_cards.push_back(card);
		return true;
	}
	return false;
}

bool CardSet::InsertCard(uint32 index, Card::sptr card)
{
	if (std::find(m_cards.begin(), m_cards.end(), card) == m_cards.end())
	{
		m_cards.insert(m_cards.begin() + index, card);
		return true;
	}
	return false;
}

bool CardSet::RemoveCard(Card::sptr card)
{
	auto it = std::find(m_cards.begin(), m_cards.end(), card);
	if (it != m_cards.end())
	{
		m_cards.erase(it);
		return true;
	}
	return false;
}

const PathU16& CardSet::GetPath() const
{
	return m_path;
}

const CardSetKey& CardSet::GetKey() const
{
	return m_key;
}

cmg::shared_ptr<CardSetPackage> CardSet::GetParent() const
{
	return m_parent;
}

CardSetType CardSet::GetCardSetType() const
{
	return m_cardSetType;
}

const AccentedText & CardSet::GetName() const
{
	return m_name;
}

const Array<Card::sptr>& CardSet::GetCards() const
{
	return m_cards;
}

Array<Card::sptr>& CardSet::GetCards()
{
	return m_cards;
}

bool CardSet::HasCard(Card::sptr card) const
{
	return (std::find(m_cards.begin(), m_cards.end(), card) != m_cards.end());
}

std::ostream& operator<<(std::ostream& out, const CardSetKey& key)
{
	out << "<CardSetKey(" << (AccentedText) key.name << ")>";
	return out;
}
