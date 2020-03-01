#include "Wiktionary.h"
#include "core/JsonUtility.h"

namespace wiki
{

Definition::Definition()
{
}

void Definition::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	String text = ConvertToUTF8(m_definition.ToMarkedString());
	value.AddMember("definition", rapidjson::Value(
		text.c_str(), allocator).Move(), allocator);

	if (!m_examples.empty())
	{
		rapidjson::Value exampleListData(rapidjson::kArrayType);
		for (TranslationPair& example : m_examples)
		{
			rapidjson::Value exampleData(rapidjson::kObjectType);
			text = ConvertToUTF8(example.russian.ToMarkedString());
			exampleData.AddMember("ru", rapidjson::Value(
				text.c_str(), allocator).Move(), allocator);
			text = ConvertToUTF8(example.english.ToMarkedString());
			exampleData.AddMember("en", rapidjson::Value(
				text.c_str(), allocator).Move(), allocator);
			exampleListData.PushBack(exampleData, allocator);
		}
		value.AddMember("examples", exampleListData, allocator);
	}
}

Error Definition::Deserialize(rapidjson::Value& data)
{
	m_definition = AccentedText(data["definition"].GetString());

	// Deserialize examples
	if (data.HasMember("examples"))
	{
		rapidjson::Value& exampleDataList = data["examples"];
		for (auto it = exampleDataList.Begin();
			it != exampleDataList.End(); it++)
		{
			rapidjson::Value& exampleData = *it;
			TranslationPair example;
			example.russian = AccentedText(exampleData["ru"].GetString());
			example.english = AccentedText(exampleData["en"].GetString());
			m_examples.push_back(example);
		}
	}

	return CMG_ERROR_SUCCESS;
}


Word::Word(WordType wordType) :
	m_wordType(wordType)
{
}

void Word::GetAllForms(Set<AccentedText>& outForms) const
{
	outForms.insert(m_text);
}

void Word::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	String text = ConvertToUTF8(m_text.ToMarkedString());
	value.AddMember("text", rapidjson::Value(
		text.c_str(), allocator).Move(), allocator);

	String etymology = ConvertToUTF8(m_etymology.ToMarkedString());
	value.AddMember("etymology", rapidjson::Value(
		etymology.c_str(), allocator).Move(), allocator);

	// Definitions
	rapidjson::Value definitionListData(rapidjson::kArrayType);
	for (Definition& definition : m_definitions)
	{
		rapidjson::Value definitionData(rapidjson::kObjectType);
		definition.Serialize(definitionData, allocator);
		definitionListData.PushBack(definitionData, allocator);
	}
	value.AddMember("definitions", definitionListData, allocator);

	// Related terms
	rapidjson::Value relatedTermsListData(rapidjson::kArrayType);
	for (const AccentedText& term : m_relatedTerms)
	{
		rapidjson::Value termData(rapidjson::kObjectType);
		String termName = ConvertToUTF8(term.ToMarkedString());
		relatedTermsListData.PushBack(rapidjson::Value(
			termName.c_str(), allocator).Move(), allocator);
	}
	value.AddMember("related_terms", relatedTermsListData, allocator);

	// Derived terms
	rapidjson::Value derivedTermsListData(rapidjson::kArrayType);
	for (const AccentedText& term : m_derivedTerms)
	{
		rapidjson::Value termData(rapidjson::kObjectType);
		String termName = ConvertToUTF8(term.ToMarkedString());
		derivedTermsListData.PushBack(rapidjson::Value(
			termName.c_str(), allocator).Move(), allocator);
	}
	value.AddMember("derived_terms", derivedTermsListData, allocator);

	// Synonyms
	rapidjson::Value synonymsListData(rapidjson::kArrayType);
	for (const AccentedText& term : m_synonyms)
	{
		rapidjson::Value termData(rapidjson::kObjectType);
		String termName = ConvertToUTF8(term.ToMarkedString());
		synonymsListData.PushBack(rapidjson::Value(
			termName.c_str(), allocator).Move(), allocator);
	}
	value.AddMember("synonyms", synonymsListData, allocator);

	// Antonyms
	rapidjson::Value antonymsListData(rapidjson::kArrayType);
	for (const AccentedText& term : m_antonyms)
	{
		rapidjson::Value termData(rapidjson::kObjectType);
		String termName = ConvertToUTF8(term.ToMarkedString());
		antonymsListData.PushBack(rapidjson::Value(
			termName.c_str(), allocator).Move(), allocator);
	}
	value.AddMember("antonyms", antonymsListData, allocator);
}

