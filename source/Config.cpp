#include "Config.h"

CONFIG_MACRO_GUI(GUIConfig, EXPAND_TO_CONFIG_DEFINE)
CONFIG_MACRO_APP(Config, EXPAND_TO_CONFIG_DEFINE)

Map<ProficiencyLevel, uint32> Config::k_proficiencyLevelIntervals;
Map<ProficiencyLevel, float> Config::k_proficiencyLevelScoreMultipliers;

const Color& Config::GetProficiencyLevelColor(ProficiencyLevel level)
{
	if (level == ProficiencyLevel::k_new)
		return Config::color_proficiency_level_0;
	if (level == ProficiencyLevel::k_hard)
		return Config::color_proficiency_level_1;
	if (level == ProficiencyLevel::k_medium)
		return Config::color_proficiency_level_2;
	if (level == ProficiencyLevel::k_easy)
		return Config::color_proficiency_level_3;
	if (level == ProficiencyLevel::k_learned)
		return Config::color_proficiency_level_4;
	return Config::color_proficiency_level_0;
}
