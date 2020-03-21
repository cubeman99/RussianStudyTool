#pragma once

#include "study/StudySetMetrics.h"
#include "widgets/AppWidget.h"
#include "external/wiktionary/Term.h"


class WordDefinitionWidget : public AppWidget
{
public:
	WordDefinitionWidget();

	const AccentedText& GetWordText() const;

	void SetWord(wiki::Word::sptr word);

	virtual void OnInitialize() override;

private:
	AccentedText m_wordText;
	wiki::Word::sptr m_wikiWord;
	VBoxLayout m_layoutDefinitions;
};
