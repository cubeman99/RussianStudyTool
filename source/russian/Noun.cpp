#include "Noun.h"

namespace ru {

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

} // namespace ru