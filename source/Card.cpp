#include "Card.h"

DEFINE_ENUM(CardTags, ENUM_MACRO_CARD_TAG)


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

const AccentedText & Card::GetText(Language language) const
{
	return m_text.GetText(language);
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
	CardRuKey key;
	key.type = m_type;
	key.russian = m_text.russian.GetString();
	ru::ToLowerIP(key.russian);
	return key;
}

CardEnKey Card::GetEnKey() const
{
	CardEnKey key;
	key.type = m_type;
	key.english = m_text.english.GetString();
	key.keyTags = m_tags.GetValue(0);
	ru::ToLowerIP(key.english);
	return key;
}

bool CardKey::operator<(const CardKey & other) const
{
	auto a = std::tuple<uint32, unistr, unistr>(
		(uint32) type, russian, english);
	auto b = std::tuple<uint32, unistr, unistr>(
		(uint32) other.type, other.russian, other.english);
	return a < b;
}

std::ostream & operator<<(std::ostream & out, const CardKey & key)
{
	out << "<CardKey(" << EnumToString(key.type) << ", "
		<< key.russian << ", " << key.english << ")>";
	return out;
}

std::ostream & operator<<(std::ostream & out, const CardEnKey & key)
{
	out << "<CardKey(" << EnumToString(key.type) << ", " << ", "
		<< key.english << ", " << key.keyTags << ")>";
	return out;
}

std::ostream & operator<<(std::ostream & out, const CardRuKey & key)
{
	out << "<CardRuKey(" << EnumToString(key.type) << ", " << key.russian << ")>";
	return out;
}
