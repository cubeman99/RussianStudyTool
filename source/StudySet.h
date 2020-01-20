#pragma once

#include "Card.h"


class StudySet
{
public:
	using sptr = cmg::shared_ptr<StudySet>;

	StudySet() {}
	StudySet(const AccentedText& name) :
		m_name(name)
	{}
	virtual ~StudySet() {}

	const AccentedText& GetName() const { return m_name; }
	const Set<Card::sptr>& GetCards() const { return m_cards; }
	Set<Card::sptr>& GetCards() { return m_cards; }

	void AddCard(Card::sptr card);

private:
	AccentedText m_name;
	Set<Card::sptr> m_cards;
};
