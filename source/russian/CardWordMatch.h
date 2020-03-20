#pragma once

#include "cards/Card.h"
#include "external/wiktionary/Term.h"


struct CardVariantWordMatchPart
{
	unistr text;
	bool isLoaded = false;
	wiki::Word::sptr word;
	wiki::Term::sptr term;
	bool isMatchingText = false;
	bool isMatchingType = false;

	// Find the word
	wiki::Word::sptr SetWord(WordType wordType);
};


struct CardVariantWordMatch
{
	unistr text;
	Array<CardVariantWordMatchPart> parts;

	// Gets the first word
	const CardVariantWordMatchPart* GetPart() const;

	// Gets the first word
	wiki::Word::sptr GetWord() const;

	// Returns true if all parts are downloaded
	bool IsFullyLoaded() const;
};


struct CardWordMatch
{
	Card::sptr card;
	WordType wordType;
	unistr text;
	Array<CardVariantWordMatch> variants;

	bool IsMatchingType() const;

	bool IsMatchingText() const;

	// Gets the first matching variant word-part, or nullptr
	const CardVariantWordMatchPart* GetPart() const;

	// Gets the first term
	wiki::Term::sptr GetTerm() const;

	// Gets the first word
	wiki::Word::sptr GetWord() const;

	// Returns true if all variants/parts are downloaded
	bool IsFullyLoaded() const;

	bool Empty() const;
};
