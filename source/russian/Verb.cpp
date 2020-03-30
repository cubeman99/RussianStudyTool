#include "Verb.h"

namespace ru {

VerbConjugation::VerbConjugation() :
	m_aspect(Aspect::k_imperfective)
{
	auto genderList = {
		Gender::k_masculine,
		Gender::k_feminine,
		Gender::k_neuter,
		Gender::k_plural,
	};

	for (uint32 index = 0; index < 6; index++)
		SetNonPast(index, AccentedText());
	for (Gender gender : genderList)
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

Transitivity VerbConjugation::GetTransitivity() const
{
	return m_transitivity;
}

bool VerbConjugation::IsImpersonal() const
{
	return m_isImpersonal;
}

const VerbConjugationClass & VerbConjugation::GetVerbClass() const
{
	return m_verbClass;
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

void VerbConjugation::SetVerbClass(const VerbConjugationClass & verbClass)
{
	m_verbClass = verbClass;
}

void VerbConjugation::SetTransitivity(Transitivity transitivity)
{
	m_transitivity = transitivity;
}

void VerbConjugation::SetImpersonal(bool impersonal)
{
	m_isImpersonal = impersonal;
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

void VerbConjugation::Serialize(json::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	String form;

	auto genderList = {
		Gender::k_masculine,
		Gender::k_feminine,
		Gender::k_neuter,
		Gender::k_plural,
	};

	// Infinitive
	json::Serialize(value, "infinitive", m_infinitive, allocator);

	// Aspect
	json::SerializeEnum(value, "aspect", m_aspect, allocator);

	// Transitivity
	if (m_transitivity != Transitivity::k_unknown)
		json::SerializeEnum(value, "transitivity", m_transitivity, allocator);

	if (m_isImpersonal)
		value.AddMember("impersonal", m_isImpersonal, allocator);

	// Non-past tense
	json::Value nonPastData(rapidjson::kArrayType);
	for (uint32 index = 0; index < 6; index++)
		json::PushBack(nonPastData, GetNonPast(index), allocator);
	value.AddMember("non_past", nonPastData, allocator);

	// Past tense
	json::Value pastData(rapidjson::kObjectType);
	for (Gender gender : genderList)
		json::AddMember(pastData, EnumToShortString(gender),
			GetPast(gender), allocator);
	value.AddMember("past", pastData, allocator);

	// Imperative
	json::Value imperativeData(rapidjson::kObjectType);
	for (Plurality plurality : {Plurality::k_singular, Plurality::k_plural})
		json::AddMember(imperativeData, EnumToShortString(plurality),
			GetImperative(plurality), allocator);
	value.AddMember("imperative", imperativeData, allocator);

	// Participles
	json::Value participleData(rapidjson::kObjectType);
	for (Tense tense : {Tense::k_present, Tense::k_past})
	{
		json::Value tenseData(rapidjson::kObjectType);
		for (Participle participle : EnumValues<Participle>())
			json::AddMember(tenseData, EnumToShortString(participle),
				GetParticiple(participle, tense), allocator);
		json::AddMember(participleData, EnumToShortString(tense), tenseData, allocator);
	}
	value.AddMember("participles", participleData, allocator);

	// Verb class
	if (m_verbClass.IsValid())
	{
		json::Value verbClassData(rapidjson::kObjectType);
		verbClassData.AddMember("class", m_verbClass.classNumber, allocator);
		verbClassData.AddMember("variant", m_verbClass.variantIndex, allocator);
		json::AddMember(verbClassData, "accent", EnumToShortString(m_verbClass.accentPattern), allocator);
		value.AddMember("verb_class", verbClassData, allocator);
	}
}

Error VerbConjugation::Deserialize(json::Value& data)
{
	auto genderList = {
		Gender::k_masculine,
		Gender::k_feminine,
		Gender::k_neuter,
		Gender::k_plural,
	};

	json::Deserialize(data, "infinitive", m_infinitive);
	json::DeserializeEnum(data, "aspect", m_aspect);
	json::DeserializeEnum(data, "transitivity", m_transitivity);
	json::DeserializeBool(data, "impersonal", m_isImpersonal);

	// Non-past
	json::Value& nonPastData = data["non_past"];
	uint32 index = 0;
	for (auto it = nonPastData.Begin();
		it != nonPastData.End(); it++, index++)
	{
		SetNonPast(index, it->GetString());
	}

	// Past
	json::Value& pastData = data["past"];
	for (Gender gender : genderList)
	{
		String genderName = EnumToShortString(gender);
		SetPast(gender, pastData[genderName.c_str()].GetString());
	}

	// Imperative
	json::Value& imperativeData = data["imperative"];
	for (Plurality plurality : {Plurality::k_singular, Plurality::k_plural})
	{
		String pluralityName = EnumToShortString(plurality);
		SetImperative(plurality, imperativeData[pluralityName.c_str()].GetString());
	}

	// Participles
	json::Value& participleData = data["participles"];
	for (Tense tense : {Tense::k_present, Tense::k_past})
	{
		String tenseName = EnumToShortString(tense);
		json::Value& tenseData = participleData[tenseName.c_str()];
		for (Participle participle : EnumValues<Participle>())
		{
			String participleName = EnumToShortString(participle);
			SetParticiple(participle, tense,
				tenseData[participleName.c_str()].GetString());
		}
	}

	// Verb class
	if (data.HasMember("verb_class"))
	{
		json::Value& verbClassData = data["verb_class"];
		m_verbClass.classNumber = verbClassData["class"].GetInt();
		if (verbClassData.HasMember("variant"))
			m_verbClass.variantIndex = verbClassData["variant"].GetInt();
		json::DeserializeShortEnum(verbClassData, "accent", m_verbClass.accentPattern);
	}

	return CMG_ERROR_SUCCESS;
}

bool VerbConjugationClass::IsValid() const
{
	return (classNumber >= 0 && classNumber <= 16 &&
		variantIndex >= 0 && variantIndex <= 1);
}

bool VerbConjugationClass::IsIrregular() const
{
	return (classNumber == 0);
}

unistr VerbConjugationClass::ToString() const
{
	unistr str = u"class ";
	if (IsIrregular())
		str += u"irreg-";
	else
		str += ConvertFromUTF8(std::to_string(classNumber));
	if (variantIndex == 1)
		str += u"°";
	str += ConvertFromUTF8(EnumToShortString(accentPattern));
	return str;
}

} // namespace ru