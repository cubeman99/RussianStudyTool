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

EnumFlags<CardTags> Word::GetTags() const
{
	EnumFlags<CardTags> tags;
	return tags;
}

AccentedText Word::GetInfoString() const
{
	AccentedText info = m_text + " (" + EnumToString(m_wordType) + "):";
	return info;
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
	m_key = text.GetString();
	ru::ToLowerIP(m_key);
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

Map<WordType, Word::sptr>& Term::GetWords()
{
	return m_words;
}

void Term::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	value.AddMember("download_timestamp", int32(m_downloadTimestamp), allocator);
	json::AddMember(value, "text", m_text, allocator);
	json::AddMember(value, "etymology", m_etymology, allocator);

	rapidjson::Value wordListData(rapidjson::kObjectType);
	for (auto it : m_words)
	{
		rapidjson::Value wordData(rapidjson::kObjectType);
		it.second->Serialize(wordData, allocator);
		json::AddMember(wordListData, EnumToString(it.first), wordData, allocator);
	}
	value.AddMember("words", wordListData, allocator);
}

Error Term::Deserialize(rapidjson::Value& data)
{
	Error error;

	//m_text = AccentedText(data["text"].GetString());
	m_downloadTimestamp = data["download_timestamp"].GetDouble();

	if (data.HasMember("etymology"))
		json::Deserialize(data, "etymology", m_etymology);
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

EnumFlags<CardTags> Noun::GetTags() const
{
	EnumFlags<CardTags> tags;

	// Gender
	if (m_declension.GetGender() == Gender::k_masculine)
		tags.Set(CardTags::k_masculine, true);
	else if (m_declension.GetGender() == Gender::k_feminine)
		tags.Set(CardTags::k_feminine, true);
	else if (m_declension.GetGender() == Gender::k_neuter)
		tags.Set(CardTags::k_neuter, true);
	
	// Animacy
	if (m_declension.GetAnimacy() == Animacy::k_animate)
		tags.Set(CardTags::k_animate, true);
	else if (m_declension.GetAnimacy() == Animacy::k_inanimate)
		tags.Set(CardTags::k_inanimate, true);

	return tags;
}

AccentedText Noun::GetInfoString() const
{
	AccentedText info = m_text + " (" +
		EnumToString(m_declension.GetGender()) + ", " +
		EnumToString(m_declension.GetAnimacy()) + " " +
		EnumToString(m_wordType) + "):";
	return info;
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

EnumFlags<CardTags> Verb::GetTags() const
{
	EnumFlags<CardTags> tags;
	if (m_conjugation.GetAspect() == Aspect::k_perfective)
		tags.Set(CardTags::k_perfective, true);
	else if (m_conjugation.GetAspect() == Aspect::k_imperfective)
		tags.Set(CardTags::k_imperfective, true);
	if (m_conjugation.GetTransitivity() == Transitivity::k_reflexive)
		tags.Set(CardTags::k_reflexive, true);
	else if (m_conjugation.GetTransitivity() == Transitivity::k_intransitive)
		tags.Set(CardTags::k_intransitive, true);
	else if (m_conjugation.GetTransitivity() == Transitivity::k_transitive)
		tags.Set(CardTags::k_transitive, true);
	if (m_conjugation.IsImpersonal())
		tags.Set(CardTags::k_impersonal, true);
	if (m_conjugation.GetVerbClass().IsIrregular())
		tags.Set(CardTags::k_irregular, true);

	// Verb classification
	const auto& verbClass = m_conjugation.GetVerbClass();
	auto classNumber = verbClass.classNumber;
	auto variantIndex = verbClass.variantIndex;
	
	/*if (classNumber == 2)
		tags.Set(CardTags::k_verb_suffix_ova, true);
	else if (classNumber == 3 && variantIndex == 0)
		tags.Set(CardTags::k_verb_suffix_nu, true);
	else if (classNumber == 3 && variantIndex == 1)
		tags.Set(CardTags::k_verb_suffix_nu2, true);
	else if (classNumber == 4)
		tags.Set(CardTags::k_verb_suffix_i, true);
	else if (classNumber == 7 || classNumber == 8)
		tags.Set(CardTags::k_obstruent_stem, true);
	else if (classNumber == 9 || classNumber == 11 || classNumber == 14
		|| classNumber == 15 || classNumber == 16)
		tags.Set(CardTags::k_resonant_stem, true);
	else if (classNumber == 10)
		tags.Set(CardTags::k_verb_suffix_o, true);
	else if (classNumber == 13)
		tags.Set(CardTags::k_verb_suffix_avai, true);*/

	if (verbClass.IsValid())
	{
		if (verbClass.IsIrregular())
			tags.Set(CardTags::k_irregular, true);
		else if (verbClass.classNumber == 1)
			tags.Set(CardTags::k_verb_class_1, true);
		else if (verbClass.classNumber == 2)
			tags.Set(CardTags::k_verb_class_2, true);
		else if (verbClass.classNumber == 3 && verbClass.variantIndex == 0)
			tags.Set(CardTags::k_verb_class_3, true);
		else if (verbClass.classNumber == 3 && verbClass.variantIndex == 1)
			tags.Set(CardTags::k_verb_class_3o, true);
		else if (verbClass.classNumber == 4)
			tags.Set(CardTags::k_verb_class_4, true);
		else if (verbClass.classNumber == 5 && verbClass.variantIndex == 0)
			tags.Set(CardTags::k_verb_class_5, true);
		else if (verbClass.classNumber == 5 && verbClass.variantIndex == 1)
			tags.Set(CardTags::k_verb_class_5o, true);
		else if (verbClass.classNumber == 6 && verbClass.variantIndex == 0)
			tags.Set(CardTags::k_verb_class_6, true);
		else if (verbClass.classNumber == 6 && verbClass.variantIndex == 1)
			tags.Set(CardTags::k_verb_class_6o, true);
		else if (verbClass.classNumber == 7)
			tags.Set(CardTags::k_verb_class_7, true);
		else if (verbClass.classNumber == 8)
			tags.Set(CardTags::k_verb_class_8, true);
		else if (verbClass.classNumber == 9)
			tags.Set(CardTags::k_verb_class_9, true);
		else if (verbClass.classNumber == 10)
			tags.Set(CardTags::k_verb_class_10, true);
		else if (verbClass.classNumber == 11)
			tags.Set(CardTags::k_verb_class_11, true);
		else if (verbClass.classNumber == 12)
			tags.Set(CardTags::k_verb_class_12, true);
		else if (verbClass.classNumber == 13)
			tags.Set(CardTags::k_verb_class_13, true);
		else if (verbClass.classNumber == 14)
			tags.Set(CardTags::k_verb_class_14, true);
		else if (verbClass.classNumber == 15)
			tags.Set(CardTags::k_verb_class_15, true);
		else if (verbClass.classNumber == 16)
			tags.Set(CardTags::k_verb_class_16, true);
	
		if (verbClass.accentPattern == VerbAccentPattern::k_stem_stressed)
			tags.Set(CardTags::k_stem_stressed, true);
		else if (verbClass.accentPattern == VerbAccentPattern::k_ending_stressed)
			tags.Set(CardTags::k_ending_stressed, true);
		else if (verbClass.accentPattern == VerbAccentPattern::k_changing_stress)
			tags.Set(CardTags::k_shifting_stress, true);
	}

	// TODO: stem
	return tags;
}

AccentedText Verb::GetInfoString() const
{
	AccentedText info = m_text + " (";
	if (m_conjugation.GetVerbClass().IsValid())
		info += m_conjugation.GetVerbClass().ToString() + " ";
	info += EnumToString(m_conjugation.GetAspect()) + " ";
	if (m_conjugation.GetTransitivity() == Transitivity::k_reflexive)
		info += "reflexive ";
	else if (m_conjugation.GetTransitivity() == Transitivity::k_intransitive)
		info += "intransitive ";
	else if (m_conjugation.GetTransitivity() == Transitivity::k_transitive)
		info += "transitive ";
	if (m_conjugation.IsImpersonal())
		info += "impersonal ";
	info += EnumToString(m_wordType) + "):";
	return info;
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
