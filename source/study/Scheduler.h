#pragma once

#include "cards/StudySet.h"
#include "study/StudyDatabase.h"
#include "Config.h"


class StudyParams
{
public:
	StudyParams()
	{
	}

	uint32 GetMaxRepetitions() const { return m_maxRepetitions; }
	uint32 GetNewCardInterval() const { return m_newCardInterval; }
	uint32 GetMinRepeatInterval() const { return m_minRepeatInterval; }

private:
	Map<ProficiencyLevel, uint32> proficiencyLevelIntervals = Config::k_proficiencyLevelIntervals;
	uint32 m_newCardInterval = Config::k_newCardInterval;
	uint32 m_minRepeatInterval = Config::k_minRepeatInterval;
	uint32 m_maxRepetitions = 0;
};



class Scheduler
{
public:
	Scheduler(StudyDatabase& studyDatabase, IStudySet* studySet,
		const StudyParams& params = StudyParams());
	virtual ~Scheduler() {}

	const StudyParams& GetStudyParams() const { return m_params; }
	IStudySet* GetStudySet() const { return m_studySet; }

	void MarkCard(Card::sptr card, bool knewIt);
	Card::sptr NextCard();

private:
	struct CardSchedulingInfo
	{
		CardSchedulingInfo(Card::sptr card) :
			card(card)
		{}

		Card::sptr card;
		uint32 rep = 0;
		uint32 shownCount = 0;
	};

	struct ProficiencySet
	{
		Set<Card::sptr> cards;
		uint32 rep = 0;
	};

	Card::sptr GetNextCard();
	Card::sptr GetNewCard();
	CardSchedulingInfo& GetCardSchedulingInfo(Card::sptr card);

	template<typename T_Item, typename T_Weight>
	static inline T_Item& ChooseWeighted(Array<T_Item>& items, const Array<T_Weight>& weights);

private:
	StudyDatabase& m_studyDatabase;
	IStudySet* m_studySet;
	StudyParams m_params;

	uint32 m_rep = 1;
	Array<CardSchedulingInfo> m_cardList;
	Map<Card::sptr, uint32> m_cardToInfoMap;
	Map<ProficiencyLevel, ProficiencySet> m_proficiencySets;
};

template<typename T_Item, typename T_Weight>
inline T_Item& Scheduler::ChooseWeighted(Array<T_Item>& items, const Array<T_Weight>& weights)
{
	T_Weight weightSum = T_Weight(0);
	for (const T_Weight& weight : weights)
		weightSum += weight;
	if (weightSum == T_Weight(0))
		return Random::Choose(items);
	uint32 choice = Random::NextInt(weightSum);
	weightSum = 0;
	for (uint32 i = 0; i < weights.size(); i++)
	{
		weightSum += weights[i];
		if (choice < weightSum)
			return items[i];
	}
	return items.back();
}
