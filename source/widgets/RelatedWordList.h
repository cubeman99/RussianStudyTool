#pragma once

#include "widgets/AppWidget.h"
#include "external/wiktionary/Term.h"
#include "cards/Card.h"


class RelatedWordWidget : public Label
{
public:
	RelatedWordWidget();

};


class RelatedWordList : public AppWidget
{
public:
	RelatedWordList();

	void Clear();
	void SetLabel(const AccentedText& text);
	void AddWord(const AccentedText& text);

	virtual void OnInitialize() override;

private:
	Font::sptr m_font;
	HBoxLayout m_layoutWords;
	HBoxLayout m_layout;
	Label m_label;
};
