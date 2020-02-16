#include "Card.h"


Card::Card()
{
}

Card::Card(WordType type, AccentedText russian, AccentedText english) :
	m_type(type),
	m_text(russian, english)
{
}

Card::~Card()
{
}

const TranslationPair & Card::GetText() const
{
	return m_text;
}

const AccentedText & Card::GetText(Language language) const
{
	return m_text.GetText(language);
}

AppTimestamp Card::GetCreationTimestamp() const
{
	return m_creationTimestamp;
}

CardKey Card::GetKey() const
{
	CardKey key;
	key.type = m_type;
	key.english = m_text.english.GetString();
	key.russian = m_text.russian.GetString();
	ru::ToLowerIP(key.russian);
	ru::ToLowerIP(key.english);
	return key;
}

CardRuKey Card::GetRuKey() const
{
	return CardRuKey(m_type, m_text.russian);
}

CardEnKey Card::GetEnKey() const
{
	return CardEnKey(m_type, m_text.english, m_tags);
}

Set<Card::sptr>& Card::GetRelatedCards()
{
	return m_relatedCards;
}

EnumFlags<CardTags>& Card::GetTags()
{
	return m_tags;
}

void Card::SetData(const CardData& data)
{
	m_type = data.type;
	m_text = data.text;
	m_tags = data.tags;
	m_relatedCards = data.relatedCards;
}

CardData Card::GetData()
{
	CardData data;
	data.type = m_type;
	data.text = m_text;
	data.tags = m_tags;
	data.relatedCards = m_relatedCards;
	return data;
}
