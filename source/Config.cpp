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
	case CardTags::k_verb_suffix_ai:   return AccentedText(u"-ай");
	case CardTags::k_verb_suffix_ei:   return AccentedText(u"-ей");
	case CardTags::k_verb_suffix_ova:  return AccentedText(u"-ова");
	case CardTags::k_verb_suffix_nu:   return AccentedText(u"-ну");
	case CardTags::k_verb_suffix_nu2:  return AccentedText(u"-(ну)");
	case CardTags::k_verb_suffix_a1:   return AccentedText(u"-а");
	case CardTags::k_verb_suffix_a2:   return AccentedText(u"-а");
	case CardTags::k_verb_suffix_a3:   return AccentedText(u"-а");
	case CardTags::k_verb_suffix_avai: return AccentedText(u"-авай");
	case CardTags::k_verb_suffix_o:    return AccentedText(u"-о");
	case CardTags::k_verb_suffix_i:    return AccentedText(u"-и");
	case CardTags::k_verb_suffix_e:    return AccentedText(u"-е");
	case CardTags::k_verb_suffix_zha:  return AccentedText(u"-жа");
	case CardTags::k_obstruent_stem:   return AccentedText(u"Obstr");
	case CardTags::k_resonant_stem:    return AccentedText(u"Reson");
	default:
		return EnumToShortString(tag);
	}
}
