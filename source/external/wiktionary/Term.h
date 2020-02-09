#pragma once

#include "AccentedText.h"
#include "Types.h"

namespace wiki
{

class Definition
{
public:
	Definition();

	const AccentedText& GetDefinition() const { return  m_definition; }
	const Array<TranslationPair>& GetExamples() const { return  m_examples; }

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	AccentedText m_definition;
	Array<TranslationPair> m_examples;
};


class NounDeclension
{
public:
	NounDeclension();

	const AccentedText& GetDeclension(Case nounCase, Plurality plurality) const;
	void SetDeclension(Case nounCase, Plurality plurality, const AccentedText& text);

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	Map<std::pair<Case, Plurality>, AccentedText> m_declension;
};

using NounDeclension = NounDeclension;
using VerbConjugation = NounDeclension;
using AdjectiveDeclension = NounDeclension;


class Word
{
public:
	using sptr = cmg::shared_ptr<Word>;

	Word(WordType wordType);

	WordType GetWordType() const { return m_wordType; }
	const AccentedText& GetText() const { return m_text; }
	const AccentedText& GetEtymology() const { return  m_etymology; }
	const Array<Definition>& GetDefinitions() const { return  m_definitions; }
	const Set<AccentedText>& GetRelatedTerms() const { return m_relatedTerms; }
	const Set<AccentedText>& GetDerivedTerms() const { return m_derivedTerms; }
	const Set<AccentedText>& GetSynonyms() const { return m_synonyms; }
	const Set<AccentedText>& GetAntonyms() const { return m_antonyms; }

	virtual void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	virtual Error Deserialize(rapidjson::Value& data);

private:
	WordType m_wordType;
	AccentedText m_text;
	AccentedText m_etymology;
	Array<Definition> m_definitions;
	Set<AccentedText> m_relatedTerms;
	Set<AccentedText> m_derivedTerms;
	Set<AccentedText> m_antonyms;
	Set<AccentedText> m_synonyms;
};


class Noun : public Word
{
public:
	using sptr = cmg::shared_ptr<Noun>;

	Noun();

	const NounDeclension& GetDeclension() const { return m_declension; }

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator) override;
	Error Deserialize(rapidjson::Value& data) override;

private:
	NounDeclension m_declension;
};


class Adjective : public Word
{
public:
	using sptr = cmg::shared_ptr<Adjective>;

	Adjective();

	const AdjectiveDeclension& GetDeclension() const { return m_declension; }

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator) override;
	Error Deserialize(rapidjson::Value& data) override;

private:
	AdjectiveDeclension m_declension;
};


class Verb : public Word
{
public:
	using sptr = cmg::shared_ptr<Verb>;

	Verb();

	const VerbConjugation& GetConjugation() const { return m_conjugation; }

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator) override;
	Error Deserialize(rapidjson::Value& data) override;

private:
	VerbConjugation m_conjugation;
};


class Term
{
public:
	using sptr = cmg::shared_ptr<Term>;

	Term();

	const AccentedText& GetText() const { return m_text; }
	const AccentedText& GetEtymology() const { return  m_etymology; }
	AppTimestamp GetDownloadTimestamp() const { return m_downloadTimestamp; }
	Word::sptr GetWord(WordType wordType);
	const Word::sptr GetWord(WordType wordType) const;

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	AccentedText m_text;
	AccentedText m_etymology;
	AppTimestamp m_downloadTimestamp;
	Map<WordType, Word::sptr> m_words;
};


} // namespace wiki
