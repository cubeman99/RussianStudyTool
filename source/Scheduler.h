#pragma once

#include "StudySet.h"


class StudyParams
{
public:
	StudyParams()
	{
	}

	uint32 maxRepetitions = 0;
};


class Scheduler
{
public:
	Scheduler(StudySet::sptr studySet, const StudyParams& params = StudyParams());
	virtual ~Scheduler() {}

	const StudyParams& GetStudyParams() const { return m_params; }
	StudySet::sptr GetStudySet() const { return m_studySet; }

	Card::sptr NextCard();

private:
	StudySet::sptr m_studySet;
	StudyParams m_params;
	Array<Card::sptr> m_cardList;
	uint32 m_cardIndex = 0;
};
