#include "Types.h"
#include <chrono>

DEFINE_ENUM(CardTags, ENUM_MACRO_CARD_TAG)
DEFINE_ENUM(ProficiencyLevel, ENUM_MACRO_PROFICIENCY_LEVEL)


AppTimestamp GetAppTimestamp()
{
	AppTimestamp fractionalSecondsSinceEpoch =
		std::chrono::duration_cast<std::chrono::duration<AppTimestamp>>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	return fractionalSecondsSinceEpoch;
}
