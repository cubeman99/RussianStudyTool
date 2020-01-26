#include "Scheduler.h"


Scheduler::Scheduler(StudyDatabase& studyDatabase,
	IStudySet* studySet, const StudyParams& params) :
	m_studyDatabase(studyDatabase),
	m_params(params),
	m_studySet(studySet)
{
	auto cards = m_studySet->GetCards();

	for (uint32 i = 0; i < (uint32) ProficiencyLevel::k_count; i++)
	{
		ProficiencyLevel level = (ProficiencyLevel) i;
		m_proficiencySets[level] = ProficiencySet();
	}
	
	// Create the list of card info
	for (auto it = cards.begin(); it != cards.end(); it++)
	{
		Card::sptr card = *it;
		CardSchedulingInfo info(card);
		m_cardToInfoMap[card] = m_cardList.size();
		m_cardList.push_back(info);
		const auto& studyData = m_studyDatabase.GetCardStudyData(card);
		m_proficiencySets[studyData.GetProficiencyLevel()].cards.insert(card);
	}
	
	m_rep = 1;
}

void Scheduler::MarkCard(Card::sptr card, bool knewIt)
{
	// Remove from its old set
	m_proficiencySets[m_studyDatabase.GetCardStudyData(card)
		.GetProficiencyLevel()].cards.insert(card);

	m_studyDatabase.MarkCard(card, knewIt);

	// Add to its new set
	auto& profSet = m_proficiencySets[
		m_studyDatabase.GetCardStudyData(card).GetProficiencyLevel()];
	profSet.cards.insert(card);

	auto& cardInfo = GetCardSchedulingInfo(card);
	cardInfo.rep = m_rep;
	cardInfo.shownCount++;

	// Check if we have shown this card too many times
	if (m_params.GetMaxRepetitions() > 0 &&
		cardInfo.shownCount >= m_params.GetMaxRepetitions())
		profSet.cards.erase(card);
}

Card::sptr Scheduler::NextCard()
{
	Card::sptr card = GetNextCard();
	m_rep++;
	return card;
}

Card::sptr Scheduler::GetNextCard()
{
	Card::sptr card;

	// Show a new card every new card interval
	if (m_rep % m_params.GetNewCardInterval() == 0)
	{
		card = GetNewCard();
		if (card)
			return card;
	}

	struct AvailableSet
	{
		uint32 rep;
		Array<Card::sptr> cards;
	};

	for (int32 minInterval = m_params.GetMinRepeatInterval();
		minInterval >= 0; minInterval--)
	{
		Array<AvailableSet> availableSets;

		for (auto it : m_proficiencySets)
		{
			if (it.first != ProficiencyLevel::k_new)
			{
				AvailableSet availSet;
				availSet.rep = it.second.rep;

				for (auto card : it.second.cards)
				{
					uint32 r = GetCardSchedulingInfo(card).rep;
					if (r == 0 || (int32) m_rep - (int32) r >= (int32) minInterval)
						availSet.cards.push_back(card);
				}

				if (!availSet.cards.empty())
					availableSets.push_back(availSet);
			}
		}

		if (!availableSets.empty())
		{
			auto& nextSet = ChooseWeightedByAge<AvailableSet>(availableSets);
			card = ChooseWeightedByAge<Card::sptr>(nextSet.cards);
			return card;
		}

		card = GetNewCard();
		if (card)
			return card;
	}

	return card;
}

Card::sptr Scheduler::GetNewCard()
{
	auto& set = m_proficiencySets[ProficiencyLevel::k_new];
	if (set.cards.empty())
		return nullptr;
	return Random::ChooseFromSet(set.cards);
}

Scheduler::CardSchedulingInfo& Scheduler::GetCardSchedulingInfo(Card::sptr card)
{
	return m_cardList[m_cardToInfoMap[card]];
}
