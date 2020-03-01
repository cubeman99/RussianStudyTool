#pragma once

#include "cards/Card.h"


class IStudySet
{
public:
	virtual ~IStudySet() {}

	virtual const AccentedText& GetName() const = 0;
	virtual const Array<Card::sptr>& GetCards() const = 0;
};


class StudySet : public IStudySet
{
public:
	using sptr = cmg::shared_ptr<StudySet>;

	StudySet() {}
	StudySet(const AccentedText& name) :
		m_name(name)
	{}
	virtual ~StudySet() {}

	virtual const AccentedText& GetName() const override { return m_name; }
	virtual const Array<Card::sptr>& GetCards() const override { return m_cards; }
	Array<Card::sptr>& GetCards() { return m_cards; }

	void ClearCards();
	void AddCard(Card::sptr card);
	void RemoveCard(Card::sptr card);

private:
	AccentedText m_name;
	Array<Card::sptr> m_cards;
};
