﻿#pragma once

#include "Graphics.h"
#include "states/GUIState.h"
#include "study/Scheduler.h"
#include "study/StudyDatabase.h"
#include "widgets/StudyProficiencyBar.h"
#include "external/wiktionary/Term.h"


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

	// Widgets

	VBoxLayout m_layoutDefinitions;
	Widget m_widgetDeclensionTable;
	GridLayout m_tableDeclension;

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
