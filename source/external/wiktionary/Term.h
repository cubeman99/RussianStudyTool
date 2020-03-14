#pragma once

#include "russian/Noun.h"
#include "russian/Verb.h"
#include "russian/Adjective.h"

namespace wiki
{

class Definition
{
public:
	friend class Parser;

	Definition();

	const AccentedText& GetDefinition() const { return  m_definition; }
	const Array<TranslationPair>& GetExamples() const { return m_examples; }
	Array<TranslationPair>& GetExamples() { return  m_examples; }

	void SetDefinition(const AccentedText& definition) { m_definition = definition; }

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	AccentedText m_definition;
	Array<TranslationPair> m_examples;
};


class Word
{
public:
	friend class Parser;
	using sptr = cmg::shared_ptr<Word>;

	Word(WordType wordType);

	WordType GetWordType() const { return m_wordType; }
	const AccentedText& GetText() const { return m_text; }
	const AccentedText& GetEtymology() const { return  m_etymology; }
	const Array<Definition>& GetDefinitions() const { return m_definitions; }
	Array<Definition>& GetDefinitions() { return m_definitions; }
	const Set<AccentedText>& GetRelatedTerms() const { return m_relatedTerms; }
	Set<AccentedText>& GetRelatedTerms() { return m_relatedTerms; }
	const Set<AccentedText>& GetDerivedTerms() const { return m_derivedTerms; }
	Set<AccentedText>& GetDerivedTerms() { return m_derivedTerms; }
	const Set<AccentedText>& GetSynonyms() const { return m_synonyms; }
	Set<AccentedText>& GetSynonyms() { return m_synonyms; }
	const Set<AccentedText>& GetAntonyms() const { return m_antonyms; }
	Set<AccentedText>& GetAntonyms() { return m_antonyms; }

	void SetText(const AccentedText& text) { m_text = text; }
	void SetEtymology(const AccentedText& etymology) { m_etymology = etymology; }

	virtual EnumFlags<CardTags> GetTags() const;
	virtual void GetAllForms(Set<AccentedText>& outForms) const;
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
	friend class Parser;
	using sptr = cmg::shared_ptr<Noun>;

	Noun();
	
	const ru::NounDeclension& GetDeclension() const { return m_declension; }
	ru::NounDeclension& GetDeclension() { return m_declension; }

	void SetDeclension(const ru::NounDeclension& declension) { m_declension = declension; }

	virtual EnumFlags<CardTags> GetTags() const override;
	virtual void GetAllForms(Set<AccentedText>& outForms) const;
	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator) override;
	Error Deserialize(rapidjson::Value& data) override;

private:
	ru::NounDeclension m_declension;
};


class Adjective : public Word
{
public:
	friend class Parser;
	using sptr = cmg::shared_ptr<Adjective>;

	Adjective();

	const ru::AdjectiveDeclension& GetDeclension() const { return m_declension; }

	void SetDeclension(const ru::AdjectiveDeclension& declension) { m_declension = declension; }

	virtual void GetAllForms(Set<AccentedText>& outForms) const;
	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator) override;
	Error Deserialize(rapidjson::Value& data) override;

private:
	ru::AdjectiveDeclension m_declension;
};


class Verb : public Word
{
public:
	friend class Parser;
	using sptr = cmg::shared_ptr<Verb>;

	Verb();

	const ru::VerbConjugation& GetConjugation() const { return m_conjugation; }
	const Set<AccentedText>& GetCounterparts() const { return m_counterparts; }
	Set<AccentedText>& GetCounterparts() { return m_counterparts; }

	void SetConjugation(const ru::VerbConjugation& conjugation) { m_conjugation = conjugation; }

	virtual EnumFlags<CardTags> GetTags() const override;
	virtual void GetAllForms(Set<AccentedText>& outForms) const;
	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator) override;
	Error Deserialize(rapidjson::Value& data) override;

private:
	Set<AccentedText> m_counterparts;
	ru::VerbConjugation m_conjugation;
};


class Term
{
public:
	friend class Parser;
	using sptr = cmg::shared_ptr<Term>;

	Term();
	Term(const AccentedText& text);

	const AccentedText& GetText() const { return m_text; }
	const AccentedText& GetEtymology() const { return  m_etymology; }
	AppTimestamp GetDownloadTimestamp() const { return m_downloadTimestamp; }
	Word::sptr GetWord(WordType wordType);
	const Word::sptr GetWord(WordType wordType) const;
	void GetAllForms(Set<AccentedText>& outForms) const;
	Map<WordType, Word::sptr>& GetWords();

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
