#pragma once

#include "cards/CardSetPackage.h"
#include "rapidjson/document.h"
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "gui/Events.h"

class CardDatabase
{
public:
	CardDatabase();
	~CardDatabase();

	CardSetPackage::sptr GetRootPackage() { return m_rootPackage; }

	Card::sptr GetCard(const CardEnKey& enKey);
	Card::sptr GetCard(const CardRuKey& ruKey);
	CardSet::sptr GetCardSet(const CardSetKey& key);
	const Set<CardSet::sptr>& GetCardSetsWithCard(Card::sptr card);
	Map<CardRuKey, Card::sptr>& GetCards() { return m_russianToCards; }

	Error AddCard(Card::sptr card);
	Error ModifyCard(Card::sptr card, const CardData& changes);
	void AddCardToSet(Card::sptr card, CardSet::sptr cardSet);
	void RemoveCardFromSet(Card::sptr card, CardSet::sptr cardSet);
	void LinkRelatedCards(Card::sptr a, Card::sptr b);
	void UnlinkRelatedCards(Card::sptr a, Card::sptr b);
	CardSet::sptr CreateCardSet(CardSetPackage::sptr package,
		const AccentedText& name, const unistr& fileName,
		CardSetType cardSetType);

	Error LoadCardData(const Path& path);
	Error SaveCardData();
	Error SaveCardData(const Path& path);
	Error LoadCardSets(const Path& path);
	Error SaveCardSet(CardSet::sptr cardSet);
	Error SaveChanges();

	// Events
	inline EventSignal<Card::sptr>& CardCreated() { return m_cardCreated; }
	inline EventSignal<Card::sptr>& CardDeleted() { return m_cardDeleted; }
	inline EventSignal<Card::sptr, CardRuKey>& CardKeyChanged() { return m_cardKeyChanged; }
	inline EventSignal<Card::sptr>& CardDataChanged() { return m_cardDataChanged; }
	inline EventSignal<CardSet::sptr>& CardSetCreated() { return m_cardSetCreated; }
	inline EventSignal<CardSet::sptr>& CardSetChanged() { return m_cardSetChanged; }
	inline EventSignal<Card::sptr, CardSet::sptr>& CardAddedToSet() { return m_cardAddedToSet; }
	inline EventSignal<Card::sptr, CardSet::sptr>& CardRemovedFromSet() { return m_cardRemovedFromSet; }

public:
	static void SerializeCardKey(rapidjson::Value& arrayValue, const CardKey& key,
		rapidjson::Document::AllocatorType& allocator);

private:
	static Card::sptr DeserializeCard(rapidjson::Value& data,
		Array<std::pair<Card::sptr, CardRuKey>>& relatedCardPairs);
	static void SerializeCard(rapidjson::Value& outData, Card::sptr card,
		rapidjson::Document::AllocatorType& allocator);
	Error DeserializeCardSet(rapidjson::Value& data, CardSet::sptr& outCardSet);
	CardSetPackage::sptr LoadCardSetPackage(const PathU16& path, const AccentedText& name);
	Error LoadCardSet(const PathU16& path, CardSet::sptr& outCardSet);
	Error SaveCardSet(CardSet::sptr cardSet, const PathU16& path);

	struct CardLoadThreadData
	{
		Array<rapidjson::Value*>* inCardData;
		uint32 inStart;
		uint32 inCount;
		Array<Card::sptr> outCards;
		Array<std::pair<Card::sptr, CardRuKey>> outRelatedCardPairs;
	};
	static void CardLoadThread(CardDatabase* self, CardLoadThreadData* data);
	void MarkCardDirty(Card::sptr card, bool keyChanged);
	void MarkCardSetDirty(CardSet::sptr cardSet);

	CardDatabase(const CardDatabase& copy) = delete;
	CardDatabase& operator=(const CardDatabase& copy) = delete;

	Path m_cardDataPath;
	Map<CardEnKey, Card::sptr> m_englishToCards;
	Map<CardRuKey, Card::sptr> m_russianToCards;
	CardSetPackage::sptr m_rootPackage;
	Map<CardSetKey, CardSet::sptr> m_keyToCardSets;

	EventSignal<Card::sptr> m_cardCreated;
	EventSignal<Card::sptr> m_cardDeleted;
	EventSignal<Card::sptr, CardRuKey> m_cardKeyChanged;
	EventSignal<Card::sptr> m_cardDataChanged;
	EventSignal<CardSet::sptr> m_cardSetCreated;
	EventSignal<CardSet::sptr> m_cardSetChanged;
	EventSignal<Card::sptr, CardSet::sptr> m_cardAddedToSet;
	EventSignal<Card::sptr, CardSet::sptr> m_cardRemovedFromSet;

	std::recursive_mutex m_mutexCards;
	std::recursive_mutex m_mutexCardSets;
	std::mutex m_mutexDirty;
	Set<Card::sptr> m_dirtyCards;
	Set<Card::sptr> m_dirtyKeyChangedCards;
	Set<CardSet::sptr> m_dirtyCardSets;
};
