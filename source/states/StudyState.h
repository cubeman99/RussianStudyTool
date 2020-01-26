#pragma once

#include "Graphics.h"
#include "states/GUIState.h"
#include "study/Scheduler.h"
#include "study/StudyDatabase.h"
#include "widgets/StudyProficiencyBar.h"

class StudyState : public GUIState
{
public:
	StudyState(IStudySet* studySet, const StudyParams& studyParams = StudyParams());
	virtual ~StudyState() {}

	virtual void OnBegin() override;
	virtual void OnEnd() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

	void MarkCard(bool knewIt);
	void Reveal();
	void NextCard();
	void ShowPauseMenu();

private:
	Card::sptr m_card;
	CardStudyData m_cardStudyData;
	StudyParams m_studyParams;
	IStudySet* m_studySet;
	Scheduler* m_scheduler;
	bool m_isRevealed;
	Language m_shownSide;
	Language m_revealedSide;

	// Widgets

	VBoxLayout m_mainLayout;
	AnchorLayout m_anchorLayout;
	Widget m_titleWidget;
	HBoxLayout m_titleLayout;
	Label m_labelTitle;
	StudyProficiencyBar m_topProficiencyBar;
	Widget m_rootWidget;
	Widget m_proficiencyBarTop;
	Widget m_proficiencyBarBottom;
	Label m_labelCardTextShown;
	Label m_labelCardTextRevealed;
	Label m_labelWordType;
	Label m_labelHistoryScore;
	Label m_labelEncounterTime;
};
