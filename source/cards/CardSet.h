#pragma once

#include "cards/StudySet.h"

class CardSetPackage;

struct CardSetKey
{
public:
	unistr name;

	CardSetKey();
	CardSetKey(const unistr& name);
	bool operator <(const CardSetKey& other) const;
};

// causes ambiguous << error
//std::ostream& operator <<(std::ostream &out, const CardSetKey& key);


class CardSet : public IStudySet
{
public:
	friend class CardDatabase;
	using sptr = cmg::shared_ptr<CardSet>;

	CardSet();
	CardSet(const AccentedText& name);
	virtual ~CardSet();

	const PathU16& GetPath() const;
	const CardSetKey& GetKey() const;
	cmg::shared_ptr<CardSetPackage> GetParent() const;
	CardSetType GetCardSetType() const;
	virtual const AccentedText& GetName() const override;
	virtual const Array<Card::sptr>& GetCards() const override;
	Array<Card::sptr>& GetCards();
	bool HasCard(Card::sptr card) const;

	void SetParent(cmg::shared_ptr<CardSetPackage> parent);
	bool AddCard(Card::sptr card);
	bool InsertCard(uint32 index, Card::sptr card);
	bool RemoveCard(Card::sptr card);

private:
	CardSetKey m_key;
	AccentedText m_name;
	CardSetType m_cardSetType;
	cmg::shared_ptr<CardSetPackage> m_parent;
	Array<Card::sptr> m_cards;
	PathU16 m_path;
};
