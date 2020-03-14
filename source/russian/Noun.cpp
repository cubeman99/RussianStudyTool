#include "Noun.h"

namespace ru {

static const auto kCaseList = {
	Case::k_nominative,
	Case::k_accusative,
	Case::k_genitive,
	Case::k_dative,
	Case::k_instrumental,
	Case::k_prepositional,
};

NounDeclension::NounDeclension()
{
	for (Case nounCase : kCaseList)
	{
		SetDeclension(nounCase, Plurality::k_singular, "");
		SetDeclension(nounCase, Plurality::k_plural, "");
	}
}

Gender NounDeclension::GetGender() const
{
	return m_gender;
}

Animacy NounDeclension::GetAnimacy() const
{
	return m_animacy;
}

void NounDeclension::GetAllForms(Set<AccentedText>& outForms) const
{
	for (auto it : m_declension)
		outForms.insert(it.second);
}

const AccentedText& NounDeclension::GetDeclension(
	Case nounCase, Plurality plurality) const
{
	return m_declension.at({nounCase, plurality});
}

void NounDeclension::SetGender(Gender gender)
{
	m_gender = gender;
}

void NounDeclension::SetAnimacy(Animacy animacy)
{
	m_animacy = animacy;
}

void NounDeclension::SetDeclension(Case nounCase, Plurality plurality,
	const AccentedText& text)
{
	m_declension[{nounCase, plurality}] = text;
}

void NounDeclension::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	// Gender
	if (m_gender != Gender::k_unknown)
	{
		String genderName = EnumToString(m_gender);
		value.AddMember("gender", rapidjson::Value(
			genderName.c_str(), allocator).Move(), allocator);
	}

	// Animacy
	if (m_animacy != Animacy::k_unknown)
	{
		String animacyName = EnumToString(m_animacy);
		value.AddMember("animacy", rapidjson::Value(
			animacyName.c_str(), allocator).Move(), allocator);
	}

	// Declension
	for (Plurality plurality : {Plurality::k_singular, Plurality::k_plural})
	{
		String pluralityName = EnumToShortString(plurality);
		rapidjson::Value pluralityData(rapidjson::kObjectType);
		for (Case nounCase : kCaseList)
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
	// Gender
	if (data.HasMember("gender"))
		TryStringToEnum<Gender>(data["gender"].GetString(), m_gender);

	// Animacy
	if (data.HasMember("animacy"))
		TryStringToEnum<Animacy>(data["animacy"].GetString(), m_animacy);

	// Declension
	for (Plurality plurality : {Plurality::k_singular, Plurality::k_plural})
	{
		String pluralityName = EnumToShortString(plurality);
		rapidjson::Value& pluralityData = data[pluralityName.c_str()];
		for (Case nounCase : kCaseList)
		{
			String caseName = EnumToShortString(nounCase);
			AccentedText text = pluralityData[caseName.c_str()].GetString();
			SetDeclension(nounCase, plurality, text);
		}
	}
	return CMG_ERROR_SUCCESS;
}

} // namespace ru