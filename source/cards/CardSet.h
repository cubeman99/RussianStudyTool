#pragma once

#include "cards/StudySet.h"

class CardSetPackage;

struct CardSetKey
{
	unistr name;

	CardSetKey()
	{
	}
	CardSetKey(const unistr& name) :
		name(name)
	{
	}

	bool operator <(const CardSetKey& other) const
	{
		return name < other.name;
	}
};

//std::ostream& operator <<(std::ostream &out, const CardSetKey& key);


class CardSet : public StudySet
{
public:
	friend class CardDatabase;
	using sptr = cmg::shared_ptr<CardSet>;

	CardSet()
	{}
	CardSet(const AccentedText& name) :
		StudySet(name)
	{
		m_key.name = ru::ToLower(name.GetString());
	}
	virtual ~CardSet() {}

	void SetParent(cmg::shared_ptr<CardSetPackage> parent) { m_parent = parent; }

	const CardSetKey& GetKey() const { return m_key; }
	cmg::shared_ptr<CardSetPackage> GetParent() const { return m_parent; }
	CardSetType GetCardSetType() const { return m_cardSetType; }

private:
	CardSetKey m_key;
	CardSetType m_cardSetType;
	cmg::shared_ptr<CardSetPackage> m_parent;
};
