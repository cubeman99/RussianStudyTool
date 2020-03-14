#pragma once

#include "russian/Russian.h"
#include "core/JsonUtility.h"

namespace ru {

class NounDeclension
{
public:
	NounDeclension();

	Gender GetGender() const;
	Animacy GetAnimacy() const;
	const AccentedText& GetDeclension(Case nounCase, Plurality plurality) const;

	void SetGender(Gender gender);
	void SetAnimacy(Animacy animacy);
	void SetDeclension(Case nounCase, Plurality plurality, const AccentedText& text);

	virtual void GetAllForms(Set<AccentedText>& outForms) const;
	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	Gender m_gender = Gender::k_unknown;
	Animacy m_animacy = Animacy::k_unknown;
	Map<std::pair<Case, Plurality>, AccentedText> m_declension;
};

} // namespace ru