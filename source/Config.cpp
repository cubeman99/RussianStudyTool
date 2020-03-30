#include "Config.h"

// GUI Config
Color GUIConfig::color_text = Color(230, 230, 230);
Color GUIConfig::color_text_cursor = GUIConfig::color_text;
Color GUIConfig::color_text_highlighted = Color(255, 0, 0);
Color GUIConfig::color_text_box_background = Color(25, 25, 25);
Color GUIConfig::color_text_box_background_text = Color(128, 128, 128);
Color GUIConfig::color_text_box_selection_background = Color(46, 67, 89);
Color GUIConfig::color_background = Color(10, 10, 10);
Color GUIConfig::color_background_light = Color(25, 25, 25);
Color GUIConfig::color_background_alternate = Color(20, 20, 20);
Color GUIConfig::color_background_focused = Color(25, 25, 30);
Color GUIConfig::color_background_highlighted = Color(50, 50, 50);
Color GUIConfig::color_button_background = Color(40, 40, 40);
Color GUIConfig::color_button_background_highlighted = Color(60, 60, 60);
Color GUIConfig::color_button_background_pressed = Color(80, 80, 80);
Color GUIConfig::color_button_background_focused = Color(140, 140, 140);
Color GUIConfig::color_outline = Color(128, 128, 128);
Color GUIConfig::color_outline_focused = Color(0, 255, 255);

// App Config
Color Config::k_colorGray = Color(100, 100, 100);
Color Config::k_colorGreen = Color(0, 128, 0);
Color Config::k_colorYellow = Color(100, 100, 0);
Color Config::k_colorRed = Color(128, 0, 0);
Color Config::k_colorEditedNew = Color(0, 128, 0);
Color Config::k_colorEditedModified = Color(100, 100, 0);
Color Config::k_colorEditedMatched = Color(20, 20, 100);
Color Config::k_colorEditedInvalid = Color(128, 0, 0);
Color Config::k_colorEditedDuplicate = Color(100, 50, 0);
Map<CardTags, Color> Config::k_mapCardTagColors;

void Config::Initialize()
{
	k_mapCardTagColors[CardTags::k_masculine] = Color(255, 0, 0);
	k_mapCardTagColors[CardTags::k_feminine] = Color(255, 128, 200);
	k_mapCardTagColors[CardTags::k_neuter] = Color(128, 128, 128);
	k_mapCardTagColors[CardTags::k_animate] = Color(170, 40, 140);

	k_mapCardTagColors[CardTags::k_nominative] = Color(50, 50, 50);
	k_mapCardTagColors[CardTags::k_accusative] = Color(160, 0, 0);
	k_mapCardTagColors[CardTags::k_dative] = Color(100, 0, 0);
	k_mapCardTagColors[CardTags::k_genitive] = Color(80, 15, 0);
	k_mapCardTagColors[CardTags::k_prepositional] = Color(0, 100, 0);
	k_mapCardTagColors[CardTags::k_instrumental] = Color(100, 100, 0);

	k_mapCardTagColors[CardTags::k_imperfective] = Color(0, 160, 180);
	k_mapCardTagColors[CardTags::k_perfective] = Color(128, 0, 128);
	k_mapCardTagColors[CardTags::k_unidirectional] = Color(0, 180, 0);
	k_mapCardTagColors[CardTags::k_multidirectional] = Color(180, 180, 0);
	k_mapCardTagColors[CardTags::k_reflexive] = Color(0, 0, 100);
	k_mapCardTagColors[CardTags::k_transitive] = Color(0, 100, 0);
	k_mapCardTagColors[CardTags::k_intransitive] = Color(80, 15, 0);
	k_mapCardTagColors[CardTags::k_stem_stressed] = Color(200, 0, 0);
	k_mapCardTagColors[CardTags::k_ending_stressed] = Color(100, 100, 0);
	k_mapCardTagColors[CardTags::k_shifting_stress] = Color(120, 60, 0);

}

const Color& Config::GetCardTagColor(CardTags tag)
{
	auto it = k_mapCardTagColors.find(tag);
	if (it != k_mapCardTagColors.end())
		return it->second;
	return Color::BLACK;
}

