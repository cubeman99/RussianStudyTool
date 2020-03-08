#pragma once

#include "study/StudySetMetrics.h"
#include "widgets/AppWidget.h"
#include "external/wiktionary/Term.h"


class WordDefinitionWidget : public AppWidget
{
public:
	WordDefinitionWidget();

	virtual void OnInitialize() override;
	void SetWord(wiki::Word::sptr word);

private:
	wiki::Word::sptr m_wikiWord;
	VBoxLayout m_layoutDefinitions;
};
