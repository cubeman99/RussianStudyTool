#pragma once

#include "cards/CardSetPackage.h"
#include "rapidjson/document.h"
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>


class CardDatabase
{
public:
	CardDatabase();
	~CardDatabase();

	CardSetPackage::sptr GetRootPackage() { return m_rootPackage; }

	Card::sptr GetCard(const CardKey& key);
	Card::sptr GetCard(const CardEnKey& enKey);
	Card::sptr GetCard(const CardRuKey& ruKey);
	CardSet::sptr GetCardSet(const CardSetKey& key);
	const Set<CardSet::sptr>& GetCardSetsWithCard(Card::sptr card);

	Error LoadCardData(const Path& path);
	Error SaveCardData(const Path& path);
	Error LoadCardSets(const Path& path);

	static Card::sptr DeserializeCard(rapidjson::Value& data,
		Array<std::pair<Card::sptr, CardKey>>& relatedCardPairs);
	CardSet::sptr DeserializeCardSet(rapidjson::Value& data);
	CardSetPackage::sptr LoadCardSetPackage(const std::filesystem::path& path, const AccentedText& name);
	CardSet::sptr LoadCardSet(const std::filesystem::path& path);

	Error AddCard(Card::sptr card);

private:
	struct CardLoadThreadData
	{
		Array<rapidjson::Value*>* inCardData;
		uint32 inStart;
		uint32 inCount;
		Array<Card::sptr> outCards;
		Array<std::pair<Card::sptr, CardKey>> outRelatedCardPairs;
	};
	static void CardLoadThread(CardDatabase* self, CardLoadThreadData* data);

	Map<CardKey, Card::sptr> m_cards;
	Map<CardEnKey, Card::sptr> m_englishToCards;
	Map<CardRuKey, Card::sptr> m_russianToCards;
	CardSetPackage::sptr m_rootPackage;
	Map<CardSetKey, CardSet::sptr> m_keyToCardSets;

	std::recursive_mutex m_mutexCards;
	std::recursive_mutex m_mutexCardSets;
};
