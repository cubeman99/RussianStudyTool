#include "Verb.h"

namespace ru {

VerbConjugation::VerbConjugation() :
	m_aspect(Aspect::k_imperfective)
{
	for (uint32 index = 0; index < 6; index++)
		SetNonPast(index, AccentedText());
	for (Gender gender : EnumValues<Gender>())
		SetPast(gender, AccentedText());
	SetImperative(Plurality::k_singular, AccentedText());
	SetImperative(Plurality::k_plural, AccentedText());
	for (Participle participle : EnumValues<Participle>())
	{
		SetParticiple(participle, Tense::k_present, AccentedText());
		SetParticiple(participle, Tense::k_past, AccentedText());
	}
}

const AccentedText& VerbConjugation::GetInfinitive() const
{
	return m_infinitive;
}

Aspect VerbConjugation::GetAspect() const
{
	return m_aspect;
}

const AccentedText& VerbConjugation::GetNonPast(uint32 index) const
{
	CMG_ASSERT(index < 6);
	Person person = (Person) (index % 3);
	Plurality plurality = (Plurality) (index / 3);
	return GetNonPast(person, plurality);
}

const AccentedText& VerbConjugation::GetNonPast(Person person, Plurality plurality) const
{
	return m_nonPast.at({person, plurality});
}

const AccentedText& VerbConjugation::GetPast(uint32 index) const
{
	CMG_ASSERT(index < 4);
	return m_past.at((Gender) index);
}

const AccentedText& VerbConjugation::GetPast(Gender gender) const
{
	return m_past.at(gender);
}

const AccentedText& VerbConjugation::GetParticiple(Participle participle, Tense tense) const
{
	CMG_ASSERT(tense != Tense::k_future);
	return m_participles.at({participle, tense});
}

const AccentedText& VerbConjugation::GetImperative(Plurality plurality) const
{
	return m_imperative.at(plurality);
}

void VerbConjugation::SetInfinitive(const AccentedText& infinitive)
{
	m_infinitive = infinitive;
}

void VerbConjugation::SetAspect(Aspect aspect)
{
	m_aspect = aspect;
}

void VerbConjugation::SetNonPast(uint32 index, const AccentedText & text)
{
	CMG_ASSERT(index < 6);
	Person person = (Person) (index % 3);
	Plurality plurality = (Plurality) (index / 3);
	SetNonPast(person, plurality, text);
}

void VerbConjugation::SetNonPast(Person person, Plurality plurality, const AccentedText & text)
{
	m_nonPast[{person, plurality}] = text;
}

void VerbConjugation::SetPast(uint32 index, const AccentedText & text)
{
	CMG_ASSERT(index < 4);
	SetPast((Gender) index, text);
}

void VerbConjugation::SetPast(Gender gender, const AccentedText & text)
{
	m_past[gender] = text;
}

void VerbConjugation::SetParticiple(Participle participle, Tense tense, const AccentedText & text)
{
	CMG_ASSERT(tense != Tense::k_future);
	m_participles[{participle, tense}] = text;
}

void VerbConjugation::SetImperative(Plurality plurality, const AccentedText & text)
{
	m_imperative[plurality] = text;
}

void VerbConjugation::GetAllForms(Set<AccentedText>& outForms) const
{
	outForms.insert(m_infinitive);
	for (auto it : m_nonPast)
		outForms.insert(it.second);
	for (auto it : m_past)
		outForms.insert(it.second);
	for (auto it : m_imperative)
		outForms.insert(it.second);
	for (auto it : m_participles)
	{
		if (it.second.length() > 0)
			outForms.insert(it.second);
	}
}

void VerbConjugation::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	String form;

	// Infinitive
	form = ConvertToUTF8(m_infinitive.ToMarkedString());
	value.AddMember("infinitive", rapidjson::Value(
		form.c_str(), allocator).Move(), allocator);

	// Aspect
	String aspectName = EnumToString(m_aspect);
	value.AddMember("aspect", rapidjson::Value(
		aspectName.c_str(), allocator).Move(), allocator);

