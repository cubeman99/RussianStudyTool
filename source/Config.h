﻿#pragma once

#include "Types.h"
#include <cmgGraphics/cmg_graphics.h>

#define CONFIG_MACRO_GUI(_context, _ex) \
	_ex(_context, Color, color_text, Color(200, 200, 200)) \
	_ex(_context, Color, color_text_cursor, Color(200, 200, 200)) \
	_ex(_context, Color, color_text_highlighted, Color(255, 0, 0)) \
	_ex(_context, Color, color_background, Color(25, 25, 25)) \
	_ex(_context, Color, color_background_light, Color(40, 40, 40)) \
	_ex(_context, Color, color_background_alternate, Color(32, 32, 32)) \
	_ex(_context, Color, color_background_focused, Color(30, 30, 40)) \
	_ex(_context, Color, color_background_highlighted, Color(90, 90, 50)) \
	_ex(_context, Color, color_text_box_background, Color(40, 40, 40)) \
	_ex(_context, Color, color_text_box_background_text, Color(128, 128, 128)) \
	_ex(_context, Color, color_text_box_selection_background, Color(46, 67, 89)) \
	_ex(_context, Color, color_button_background, Color(64, 64, 64)) \
	_ex(_context, Color, color_button_background_highlighted, Color(110, 110, 110)) \
	_ex(_context, Color, color_button_background_pressed, Color(110, 110, 110)) \
	_ex(_context, Color, color_button_background_focused, Color(140, 140, 140)) \
	_ex(_context, Color, color_outline, Color(128, 128, 128)) \
	_ex(_context, Color, color_outline_focused, Color(0, 255, 255))

#define EXPAND_TO_CONFIG_DECLARE(_context, _type, _name, _value)  static const _type _name;
#define EXPAND_TO_CONFIG_DEFINE(_context, _type, _name, _value) const _type _context::_name = _value;

class GUIConfig
{
public:
	CONFIG_MACRO_GUI(GUIConfig, EXPAND_TO_CONFIG_DECLARE)
};


class Config
{
public:
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
