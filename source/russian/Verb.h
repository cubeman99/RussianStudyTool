#pragma once

#include "Russian.h"
#include "core/JsonUtility.h"

namespace ru {

class VerbConjugation
{
public:
	VerbConjugation();

	const AccentedText& GetInfinitive() const;
	Aspect GetAspect() const;
	const AccentedText& GetNonPast(uint32 index) const;
	const AccentedText& GetNonPast(Person person, Plurality plurality) const;
	const AccentedText& GetPast(uint32 index) const;
	const AccentedText& GetPast(Gender gender) const;
	const AccentedText& GetParticiple(Participle participle, Tense tense) const;
	const AccentedText& GetImperative(Plurality plurality) const;

	void SetInfinitive(const AccentedText& text);
	void SetAspect(Aspect aspect);
	void SetNonPast(uint32 index, const AccentedText& text);
	void SetNonPast(Person person, Plurality plurality, const AccentedText& text);
	void SetPast(uint32 index, const AccentedText& text);
	void SetPast(Gender gender, const AccentedText& text);
	void SetParticiple(Participle participle, Tense tense, const AccentedText& text);
	void SetImperative(Plurality plurality, const AccentedText& text);

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	AccentedText m_infinitive;
	Aspect m_aspect;
	Map<std::pair<Person, Plurality>, AccentedText> m_nonPast;
	Map<Gender, AccentedText> m_past;
	Map<Plurality, AccentedText> m_imperative;
	Map<std::pair<Participle, Tense>, AccentedText> m_participles;
};

} // namespace ru