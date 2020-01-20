#pragma once

#include "CardSet.h"


class CardSetPackage : public StudySet
{
public:
	friend class CardDatabase;
	using sptr = cmg::shared_ptr<CardSetPackage>;

	CardSetPackage() {}
	CardSetPackage(const AccentedText& name) :
		StudySet(name)
	{}
	virtual ~CardSetPackage() {}

	const unistr& GetKey() const { return m_key; }
	const Path& GetPath() const { return m_path; }
	CardSetPackage::sptr GetParent() { return m_parent; }
	Array<CardSetPackage::sptr>& GetPackages() { return m_packages; }
	Array<CardSet::sptr>& GetCardSets() { return m_cardSets; }

	void AddCardSet(CardSet::sptr cardSet);
	void AddPackage(CardSetPackage::sptr package);
	void SetParent(CardSetPackage::sptr parent);

private:
	unistr m_key;
	Path m_path;
	CardSetPackage::sptr m_parent;
	Array<CardSetPackage::sptr> m_packages;
	Array<CardSet::sptr> m_cardSets;
};
