#pragma once

#include "widgets/AppWidget.h"
#include "external/wiktionary/Term.h"
#include "cards/Card.h"


class RelatedWordWidget : public AppWidget
{
public:
	friend class RelatedWordList;

	RelatedWordWidget(const AccentedText& text);

	virtual void OnInitialize() override;

	Card::sptr GetCard() const { return m_card; }
	wiki::Term::sptr GetWiktionaryTerm() const { return m_term; }
	const AccentedText& GetText() const { return m_label.GetText(); }
	EventSignal<>& Clicked() { return m_label.Clicked(); }

private:
	Button m_label;
	HBoxLayout m_mainLayout;
	Card::sptr m_card;
	wiki::Term::sptr m_term;
};


class RelatedWordList : public AppWidget
{
public:
	RelatedWordList();

	void Clear();
	void SetLabel(const AccentedText& text);
	RelatedWordWidget* AddWord(const AccentedText& text);

	virtual void OnInitialize() override;

private:
	Font::sptr m_font;
	HBoxLayout m_layoutWords;
	HBoxLayout m_layout;
	Label m_label;
};
