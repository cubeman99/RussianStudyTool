#pragma once

#include "Graphics.h"
#include "states/GUIState.h"
#include "study/Scheduler.h"
#include "study/StudyDatabase.h"
#include "widgets/StudyProficiencyBar.h"
#include "external/wiktionary/Term.h"
#include "widgets/DeclensionTable.h"
#include "widgets/RelatedWordList.h"


class StudyState : public AppWidget
{
public:
	StudyState(IStudySet* studySet, const StudyParams& studyParams = StudyParams());
	virtual ~StudyState() {}

	virtual void OnInitialize() override;
	virtual void OnUninitialize() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

	void MarkCard(bool knewIt);
	void Reveal();
	void NextCard();

	void MarkGoodAndNext();
	void RevealOrMarkBadAndNext();
	void ShowPauseMenu();
	void OpenCardEditView();
	void OpenRelatedCardsView();
	void OpenAddCardToSetView();
	void OpenInWebBrowser();
	void Copy();

private:
	Card::sptr m_card;
	CardStudyData m_cardStudyData;
	wiki::Term::sptr m_term;
	wiki::Word::sptr m_wikiWord;

	StudyParams m_studyParams;
	IStudySet* m_studySet;
	Scheduler* m_scheduler;
	bool m_isRevealed;
	Language m_shownSide;
	Language m_revealedSide;

	// Word widgets
	VBoxLayout m_layoutDefinitions;
	RelatedWordList m_listRelatedCards;

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

	Widget m_titleWidget;
	Label m_labelTitle;
	StudyProficiencyBar m_topProficiencyBar;
	Widget m_proficiencyBarTop;
	Widget m_proficiencyBarBottom;
	Label m_labelCardTextShown;
	Label m_labelCardTextRevealed;
	Label m_labelWordType;
	Label m_labelHistoryScore;
	Label m_labelEncounterTime;
};
