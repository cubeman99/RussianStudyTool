#include "WordDatabase.h"

WordDatabase::WordDatabase(wiki::Wiktionary& wiktionary) :
	m_wiktionary(wiktionary)
{
}

WordDatabase::~WordDatabase()
{
}

wiki::Wiktionary& WordDatabase::GetWiktionary()
{
	return m_wiktionary;
}

bool WordDatabase::GetWordFromCard(Card::sptr card,
	wiki::Term::sptr& outTerm, wiki::Word::sptr& outWord)
{
	outTerm = nullptr;
	outWord = nullptr;

	Array<unistr> wordNames;
	for (const unistr& pattern : card->GetWordPatterns())
		wordNames.push_back(pattern);
	for (const unistr& pattern : card->GetWordNames())
		wordNames.push_back(pattern);
	if (wordNames.empty())
		return false;

	// Find the term
	for (uint32 i = 0; i < wordNames.size() && !outTerm; i++)
	{
		outTerm = m_wiktionary.GetTerm(wordNames[i]);
		if (!outTerm)
			outTerm = m_wiktionary.DownloadTerm(wordNames[i]);
	}
	if (!outTerm)
		return false;

	// Find the word
	outWord = outTerm->GetWord(card->GetWordType());
	if (!outWord)
	{
		for (WordType wordType : EnumValues<WordType>())
		{
			outWord = outTerm->GetWord(wordType);
			if (outWord)
				break;
		}
	}

	return true;
}
