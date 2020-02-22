#pragma once

#include "Types.h"
#include <cmgGraphics/cmg_graphics.h>

class GUIConfig
{
public:
	static Color color_text;
	static Color color_text_cursor;
	static Color color_text_highlighted;
	static Color color_text_box_background;
	static Color color_text_box_background_text;
	static Color color_text_box_selection_background;
	static Color color_background;
	static Color color_background_light;
	static Color color_background_alternate;
	static Color color_background_focused;
	static Color color_background_highlighted;
	static Color color_button_background;
	static Color color_button_background_highlighted;
	static Color color_button_background_pressed;
	static Color color_button_background_focused;
	static Color color_outline;
	static Color color_outline_focused;
};


class Config
{
public:
	static constexpr uint32 k_maxExamplesToDisplay = 7;
	static Color k_colorEditedNew;
	static Color k_colorEditedModified;
	static Color k_colorEditedInvalid;
	static Color k_colorEditedDuplicate;
	static Color k_colorEditedMatched;
	static constexpr uint32 k_maxCardHistorySize = 100;
	static constexpr uint32 k_minRepeatInterval = 4;
	static constexpr uint32 k_newCardInterval = 4;
	static Map<ProficiencyLevel, float> k_proficiencyLevelScoreMultipliers;
	static Map<ProficiencyLevel, uint32> k_proficiencyLevelIntervals;

	static void Initialize();

	static const Color& GetCardTagColor(CardTags tag);
	static const Color& GetProficiencyLevelColor(ProficiencyLevel level);

private:
	static Map<CardTags, Color> k_mapCardTagColors;
	static Map<ProficiencyLevel, Color> k_proficiencyLevelColors;
};
