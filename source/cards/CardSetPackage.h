#pragma once

#include "cards/CardSet.h"


class CardSetPackage : public IStudySet
{
public:
	friend class CardDatabase;
	using sptr = cmg::shared_ptr<CardSetPackage>;

	CardSetPackage();
	CardSetPackage(const AccentedText& name);
	virtual ~CardSetPackage();

	virtual const AccentedText& GetName() const override { return m_name; }
	virtual const Array<Card::sptr>& GetCards() const override;

	const unistr& GetKey() const { return m_key; }
	const PathU16& GetPath() const { return m_path; }
	CardSetPackage::sptr GetParent() { return m_parent; }
	Array<CardSetPackage::sptr>& GetPackages() { return m_packages; }
	Array<CardSet::sptr>& GetCardSets() { return m_cardSets; }

	void AddCardSet(CardSet::sptr cardSet);
	void AddPackage(CardSetPackage::sptr package);
	void SetParent(CardSetPackage::sptr parent);

	template <class T_Func>
	void IterAllCardSets(T_Func callback)
	{
		for (CardSet::sptr cardSet : m_cardSets)
			callback(cardSet);
		for (CardSetPackage::sptr package : m_packages)
			package->IterAllCardSets(callback);
	}

private:
	AccentedText m_name;
	unistr m_key;
	PathU16 m_path;
	CardSetPackage::sptr m_parent;
	Array<CardSetPackage::sptr> m_packages;
	Array<CardSet::sptr> m_cardSets;

	// Cached
	mutable Array<Card::sptr> m_cards;
};