	// Non-past tense
	rapidjson::Value nonPastData(rapidjson::kArrayType);
	for (uint32 index = 0; index < 6; index++)
	{
		form = ConvertToUTF8(GetNonPast(index).ToMarkedString());
		nonPastData.PushBack(rapidjson::Value(
			form.c_str(), allocator).Move(), allocator);
	}
	value.AddMember("non_past", nonPastData, allocator);

	// Past tense
	rapidjson::Value pastData(rapidjson::kObjectType);
	for (Gender gender : EnumValues<Gender>())
	{
		String genderName = EnumToShortString(gender);
		form = ConvertToUTF8(GetPast(gender).ToMarkedString());
		pastData.AddMember(
			rapidjson::Value(genderName.c_str(), allocator).Move(),
			rapidjson::Value(form.c_str(), allocator).Move(), allocator);
	}
	value.AddMember("past", pastData, allocator);

	// Imperative
	rapidjson::Value imperativeData(rapidjson::kObjectType);
	for (Plurality plurality : EnumValues<Plurality>())
	{
		String pluralityName = EnumToShortString(plurality);
		form = ConvertToUTF8(GetImperative(plurality).ToMarkedString());
		imperativeData.AddMember(
			rapidjson::Value(pluralityName.c_str(), allocator).Move(),
			rapidjson::Value(form.c_str(), allocator).Move(), allocator);
	}
	value.AddMember("imperative", imperativeData, allocator);

	// Participles
	rapidjson::Value participleData(rapidjson::kObjectType);
	for (Tense tense : {Tense::k_present, Tense::k_past})
	{
		String tenseName = EnumToShortString(tense);
		rapidjson::Value tenseData(rapidjson::kObjectType);

		for (Participle participle : EnumValues<Participle>())
		{
			String participleName = EnumToShortString(participle);
			form = ConvertToUTF8(GetParticiple(
				participle, tense).ToMarkedString());
			tenseData.AddMember(
				rapidjson::Value(participleName.c_str(), allocator).Move(),
				rapidjson::Value(form.c_str(), allocator).Move(), allocator);
		}
		participleData.AddMember(
			rapidjson::Value(tenseName.c_str(), allocator).Move(),
			tenseData, allocator);
	}
	value.AddMember("participles", participleData, allocator);
}

Error VerbConjugation::Deserialize(rapidjson::Value& data)
{
	// Infinitive
	m_infinitive = data["infinitive"].GetString();

	// Aspect
	TryStringToEnum<Aspect>(data["aspect"].GetString(), m_aspect);

	// Non-past
	rapidjson::Value& nonPastData = data["non_past"];
	uint32 index = 0;
	for (auto it = nonPastData.Begin();
		it != nonPastData.End(); it++, index++)
	{
		SetNonPast(index, it->GetString());
	}

	// Past
	rapidjson::Value& pastData = data["past"];
	for (Gender gender : EnumValues<Gender>())
	{
		String genderName = EnumToShortString(gender);
		SetPast(gender, pastData[genderName.c_str()].GetString());
	}

	// Imperative
	rapidjson::Value& imperativeData = data["imperative"];
	for (Plurality plurality : EnumValues<Plurality>())
	{
		String pluralityName = EnumToShortString(plurality);
		SetImperative(plurality, imperativeData[pluralityName.c_str()].GetString());
	}

	// Participles
	rapidjson::Value& participleData = data["participles"];
	for (Tense tense : {Tense::k_present, Tense::k_past})
	{
		String tenseName = EnumToShortString(tense);
		rapidjson::Value& tenseData = participleData[tenseName.c_str()];
		for (Participle participle : EnumValues<Participle>())
		{
			String participleName = EnumToShortString(participle);
			SetParticiple(participle, tense,
				tenseData[participleName.c_str()].GetString());
		}
	}

	return CMG_ERROR_SUCCESS;
}

} // namespace ru