Error Word::Deserialize(rapidjson::Value& data)
{
	Error error;
	m_definitions.clear();
	m_derivedTerms.clear();
	m_relatedTerms.clear();
	m_antonyms.clear();
	m_synonyms.clear();
	m_etymology = AccentedText("");

	m_text = AccentedText(data["text"].GetString());

	// Deserialize definitions
	rapidjson::Value& definitionsDataList = data["definitions"];
	for (auto it = definitionsDataList.Begin();
		it != definitionsDataList.End(); it++)
	{
		rapidjson::Value& definitionData = *it;
		Definition definition;
		error = definition.Deserialize(definitionData);
		if (error.Failed())
			return error.Uncheck();
		m_definitions.push_back(definition);
	}

	// Deserialize related terms
	if (data.HasMember("related_terms"))
	{
		rapidjson::Value& relatedTermList = data["related_terms"];
		for (auto it = relatedTermList.Begin(); it != relatedTermList.End(); it++)
			m_relatedTerms.insert(AccentedText(it->GetString()));
	}
	if (data.HasMember("derived_terms"))
	{
		rapidjson::Value& derivedTermList = data["derived_terms"];
		for (auto it = derivedTermList.Begin(); it != derivedTermList.End(); it++)
			m_derivedTerms.insert(AccentedText(it->GetString()));
	}
	if (data.HasMember("synonyms"))
	{
		rapidjson::Value& synonymTermList = data["synonyms"];
		for (auto it = synonymTermList.Begin(); it != synonymTermList.End(); it++)
			m_synonyms.insert(AccentedText(it->GetString()));
	}
	if (data.HasMember("antonyms"))
	{
		rapidjson::Value& antonymTermList = data["antonyms"];
		for (auto it = antonymTermList.Begin(); it != antonymTermList.End(); it++)
			m_antonyms.insert(AccentedText(it->GetString()));
	}

	return CMG_ERROR_SUCCESS;
}


Term::Term()
{
}

Term::Term(const AccentedText& text) :
	m_text(text)
{
}

Word::sptr Term::GetWord(WordType wordType)
{
	auto it = m_words.find(wordType);
	if (it != m_words.end())
		return it->second;
	return nullptr;
}

const Word::sptr Term::GetWord(WordType wordType) const
{
	auto it = m_words.find(wordType);
	if (it != m_words.end())
		return it->second;
	return nullptr;
}

void Term::GetAllForms(Set<AccentedText>& outForms) const
{
	for (auto it : m_words)
		it.second->GetAllForms(outForms);
}

void Term::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	value.AddMember("download_timestamp", m_downloadTimestamp, allocator);

	String text = ConvertToUTF8(m_text.ToMarkedString());
	value.AddMember("text", rapidjson::Value(
		text.c_str(), allocator).Move(), allocator);

	String etymology = ConvertToUTF8(m_etymology.ToMarkedString());
	value.AddMember("etymology", rapidjson::Value(
		etymology.c_str(), allocator).Move(), allocator);

	rapidjson::Value wordListData(rapidjson::kObjectType);
	for (auto it : m_words)
	{
		rapidjson::Value wordData(rapidjson::kObjectType);
		String wordTypeName = EnumToString(it.first);
		Word::sptr word = it.second;
		word->Serialize(wordData, allocator);
		wordListData.AddMember(rapidjson::Value(
			wordTypeName.c_str(), allocator).Move(), wordData, allocator);
	}
	value.AddMember("words", wordListData, allocator);
}

