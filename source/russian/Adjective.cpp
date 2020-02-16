#include "Adjective.h"

namespace ru {

AdjectiveDeclension::AdjectiveDeclension()
{
	for (Gender gender : EnumValues<Gender>())
	{
		for (Case nounCase : EnumValues<Case>())
			SetDeclension(nounCase, gender, AccentedText());
		SetShortForm(gender, AccentedText());
	}
}

const AccentedText& AdjectiveDeclension::GetDeclension(
	Case nounCase, Gender gender) const
{
	return m_declension.at({nounCase, gender});
}

void AdjectiveDeclension::SetDeclension(Case nounCase, Gender gender,
	const AccentedText& text)
{
	m_declension[{nounCase, gender}] = text;
}

const AccentedText& AdjectiveDeclension::GetShortForm(Gender gender) const
{
	return m_shortForms.at(gender);
}

void AdjectiveDeclension::SetShortForm(Gender gender, const AccentedText& text)
{
	m_shortForms[gender] = text;
}

void AdjectiveDeclension::Serialize(rapidjson::Value& value,
	rapidjson::Document::AllocatorType& allocator)
{
	for (Gender gender : EnumValues<Gender>())
	{
		String genderName = EnumToShortString(gender);
		rapidjson::Value genderData(rapidjson::kObjectType);

		for (Case nounCase : EnumValues<Case>())
		{
			String caseName = EnumToShortString(nounCase);
			String text = ConvertToUTF8(GetDeclension(
				nounCase, gender).ToMarkedString());
			genderData.AddMember(
				rapidjson::Value(caseName.c_str(), allocator).Move(),
				rapidjson::Value(text.c_str(), allocator).Move(), allocator);
		}

		String shortText = ConvertToUTF8(
			GetShortForm(gender).ToMarkedString());
		genderData.AddMember("short", rapidjson::Value(
			shortText.c_str(), allocator).Move(), allocator);

		value.AddMember(
			rapidjson::Value(genderName.c_str(), allocator).Move(),
			genderData, allocator);
	}
}

Error AdjectiveDeclension::Deserialize(rapidjson::Value& data)
{
	for (Gender gender : EnumValues<Gender>())
	{
		String genderName = EnumToShortString(gender);
		rapidjson::Value& genderData = data[genderName.c_str()];

		for (Case nounCase : EnumValues<Case>())
		{
			String caseName = EnumToShortString(nounCase);
			AccentedText text = genderData[caseName.c_str()].GetString();
			SetDeclension(nounCase, gender, text);
		}

		AccentedText shortText = genderData["short"].GetString();
		SetShortForm(gender, shortText);
	}
	return CMG_ERROR_SUCCESS;
}

} // namespace ru
