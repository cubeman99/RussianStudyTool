#include "CardKey.h"

CardRuKey CardKey::GetRuKey() const
{
	return CardRuKey(type, russian);
}

bool CardKey::operator<(const CardKey & other) const
{
	auto a = std::tuple<uint32, unistr, unistr>(
		(uint32) type, russian, english);
	auto b = std::tuple<uint32, unistr, unistr>(
		(uint32) other.type, other.russian, other.english);
	return a < b;
}

bool CardKey::operator==(const CardKey & other) const
{
	return (type == other.type &&
		russian == other.russian &&
		english == other.english);
}

bool CardKey::operator!=(const CardKey & other) const
{
	return !(*this == other);
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

CardRuKey::CardRuKey()
{
}

CardRuKey::CardRuKey(WordType type, const unistr& russian) :
	type(type),
	russian(russian)
{
}

CardRuKey::CardRuKey(WordType inType, const AccentedText& inRussian) :
	CardRuKey(inType, inRussian.GetString())
{
	ru::ToLowerIP(russian);
}

bool CardRuKey::operator<(const CardRuKey& other) const
{
	auto a = std::tuple<uint32, unistr>(
		(uint32) type, russian);
	auto b = std::tuple<uint32, unistr>(
		(uint32) other.type, other.russian);
	return a < b;
}

bool CardRuKey::operator==(const CardRuKey& other) const
{
	return (type == other.type && russian == other.russian);
}

bool CardRuKey::operator!=(const CardRuKey& other) const
{
	return !(*this == other);
}

CardEnKey::CardEnKey()
{
}

CardEnKey::CardEnKey(WordType inType, const unistr& inEnglish, uint32 inKeyTags) :
	type(inType),
	english(inEnglish),
	keyTags(inKeyTags)
{
}

CardEnKey::CardEnKey(WordType inType, const AccentedText& inEnglish, const EnumFlags<CardTags>& tags) :
	CardEnKey(inType, inEnglish.GetString(), tags.GetValue(0))
{
	ru::ToLowerIP(english);
}

bool CardEnKey::operator<(const CardEnKey& other) const
{
	auto a = std::tuple<uint32, unistr, uint32>(
		(uint32) type, english, keyTags);
	auto b = std::tuple<uint32, unistr, uint32>(
		(uint32) other.type, other.english, other.keyTags);
	return a < b;
}

bool CardEnKey::operator==(const CardEnKey& other) const
{
	return (type == other.type &&
		keyTags == other.keyTags &&
		english == other.english);
}

bool CardEnKey::operator!=(const CardEnKey& other) const
{
	return !(*this == other);
}
