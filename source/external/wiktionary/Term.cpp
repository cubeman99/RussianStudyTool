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

void Word::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
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

void Term::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
}

Error Term::Deserialize(rapidjson::Value& data)
{
	Error error;

	m_text = AccentedText(data["text"].GetString());
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

NounDeclension::NounDeclension()
{
	for (Case nounCase : EnumValues<Case>())
	{
		for (Plurality plurality : EnumValues<Plurality>())
			SetDeclension(nounCase, plurality, "");
	}
}

const AccentedText& NounDeclension::GetDeclension(
	Case nounCase, Plurality plurality) const
{
	return m_declension.at({nounCase, plurality});
}

void NounDeclension::SetDeclension(Case nounCase, Plurality plurality,
	const AccentedText& text)
{
	m_declension[{nounCase, plurality}] = text;
}

void NounDeclension::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	for (Plurality plurality : EnumValues<Plurality>())
	{
		String pluralityName = EnumToShortString(plurality);
		rapidjson::Value pluralityData(rapidjson::kObjectType);
		for (Case nounCase : EnumValues<Case>())
		{
			String caseName = EnumToShortString(nounCase);
			String text = ConvertToUTF8(GetDeclension(
				nounCase, plurality).ToMarkedString());
			pluralityData.AddMember(
				rapidjson::Value(caseName.c_str(), allocator).Move(),
				rapidjson::Value(text.c_str(), allocator).Move(), allocator);
		}
		value.AddMember(
			rapidjson::Value(pluralityName.c_str(), allocator).Move(),
			pluralityData, allocator);
	}
}

Error NounDeclension::Deserialize(rapidjson::Value& data)
{
	for (Plurality plurality : EnumValues<Plurality>())
	{
		String pluralityName = EnumToShortString(plurality);
		rapidjson::Value& pluralityData = data[pluralityName.c_str()];
		for (Case nounCase : EnumValues<Case>())
		{
			String caseName = EnumToShortString(nounCase);
			AccentedText text = pluralityData[caseName.c_str()].GetString();
			SetDeclension(nounCase, plurality, text);
		}
	}
	return CMG_ERROR_SUCCESS;
}

Noun::Noun() :
	Word(WordType::k_noun)
{
}

void Noun::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	Word::Serialize(value, allocator);
	m_declension.Serialize(value, allocator);
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

void Adjective::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	Word::Serialize(value, allocator);
	m_declension.Serialize(value, allocator);
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

void Verb::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	Word::Serialize(value, allocator);
	m_conjugation.Serialize(value, allocator);
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
	return CMG_ERROR_SUCCESS;
}


} // namespace wiki
