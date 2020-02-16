#include "DeclensionTable.h"
#include "RussianApp.h"

DeclensionTable::DeclensionTable()
{
	m_font = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);
	m_gridLayout.SetSpacing(1.0f);
	m_gridLayout.SetMargins(1.0f);
	SetLayout(&m_gridLayout);
	SetBackgroundColor(GUIConfig::color_background_light);
}

void DeclensionTable::Clear()
{
	m_gridLayout.Clear();
}

void DeclensionTable::InitNoun(const ru::NounDeclension& declension)
{
	Clear();

	// Create table header
	for (Plurality plurality : EnumValues<Plurality>())
		SetHeader(EnumToShortString(plurality) + ".", 0, 1 + (uint32) plurality);
	for (Case adjectiveCase : EnumValues<Case>())
		SetHeader(EnumToShortString(adjectiveCase) + ".", 1 + (uint32) adjectiveCase, 0);

	// Fill table body
	for (Plurality plurality : EnumValues<Plurality>())
	{
		for (Case adjectiveCase : EnumValues<Case>())
		{
			SetText(declension.GetDeclension(adjectiveCase, plurality),
				1 + (uint32) adjectiveCase, 1 + (uint32) plurality);
		}
	}
}

void DeclensionTable::InitAdjective(const ru::AdjectiveDeclension& declension)
{
	Clear();

	// Create table header
	for (Gender gender : EnumValues<Gender>())
		SetHeader(EnumToShortString(gender) + ".", 0, 1 + (uint32) gender);
	for (Case nounCase : EnumValues<Case>())
		SetHeader(EnumToShortString(nounCase) + ".", 1 + (uint32) nounCase, 0);
	SetHeader(u"short", 1 + (uint32) Case::k_count, 0);

	// Fill table body
	for (Gender gender : EnumValues<Gender>())
	{
		for (Case nounCase : EnumValues<Case>())
		{
			SetText(declension.GetDeclension(nounCase, gender),
				1 + (uint32) nounCase, 1 + (uint32) gender);
		}
		SetText(declension.GetShortForm(gender),
			1 + (uint32) Case::k_count, 1 + (uint32) gender);
	}
}

void DeclensionTable::InitVerbNonPast(const ru::VerbConjugation& conjugation)
{
	Clear();

	// Create table headers
	SetHeader(u"я", 1, 0);
	SetHeader(u"ты", 2, 0);
	SetHeader(u"он", 3, 0);
	SetHeader(u"мы", 4, 0);
	SetHeader(u"вы", 5, 0);
	SetHeader(u"они", 6, 0);
	if (conjugation.GetAspect() == Aspect::k_perfective)
		SetHeader(u"future", 0, 1);
	else
		SetHeader(u"present", 0, 1);

	// Fill table body
	for (uint32 index = 0; index < 6; index++)
		SetText(conjugation.GetNonPast(index), 1 + index, 1);
}

void DeclensionTable::InitVerbPastAndImperative(const ru::VerbConjugation& conjugation)
{
	Clear();

	// Create table headers
	SetHeader(u"он", 1, 0);
	SetHeader(u"она", 2, 0);
	SetHeader(u"оно", 3, 0);
	SetHeader(u"они", 4, 0);
	SetHeader(u"imp1", 5, 0);
	SetHeader(u"imp2", 6, 0);
	SetHeader(u"past", 0, 1);

	// Fill table body
	SetText(conjugation.GetPast(Gender::k_masculine), 1, 1);
	SetText(conjugation.GetPast(Gender::k_feminine), 2, 1);
	SetText(conjugation.GetPast(Gender::k_neuter), 3, 1);
	SetText(conjugation.GetPast(Gender::k_plural), 4, 1);
	SetText(conjugation.GetImperative(Plurality::k_singular), 5, 1);
	SetText(conjugation.GetImperative(Plurality::k_plural), 6, 1);
}

void DeclensionTable::InitVerbParticiples(const ru::VerbConjugation& conjugation)
{
	Clear();

	// Create table headers
	SetHeader(u"act", 1, 0);
	SetHeader(u"pass", 2, 0);
	SetHeader(u"adv", 3, 0);
	SetHeader(u"present", 0, 1);
	SetHeader(u"past", 0, 2);

	// Fill table body
	SetText(conjugation.GetParticiple(Participle::k_active, Tense::k_present), 1, 1);
	SetText(conjugation.GetParticiple(Participle::k_passive, Tense::k_present), 2, 1);
	SetText(conjugation.GetParticiple(Participle::k_adverbial, Tense::k_present), 3, 1);
	SetText(conjugation.GetParticiple(Participle::k_active, Tense::k_past), 1, 2);
	SetText(conjugation.GetParticiple(Participle::k_passive, Tense::k_past), 2, 2);
	SetText(conjugation.GetParticiple(Participle::k_adverbial, Tense::k_past), 3, 2);
}

void DeclensionTable::SetHeader(const AccentedText& text, uint32 row, uint32 column)
{
	Label* label = new Label(text, m_font);
	label->SetBackgroundColor(GUIConfig::color_background_alternate);
	m_gridLayout.Add(label, row, column);
}

void DeclensionTable::SetText(const AccentedText& text, uint32 row, uint32 column)
{
	Label* label = new Label(text, m_font);
	label->SetBackgroundColor(GUIConfig::color_background);
	m_gridLayout.Add(label, row, column);
}
