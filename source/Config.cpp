#include "Config.h"

CONFIG_MACRO_GUI(GUIConfig, EXPAND_TO_CONFIG_DEFINE)


Color Config::k_colorEditedNew = Color(0, 128, 0);
Color Config::k_colorEditedModified = Color(100, 100, 0);
Color Config::k_colorEditedMatched = Color(20, 20, 100);
Color Config::k_colorEditedInvalid = Color(128, 0, 0);
Color Config::k_colorEditedDuplicate = Color(100, 50, 0);
Map<CardTags, Color> Config::k_mapCardTagColors;
Map<ProficiencyLevel, Color> Config::k_proficiencyLevelColors;
Map<ProficiencyLevel, uint32> Config::k_proficiencyLevelIntervals;
Map<ProficiencyLevel, float> Config::k_proficiencyLevelScoreMultipliers;

void Config::Initialize()
{
	k_mapCardTagColors[CardTags::k_masculine] = Color(255, 0, 0);
	k_mapCardTagColors[CardTags::k_feminine] = Color(255, 128, 200);
	k_mapCardTagColors[CardTags::k_neuter] = Color(128, 128, 128);
	k_mapCardTagColors[CardTags::k_perfective] = Color(128, 0, 128);
	k_mapCardTagColors[CardTags::k_imperfective] = Color(0, 160, 180);
	k_mapCardTagColors[CardTags::k_unidirectional] = Color(0, 180, 0);
	k_mapCardTagColors[CardTags::k_multidirectional] = Color(180, 180, 0);
	k_mapCardTagColors[CardTags::k_nominative] = Color(50, 50, 50);
	k_mapCardTagColors[CardTags::k_accusative] = Color(160, 0, 0);
	k_mapCardTagColors[CardTags::k_dative] = Color(100, 0, 0);
	k_mapCardTagColors[CardTags::k_genitive] = Color(80, 15, 0);
	k_mapCardTagColors[CardTags::k_prepositional] = Color(0, 100, 0);
	k_mapCardTagColors[CardTags::k_instrumental] = Color(100, 100, 0);

	k_proficiencyLevelColors[ProficiencyLevel::k_new] = Color(100, 100, 100);
	k_proficiencyLevelColors[ProficiencyLevel::k_hard] = Color(128, 0, 0);
	k_proficiencyLevelColors[ProficiencyLevel::k_medium] =  Color(128, 64, 0);
	k_proficiencyLevelColors[ProficiencyLevel::k_easy] = Color(128, 128, 0);
	k_proficiencyLevelColors[ProficiencyLevel::k_learned] = Color(0, 128, 0);
}

const Color& Config::GetCardTagColor(CardTags tag)
{
	auto it = k_mapCardTagColors.find(tag);
	if (it != k_mapCardTagColors.end())
		return it->second;
	return Color::BLACK;
}

const Color& Config::GetProficiencyLevelColor(ProficiencyLevel level)
{
	return k_proficiencyLevelColors[level];
}
