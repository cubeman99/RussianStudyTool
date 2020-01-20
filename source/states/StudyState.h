#pragma once

#include "cmgApplicationState.h"
#include "Graphics.h"
#include "Scheduler.h"

class StudyState : public cmg::ApplicationState<AppGraphics&>
{
public:
	using sptr = cmg::shared_ptr<StudySet>;

	StudyState(StudySet::sptr studySet) :
		m_studySet(studySet),
		m_scheduler(studySet)
	{}
	virtual ~StudyState() {}

	virtual void OnBegin() override;
	virtual void OnEnd() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

	void NextCard();

private:
	Card::sptr m_card;
	StudySet::sptr m_studySet;
	Scheduler m_scheduler;
	bool m_isRevealed;
	Language m_shownSide;
};