Error Term::Deserialize(rapidjson::Value& data)
{
	Error error;

	//m_text = AccentedText(data["text"].GetString());
	m_downloadTimestamp = data["download_timestamp"].GetDouble();

	if (data.HasMember("etymology"))
		m_etymology = AccentedText(data["etymology"].GetString());
	else
		m_etymology = "";

	m_words.clear();
	rapidjson::Value& wordsDataList = data["words"];
	for (auto it = wordsDataList.MemberBegin(); it != wordsDataList.MemberEnd(); it++)
	{
		// Get word type
		WordType wordType;
		TryStringToEnum<WordType>(String(it->name.GetString()), wordType);

		// Deserialize word data
		Word::sptr word;
		if (wordType == WordType::k_noun)
			word = cmg::make_shared<Noun>();
		else if (wordType == WordType::k_adjective)
			word = cmg::make_shared<Adjective>();
		else if (wordType == WordType::k_verb)
			word = cmg::make_shared<Verb>();
		else
			word = cmg::make_shared<Word>(wordType);
		error = word->Deserialize(it->value);
		if (error.Failed())
			return error.Uncheck();

		m_words[wordType] = word;
	}

	return CMG_ERROR_SUCCESS;
}



Noun::Noun() :
	Word(WordType::k_noun)
{
}

void Noun::GetAllForms(Set<AccentedText>& outForms) const
{
	m_declension.GetAllForms(outForms);
}

void Noun::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	Word::Serialize(value, allocator);
	rapidjson::Value declensionData(rapidjson::kObjectType);
	m_declension.Serialize(declensionData, allocator);
	value.AddMember("declension", declensionData, allocator);
}

Error Noun::Deserialize(rapidjson::Value& data)
{
	Error error = Word::Deserialize(data);
	if (error.Failed())
		return error.Uncheck();
	if (data.HasMember("declension"))
	{
		error = m_declension.Deserialize(data["declension"]);
		if (error.Failed())
			return error.Uncheck();
	}
	return CMG_ERROR_SUCCESS;
}


Adjective::Adjective() :
	Word(WordType::k_adjective)
{
}

void Adjective::GetAllForms(Set<AccentedText>& outForms) const
{
	m_declension.GetAllForms(outForms);
}

void Adjective::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	Word::Serialize(value, allocator);
	rapidjson::Value declensionData(rapidjson::kObjectType);
	m_declension.Serialize(declensionData, allocator);
	value.AddMember("declension", declensionData, allocator);
}

Error Adjective::Deserialize(rapidjson::Value& data)
{
	Error error = Word::Deserialize(data);
	if (data.HasMember("declension"))
	{
		error = m_declension.Deserialize(data["declension"]);
		if (error.Failed())
			return error.Uncheck();
	}
	return CMG_ERROR_SUCCESS;
}


Verb::Verb() :
	Word(WordType::k_verb)
{
}

void Verb::GetAllForms(Set<AccentedText>& outForms) const
{
	m_conjugation.GetAllForms(outForms);
}

void Verb::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	Word::Serialize(value, allocator);
	rapidjson::Value conjugationData(rapidjson::kObjectType);
	m_conjugation.Serialize(conjugationData, allocator);
	value.AddMember("conjugation", conjugationData, allocator);
	if (!m_counterparts.empty())
	{
		rapidjson::Value counterpartsData(rapidjson::kArrayType);
		for (auto counterpart : m_counterparts)
		{
			String text = ConvertToUTF8(counterpart.ToMarkedString());
			counterpartsData.PushBack(rapidjson::Value(
				text.c_str(), allocator).Move(), allocator);
		}
		value.AddMember("counterparts", counterpartsData, allocator);
	}
}

Error Verb::Deserialize(rapidjson::Value& data)
{
	Error error = Word::Deserialize(data);
	if (error.Failed())
		return error.Uncheck();
	if (data.HasMember("conjugation"))
	{
		error = m_conjugation.Deserialize(data["conjugation"]);
		if (error.Failed())
			return error.Uncheck();
	}
	if (data.HasMember("counterparts"))
	{
		m_counterparts.clear();
		rapidjson::Value& counterpartsData = data["counterparts"];
		for (auto it = counterpartsData.Begin();
			it != counterpartsData.End(); it++)
		{
			m_counterparts.insert(AccentedText(it->GetString()));
		}
	}
	return CMG_ERROR_SUCCESS;
}


} // namespace wiki
