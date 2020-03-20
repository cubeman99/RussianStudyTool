#pragma once

#include "cards/StudySet.h"
#include "study/StudyDatabase.h"
#include "study/SchedulerBase.h"


class Scheduler : public SchedulerBase<Card::sptr, IStudySet>
{
public:
	Scheduler(StudyDatabase& studyDatabase);
	virtual ~Scheduler() {}

	virtual bool IsItemEncountered(Card::sptr card) override;
	virtual int32 GetRepeatInterval(Card::sptr card) override;

	void Begin(IStudySet* studySet, const StudyParams& params = StudyParams());
	void MarkCard(Card::sptr card, bool knewIt);

private:
	StudyDatabase& m_studyDatabase;
};

