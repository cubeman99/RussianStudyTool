#pragma once

#include "russian/Russian.h"
#include "examples/Story.h"
#include "external/wiktionary/Wiktionary.h"
#include "cards/Card.h"
#include <mutex>

struct WordMatchPattern
{
	// List of words, each word being a set of possible forms
	Array<Set<unistr>> words;
};

struct SentenceMatch
{
	AccentedText sentence;
	Array<std::pair<uint32, uint32>> locations;
};


class ExampleDatabase
{
public:
	ExampleDatabase(wiki::Wiktionary& wiktionary);
	~ExampleDatabase();

	Array<SentenceMatch> GetExampleSentences(Card::sptr card);
	Array<SentenceMatch> GetExampleSentences(const Array<WordMatchPattern>& words);
	uint32 GetExampleSentences(const Array<WordMatchPattern> &words, Array<SentenceMatch>& outMatches);
	static uint32 ExampleDatabase::GetExamplesInSentence(
		const AccentedText& sentence, const Array<WordMatchPattern>& words,
		Array<std::pair<uint32, uint32>>& outMatchLocations);
	Set<unistr> GetWordForms(const unistr& word);

	Error LoadExamples(const PathU16& path);
	Error LoadStory(const PathU16& path, Story::sptr& outStory);

private:
	Error LoadExamplesInDir(const PathU16& path);

	ExampleDatabase(const ExampleDatabase& copy) = delete;
	ExampleDatabase& operator=(const ExampleDatabase& copy) = delete;

private:
	wiki::Wiktionary& m_wiktionary;
	Array<Story::sptr> m_stories;
};
