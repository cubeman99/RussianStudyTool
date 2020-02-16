#pragma once

#include "russian/Adjective.h"
#include "russian/Noun.h"
#include "russian/Verb.h"
#include "widgets/AppWidget.h"


class DeclensionTable : public AppWidget
{
public:
	DeclensionTable();

	void Clear();
	void InitNoun(const ru::NounDeclension& declension);
	void InitAdjective(const ru::AdjectiveDeclension& declension);
	void InitVerbNonPast(const ru::VerbConjugation& conjugation);
	void InitVerbPastAndImperative(const ru::VerbConjugation& conjugation);
	void InitVerbParticiples(const ru::VerbConjugation& conjugation);

	void SetHeader(const AccentedText& text, uint32 row, uint32 column);
	void SetText(const AccentedText& text, uint32 row, uint32 column);

private:
	Font::sptr m_font;
	GridLayout m_gridLayout;
};
