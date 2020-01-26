#pragma once

#include "AccentedText.h"
#include "Types.h"

struct CardEnKey
{
	WordType type;
	unistr english;
	uint32 keyTags;

	bool operator <(const CardEnKey& other) const
	{
		auto a = std::tuple<uint32, unistr, uint32>(
			(uint32) type, english, keyTags);
		auto b = std::tuple<uint32, unistr, uint32>(
			(uint32) other.type, other.english, other.keyTags);
		return a < b;
	}
};

struct CardRuKey
{
	WordType type;
	unistr russian;

	CardRuKey()
	{}
	CardRuKey(WordType type, const unistr& russian) :
		type(type),
		russian(russian)
	{}

	bool operator <(const CardRuKey& other) const
	{
		auto a = std::tuple<uint32, unistr>(
			(uint32) type, russian);
		auto b = std::tuple<uint32, unistr>(
			(uint32) other.type, other.russian);
		return a < b;
	}
};

struct CardKey
{
	WordType type;
	unistr russian;
	unistr english;

	bool operator <(const CardKey& other) const;
};

std::ostream& operator <<(std::ostream &out, const CardKey& key);
std::ostream& operator <<(std::ostream &out, const CardEnKey& key);
std::ostream& operator <<(std::ostream &out, const CardRuKey& key);


class Card
{
public:
	friend class CardDatabase;
	using sptr = cmg::shared_ptr<Card>;

	Card();
	Card(WordType type, AccentedText russian, AccentedText english);
	~Card();

	WordType GetWordType() const { return m_type; }
	const AccentedText& GetEnglish() const { return m_text.english; }
	const AccentedText& GetRussian() const { return m_text.russian; }
	const AccentedText& GetText(Language language) const;
	CardKey GetKey() const;
	CardRuKey GetRuKey() const;
	CardEnKey GetEnKey() const;

private:
	WordType m_type = WordType::k_other;
	TranslationPair m_text;
	EnumFlags<CardTags> m_tags;
	Array<TranslationPair> m_examples;
	Set<Card::sptr> m_relatedCards;
	float32 m_creationTimestamp = 0.0f;

	// Used by CardDatabase
	Set<cmg::shared_ptr<CardSet>> m_cardSets;
};
