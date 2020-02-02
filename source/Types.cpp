#include "Types.h"
#include <chrono>

DEFINE_ENUM(ProficiencyLevel, ENUM_MACRO_PROFICIENCY_LEVEL)
DEFINE_ENUM(CardSetType, ENUM_MACRO_CARD_SET_TYPE)
DEFINE_ENUM(CardTags, ENUM_MACRO_CARD_TAG)

AppTimestamp GetAppTimestamp()
{
	AppTimestamp fractionalSecondsSinceEpoch =
		std::chrono::duration_cast<std::chrono::duration<AppTimestamp>>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	return fractionalSecondsSinceEpoch;
}
