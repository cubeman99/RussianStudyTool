﻿#pragma once

#include "Types.h"
#include "AccentedText.h"
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
	static Color k_colorGray;
	static Color k_colorGreen;
	static Color k_colorRed;
	static Color k_colorYellow;
	static constexpr uint32 k_minRepeatInterval = 8;
	static constexpr uint32 k_newCardInterval = 4;
	static constexpr uint32 k_studyScoreHistogramBinCount = 10;
	static constexpr const char* k_keyShortcutEditCard = "e";
	static constexpr const char* k_keyShortcutEditRelatedCards = "r";
	static constexpr const char* k_keyShortcutAddToCardSets = "s";
	static constexpr const char* k_keyShortcutShowWikiTerm = "w";
	static constexpr const char* k_keyShortcutOpenInWebBrowser = "i";
	static constexpr const char* k_keyShortcutReshowCard = "o";
	static constexpr const char* k_keyShortcutSwapLanguage = "Ctrl+q";
	static constexpr const char* k_keyShortcutMarkGood = "enter";
	static constexpr const char* k_keyShortcutMarkBad = "backspace";
	static constexpr const char* k_keyShortcutOpenMenu = "end";

	static void Initialize();

	static const Color& GetCardTagColor(CardTags tag);
	static Color GetHistoryScoreColor(float score);
	static AccentedText GetCardTagShortDisplayName(CardTags tag);
	static AccentedText GetCardTagLongDisplayName(CardTags tag);

private:
	static Map<CardTags, Color> k_mapCardTagColors;
};