Color Config::GetHistoryScoreColor(float score)
{
	return Color(Math::PolyLerp<float, Vector3f>(score, {
		{0.0f, Color(170, 0, 0).ToVector3f()},
		{0.5f, Color(160, 150, 0).ToVector3f()},
		{1.0f, Color(20, 150, 0).ToVector3f()},
	}));
}

AccentedText Config::GetCardTagShortDisplayName(CardTags tag)
{
	switch (tag)
	{
	case CardTags::k_verb_class_1:     return AccentedText(u"class 1");
	case CardTags::k_verb_class_2:     return AccentedText(u"class 2");
	case CardTags::k_verb_class_3:     return AccentedText(u"class 3");
	case CardTags::k_verb_class_3o:    return AccentedText(u"class 3°");
	case CardTags::k_verb_class_4:     return AccentedText(u"class 4");
	case CardTags::k_verb_class_5:     return AccentedText(u"class 5");
	case CardTags::k_verb_class_5o:    return AccentedText(u"class 5°");
	case CardTags::k_verb_class_6:     return AccentedText(u"class 6");
	case CardTags::k_verb_class_6o:    return AccentedText(u"class 6°");
	case CardTags::k_verb_class_7:     return AccentedText(u"class 7");
	case CardTags::k_verb_class_8:     return AccentedText(u"class 8");
	case CardTags::k_verb_class_9:     return AccentedText(u"class 9");
	case CardTags::k_verb_class_10:    return AccentedText(u"class 10");
	case CardTags::k_verb_class_11:    return AccentedText(u"class 11");
	case CardTags::k_verb_class_12:    return AccentedText(u"class 12");
	case CardTags::k_verb_class_13:    return AccentedText(u"class 13");
	case CardTags::k_verb_class_14:    return AccentedText(u"class 14");
	case CardTags::k_verb_class_15:    return AccentedText(u"class 15");
	case CardTags::k_verb_class_16:    return AccentedText(u"class 16");

	case CardTags::k_stem_stressed:    return AccentedText(u"a");
	case CardTags::k_ending_stressed:  return AccentedText(u"b");
	case CardTags::k_shifting_stress:  return AccentedText(u"c");

	default:
		return EnumToShortString(tag);
	}
}


AccentedText Config::GetCardTagLongDisplayName(CardTags tag)
{
	switch (tag)
	{
	case CardTags::k_verb_class_1:     return AccentedText(u"-ай/-ей/-яй");
	case CardTags::k_verb_class_2:     return AccentedText(u"-ова(уй)");
	case CardTags::k_verb_class_3:     return AccentedText(u"-ну");
	case CardTags::k_verb_class_3o:    return AccentedText(u"-(ну)");
	case CardTags::k_verb_class_4:     return AccentedText(u"-и");
	case CardTags::k_verb_class_5:     return AccentedText(u"-е/-жа/-я");
	case CardTags::k_verb_class_5o:    return AccentedText(u"-е/-жа/-я");
	case CardTags::k_verb_class_6:     return AccentedText(u"-а(я)");
	case CardTags::k_verb_class_6o:    return AccentedText(u"-а(я)");
	case CardTags::k_verb_class_7:     return AccentedText(u"-з/-с/-д/-т/-б");
	case CardTags::k_verb_class_8:     return AccentedText(u"-г/-к");
	case CardTags::k_verb_class_9:     return AccentedText(u"-р");
	case CardTags::k_verb_class_10:    return AccentedText(u"-о");
	case CardTags::k_verb_class_11:    return AccentedText(u"-ий");
	case CardTags::k_verb_class_12:    return AccentedText(u"-ой/-й");
	case CardTags::k_verb_class_13:    return AccentedText(u"-авай");
	case CardTags::k_verb_class_14:    return AccentedText(u"-м/-н");
	case CardTags::k_verb_class_15:    return AccentedText(u"-н");
	case CardTags::k_verb_class_16:    return AccentedText(u"-в");

	case CardTags::k_stem_stressed:    return AccentedText(u"Stem Stress");
	case CardTags::k_ending_stressed:  return AccentedText(u"End Stress");
	case CardTags::k_shifting_stress:  return AccentedText(u"Shifting Stress");

	default:
		return EnumToString(tag);
	}
}

