#pragma once

#include "study/StudySetMetrics.h"
#include "widgets/AppWidget.h"
#include "widgets/WordDefinitionWidget.h"
#include "widgets/RelatedWordList.h"
#include "widgets/DeclensionTable.h"
#include "external/wiktionary/Term.h"


class WikiWordWidget : public AppWidget
{
public:
	friend class WikiTermView;

	WikiWordWidget(wiki::Word::sptr word);

	wiki::Word::sptr GetWord();

	virtual void OnInitialize() override;

	EventSignal<>& Clicked() { return m_clicked; }
	virtual void OnPress() override { m_clicked.Emit(); }

private:
	bool PopulateTermList(RelatedWordList& termList,
		const Set<AccentedText>& terms);
	void OnClickWordBox(RelatedWordWidget* widget);

	wiki::Word::sptr m_word;

	// Noun widgets
	Widget m_widgetNounInfo;
	HBoxLayout m_layoutNounInfo;
	DeclensionTable m_tableNounDeclension;

	// Ajdective widgets
	Widget m_widgetAdjectiveInfo;
	HBoxLayout m_layoutAdjectiveInfo;
	DeclensionTable m_tableAdjectiveDeclension;

	// Verb widgets
	Widget m_widgetVerbInfo;
	HBoxLayout m_layoutVerbInfo;
	DeclensionTable m_tableVerbNonPast;
	DeclensionTable m_tableVerbPast;
	DeclensionTable m_tableVerbParticiples;
	VBoxLayout m_layoutVerbParticiples;
	Label m_labelInfinitive;
	RelatedWordList m_listCounterparts;

	// Word widgets
	WordDefinitionWidget m_wordDefinitionWidget;
	RelatedWordList m_listRelatedCards;
	RelatedWordList m_listRelatedTerms;
	RelatedWordList m_listDerivedTerms;
	RelatedWordList m_listSynonyms;
	RelatedWordList m_listAntonyms;

	HBoxLayout m_layoutDefinition;
	VBoxLayout m_mainLayout;
	EventSignal<> m_clicked;
};


class WikiTermView : public AppWidget
{
public:
	WikiTermView(wiki::Term::sptr term = nullptr);

	virtual void OnInitialize() override;
	void SetTerm(wiki::Term::sptr term);

private:
	void OpenOptionsMenu(WikiWordWidget* widget);
	void OnClickCard(Card::sptr card);

	wiki::Term::sptr m_term;

	// Title
	HBoxLayout m_layoutTitle;
	Label m_labelTitle;
	Widget m_titleWidget;

	RelatedWordList m_listCards;
	Widget m_widgetWordList;
	VBoxLayout m_layoutWordList;
	AbstractScrollArea m_scrollArea;
	VBoxLayout m_mainLayout;
};
