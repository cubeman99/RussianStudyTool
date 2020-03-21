#pragma once

#include "Graphics.h"
#include "states/GUIState.h"
#include "study/Scheduler.h"
#include "study/StudyDatabase.h"
#include "widgets/StudyProficiencyBar.h"
#include "external/wiktionary/Term.h"
#include "widgets/DeclensionTable.h"
#include "widgets/RelatedWordList.h"
#include "widgets/StudyHistoryTimeline.h"
#include "widgets/WordDefinitionWidget.h"


class StudyState : public AppWidget
{
public:
	StudyState(IStudySet* studySet, CardSet::sptr cardSet,
		const StudyParams& studyParams = StudyParams());
	virtual ~StudyState() {}

	virtual void OnInitialize() override;
	virtual void OnUninitialize() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

	void MarkCard(bool knewIt);
	void Reveal();
	void NextCard();
	void ReshowCard();
	void ShowCard(Card::sptr card, Language shownSide);

	void MarkGoodAndNext();
	void RevealOrMarkBadAndNext();
	void ShowPauseMenu();
	void OpenCardEditView(Card::sptr card);
	void OpenRelatedCardsView(Card::sptr card);
	void OpenAddCardToSetView(Card::sptr card);
	void OpenCardInWebBrowser();
	void Copy();

	void OnCardDataChanged(Card::sptr card);
	void OnCardAddedOrRemovedFromSet(Card::sptr card, CardSet::sptr cardSet);

private:
	void OpenInWebBrowser(const AccentedText& text);
	bool PopulateTermList(RelatedWordList& termList, const Set<AccentedText>& terms);
	void OnClickWordBox(RelatedWordWidget* widget);
	void OnChooseAddToRelatedCards(Card::sptr card);
	void OnChooseRemoveFromRelatedCards(Card::sptr card);
	void OnChooseAddToCardSet(Card::sptr card);
	void OnChooseRemoveFromCardSet(Card::sptr card);

	Card::sptr m_card;
	CardSet::sptr m_cardSet;
	CardStudyData m_cardStudyData;
	wiki::Term::sptr m_term;
	wiki::Word::sptr m_wikiWord;
	EnumFlags<CardTags> m_cardTags;

	StudyParams m_studyParams;
	IStudySet* m_studySet = nullptr;
	Scheduler* m_scheduler = nullptr;
	bool m_isRevealed = false;
	Language m_shownSide = Language::k_english;
	Language m_revealedSide = Language::k_russian;

	// Word widgets
	WordDefinitionWidget m_wordDefinitionWidget;
	VBoxLayout m_layoutDefinitions;
	RelatedWordList m_listRelatedCards;
	RelatedWordList m_listRelatedTerms;
	RelatedWordList m_listDerivedTerms;
	RelatedWordList m_listSynonyms;
	RelatedWordList m_listAntonyms;

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

	VBoxLayout m_mainLayout;
	AnchorLayout m_anchorLayout;
	Widget m_widgetUnrevealed;
	Widget m_widgetRevealed;
	AnchorLayout m_layoutUnrevealed;
	AnchorLayout m_layoutRevealed;
	HBoxLayout m_titleLayout;
	Widget m_widgetTagsShown;
	Widget m_widgetTagsRevealed;
	HBoxLayout m_layoutTagsShown;
	HBoxLayout m_layoutTagsRevealed;
	VBoxLayout m_layoutCardSets;
	VBoxLayout m_layoutExamples;

	Widget m_titleWidget;
	Label m_labelTitle;
	StudyProficiencyBar m_topProficiencyBar;
	Widget m_proficiencyBarTop;
	Widget m_proficiencyBarBottom;
	Label m_labelCardTextShown;
	Label m_labelCardTextRevealed;
	Label m_labelWordType;
	Label m_labelEncounterTime;
	StudyHistoryWidget m_studyHistoryTimeline;
};
