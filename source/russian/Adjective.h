#pragma once

#include "Russian.h"
#include "core/JsonUtility.h"

namespace ru {

class AdjectiveDeclension
{
public:
	AdjectiveDeclension();

	const AccentedText& GetDeclension(Case adjectiveCase, Gender gender) const;
	const AccentedText& GetShortForm(Gender gender) const;

	void SetDeclension(Case adjectiveCase, Gender gender, const AccentedText& text);
	void SetShortForm(Gender gender, const AccentedText& text);

	virtual void GetAllForms(Set<AccentedText>& outForms) const;
	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	Map<std::pair<Case, Gender>, AccentedText> m_declension;
	Map<Gender, AccentedText> m_shortForms;
};

} // namespace ru