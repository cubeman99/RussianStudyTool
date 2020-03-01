#pragma once

#include "CardKey.h"

class Card;


struct CardData
{
	WordType type = WordType::k_other;
	TranslationPair text;
	EnumFlags<CardTags> tags;
	Set<cmg::shared_ptr<Card>> relatedCards;
};


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
	const TranslationPair& GetText() const;
	const AccentedText& GetText(Language language) const;
	AppTimestamp GetCreationTimestamp() const;
	CardRuKey GetRuKey() const;
	CardEnKey GetEnKey() const;
	Set<Card::sptr>& GetRelatedCards();
	EnumFlags<CardTags>& GetTags();
	Array<unistr> GetWordPatterns() const;
	Array<unistr> GetWordNames() const;

	void SetData(const CardData& data);
	CardData GetData();

private:
	WordType m_type = WordType::k_other;
	TranslationPair m_text;
	EnumFlags<CardTags> m_tags;
	Array<TranslationPair> m_examples;
	Set<Card::sptr> m_relatedCards;
	AppTimestamp m_creationTimestamp = -1.0f;

	// Used by CardDatabase
	Set<cmg::shared_ptr<CardSet>> m_cardSets;
};
