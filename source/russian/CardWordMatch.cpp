#include "CardWordMatch.h"


// Find the word
wiki::Word::sptr CardVariantWordMatchPart::SetWord(WordType wordType)
{
	word = nullptr;
	if (term)
	{
		word = term->GetWord(wordType);
		if (!word)
		{
			for (WordType wordType : EnumValues<WordType>())
			{
				word = term->GetWord(wordType);
				if (word)
					break;
			}
		}
	}
	isMatchingType = (word && word->GetWordType() == wordType);
	return word;
}


// Gets the first word
const CardVariantWordMatchPart* CardVariantWordMatch::GetPart() const
{
	for (const auto& part : parts)
	{
		if (part.word)
			return &part;
	}
	return nullptr;
}

// Gets the first word
wiki::Word::sptr CardVariantWordMatch::GetWord() const
{
	for (const auto& part : parts)
	{
		if (part.word)
			return part.word;
	}
	return nullptr;
}

// Returns true if all parts are downloaded
bool CardVariantWordMatch::IsFullyLoaded() const
{
	for (const auto& part : parts)
	{
		if (!part.isLoaded)
			return false;
	}
	return true;
}


bool CardWordMatch::IsMatchingType() const
{
	const CardVariantWordMatchPart* part = GetPart();
	return (part ? part->isMatchingType : false);
}

bool CardWordMatch::IsMatchingText() const
{
	const CardVariantWordMatchPart* part = GetPart();
	return (part ? part->isMatchingText : false);
}

// Gets the first matching variant word-part, or nullptr
const CardVariantWordMatchPart* CardWordMatch::GetPart() const
{
	for (const auto& variant : variants)
	{
		const CardVariantWordMatchPart* part = variant.GetPart();
		if (part)
			return part;
	}
	return nullptr;
}

// Gets the first term
wiki::Term::sptr CardWordMatch::GetTerm() const
{
	const CardVariantWordMatchPart* part = GetPart();
	return (part ? part->term : nullptr);
}

// Gets the first word
wiki::Word::sptr CardWordMatch::GetWord() const
{
	const CardVariantWordMatchPart* part = GetPart();
	return (part ? part->word : nullptr);
}

// Returns true if all variants/parts are downloaded
bool CardWordMatch::IsFullyLoaded() const
{
	for (const auto& variant : variants)
	{
		if (!variant.IsFullyLoaded())
			return false;
	}
	return true;
}

bool CardWordMatch::Empty() const
{
	for (const auto& variant : variants)
	{
		if (!variant.parts.empty())
			return false;
	}
	return true;
}
