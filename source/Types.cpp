#include "Types.h"
#include <chrono>


DEFINE_ENUM(WordType, ENUM_MACRO_WORD_TYPE)
DEFINE_ENUM(Gender, ENUM_MACRO_GENDER)
DEFINE_ENUM(Plurality, ENUM_MACRO_PLURALITY)
DEFINE_ENUM(Participle, ENUM_MACRO_PARTICIPLE)
DEFINE_ENUM(Person, ENUM_MACRO_PERSON)
DEFINE_ENUM(Case, ENUM_MACRO_CASE)
DEFINE_ENUM(Aspect, ENUM_MACRO_ASPECT)
DEFINE_ENUM(Tense, ENUM_MACRO_TENSE)
DEFINE_ENUM(Animacy, ENUM_MACRO_ANIMACY)

DEFINE_ENUM(CardSetType, ENUM_MACRO_CARD_SET_TYPE)
DEFINE_ENUM(CardTags, ENUM_MACRO_CARD_TAG)

AppTimestamp GetAppTimestamp()
{
	AppTimestamp fractionalSecondsSinceEpoch =
		std::chrono::duration_cast<std::chrono::duration<AppTimestamp>>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	return fractionalSecondsSinceEpoch;
}

bool IsKeyCardTag(CardTags tag)
{
	return (uint32) tag < 32;
}

