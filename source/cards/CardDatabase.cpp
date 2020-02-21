#include "CardDatabase.h"
#include "core/JsonUtility.h"

CardDatabase::CardDatabase()
{
}

CardDatabase::~CardDatabase()
{
}

Card::sptr CardDatabase::GetCard(const CardEnKey& enKey)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	auto it = m_englishToCards.find(enKey);
	if (it != m_englishToCards.end())
		return it->second;
	return nullptr;
}

Card::sptr CardDatabase::GetCard(const CardRuKey& ruKey)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	auto it = m_russianToCards.find(ruKey);
	if (it != m_russianToCards.end())
		return it->second;
	return nullptr;
}

CardSet::sptr CardDatabase::GetCardSet(const CardSetKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCardSets);
	auto it = m_keyToCardSets.find(key);
	if (it != m_keyToCardSets.end())
		return it->second;
	return nullptr;
}

const Set<CardSet::sptr>& CardDatabase::GetCardSetsWithCard(Card::sptr card)
{
	return card->m_cardSets;
}

void CardDatabase::CardLoadThread(CardDatabase* self, CardLoadThreadData* data)
{
	// Deserialize all card data
	Array<std::pair<Card::sptr, CardKey>> relatedCardPairs;
	for (uint32 i = data->inStart; i < data->inStart + data->inCount; i++)
	{
		auto cardDataPtr = (*data->inCardData)[i];
		Array<CardKey> relatedCardKeys;
		Card::sptr card = DeserializeCard(*cardDataPtr, data->outRelatedCardPairs);
		if (card)
		{
			data->outCards.push_back(card);
		}
	}
}

void CardDatabase::MarkCardDirty(Card::sptr card, bool keyChanged)
{
	std::lock_guard<std::mutex> guard(m_mutexDirty);
	m_dirtyCards.insert(card);
	if (keyChanged)
		m_dirtyKeyChangedCards.insert(card);
}

void CardDatabase::MarkCardSetDirty(CardSet::sptr cardSet)
{
	std::lock_guard<std::mutex> guard(m_mutexDirty);
	m_dirtyCardSets.insert(cardSet);
}

Error CardDatabase::LoadCardData(const Path& path)
{
	CMG_LOG_INFO() << "Loading cards";
	m_cardDataPath = path;

	// Open the json file
	String json;
	rapidjson::Document document;
	File::OpenAndGetContents(path, json);
	document.Parse(json.c_str());
	if (document.HasParseError())
		return CMG_ERROR(CommonErrorTypes::k_file_corrupt);

	// Gather all card data
	Array<rapidjson::Value*> cardData;
	rapidjson::Value& cardDataList = document["cards"];
	for (auto it = cardDataList.Begin(); it != cardDataList.End(); it++)
	{
		cardData.push_back(it);
	}

	// Start threads to load cards
	uint32 batchSize = 800;
	Array<std::thread*> threads;
	Array<CardLoadThreadData*> threadData;
	for (uint32 start = 0; start < cardData.size(); start += batchSize)
	{
		uint32 count = Math::Min(batchSize, cardData.size() - start);
		CardLoadThreadData* data = new CardLoadThreadData();
		data->inCardData = &cardData;
		data->inStart = start;
		data->inCount = count;
		threadData.push_back(data);
		std::thread* loadThread = new std::thread(
			&CardDatabase::CardLoadThread, this, data);
		threads.push_back(loadThread);
	}
	CMG_LOG_INFO() << "Loading " << cardData.size() << " cards in " <<
		threads.size() << " threads...";

	// Wait for the threads to finish then add their cards
	uint32 relatedCardsCount = 0;
	for (uint32 i = 0; i < threads.size(); i++)
	{
		auto loadThread = threads[i];
		auto data = threadData[i];
		loadThread->join();
		relatedCardsCount += data->outRelatedCardPairs.size();
		for (auto card : data->outCards)
			AddCard(card);
		delete loadThread;
	}
	threads.clear();

	// Link together all related cards
	CMG_LOG_INFO() << "Linking " << relatedCardsCount << " related cards...";
	for (auto data : threadData)
	{
		for (auto relatedCardPair : data->outRelatedCardPairs)
		{
			Card::sptr card1 = relatedCardPair.first;
			Card::sptr card2 = GetCard(relatedCardPair.second);
			if (card2)
			{
				card1->m_relatedCards.insert(card2);
				card2->m_relatedCards.insert(card1);
			}
		}

		delete data;
	}
	threadData.clear();

	// Clear the dirty state
	{
		std::lock_guard<std::mutex> guard(m_mutexDirty);
		m_dirtyCards.clear();
		m_dirtyKeyChangedCards.clear();
	}

	CMG_LOG_INFO() << "Loaded " << m_russianToCards.size() << " cards!";
	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::SaveCardData()
{
	return SaveCardData(m_cardDataPath);
}

Error CardDatabase::SaveCardData(const Path& path)
{
	CMG_LOG_INFO() << "Saving card data to: " << path;
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);

	rapidjson::Document document(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// Save the list of card data
	rapidjson::Value cardDataList(rapidjson::kArrayType);
	document.AddMember("save_time", GetAppTimestamp(), allocator);
	Array<char> historyString;
	for (auto it : m_russianToCards)
	{
		Card::sptr card = it.second;
		rapidjson::Value cardData(rapidjson::kObjectType);
		SerializeCard(cardData, card, allocator);
		cardDataList.PushBack(cardData, allocator);
	}
	document.AddMember("cards", cardDataList, allocator);

	// Save to the file
	Error error = json::SaveDocumentToFile(path, document);
	if (error.Failed())
		return error.Uncheck();

	// Clear the dirty state
	{
		std::lock_guard<std::mutex> guard(m_mutexDirty);
		m_dirtyCards.clear();
		m_dirtyKeyChangedCards.clear();
	}

	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::SaveChanges()
{
	Error error;
	bool saveCards = false;
	Set<Card::sptr> cardsToSave;
	Set<CardSet::sptr> cardSetsToSave;

	// Retreive dirty state
	{
		std::lock_guard<std::recursive_mutex> guardCardSets(m_mutexCardSets);
		std::lock_guard<std::recursive_mutex> guardCards(m_mutexCards);
		std::lock_guard<std::mutex> guardDirty(m_mutexDirty);
		for (Card::sptr card : m_dirtyCards)
			cardsToSave.insert(card);
		for (Card::sptr card : m_dirtyKeyChangedCards)
		{
			cardsToSave.insert(card);
			for (CardSet::sptr cardSet : GetCardSetsWithCard(card))
				cardSetsToSave.insert(cardSet);
		}
		for (CardSet::sptr cardSet : m_dirtyCardSets)
			cardSetsToSave.insert(cardSet);
	}

	// Save changed card data
	if (!cardsToSave.empty())
	{
		error = SaveCardData();
		if (error.Failed())
			return error.Uncheck();
	}

	// Save changed card sets
	if (!cardSetsToSave.empty())
	{
		for (CardSet::sptr cardSet : cardSetsToSave)
		{
			error = SaveCardSet(cardSet);
			if (error.Failed())
				return error.Uncheck();
		}
	}

	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::LoadCardSets(const Path& path)
{
	CMG_LOG_INFO() << "Loading card sets from directory: " << path;

	String str = path.ToString();
	m_rootPackage = LoadCardSetPackage(
		PathU16(StringU16(str.begin(), str.end())), u"root");
	return CMG_ERROR_SUCCESS;
}

Card::sptr CardDatabase::DeserializeCard(rapidjson::Value& cardData,
	Array<std::pair<Card::sptr, CardRuKey>>& relatedCardPairs)
{
	Card::sptr card = cmg::make_shared<Card>();

	TryStringToEnum(cardData["type"].GetString(), card->m_type, false);
	card->m_text.russian = AccentedText(cardData["ru"].GetString());
	card->m_text.english = AccentedText(cardData["en"].GetString());
	String typeName = cardData["type"].GetString();
	if (cardData.HasMember("attrs"))
	{
		rapidjson::Value& attrDataList = cardData["attrs"];
		CardTags tag;
		for (auto it = attrDataList.Begin(); it != attrDataList.End(); it++)
		{
			if (TryStringToEnum(it->GetString(), tag, true))
			{
				card->m_tags.Set(tag, true);
			}
			else
			{
				CMG_LOG_ERROR() << "Unknown card tag: " << it->GetString();
			}
		}
	}
	if (cardData.HasMember("crtd"))
	{
		card->m_creationTimestamp = cardData["crtd"].GetDouble();
	}
	if (cardData.HasMember("rel"))
	{
		rapidjson::Value& relatedCardsDataList = cardData["rel"];
		for (auto it = relatedCardsDataList.Begin(); it != relatedCardsDataList.End(); it++)
		{
			rapidjson::Value& relatedCardData = *it;
			CardRuKey relatedCardKey;
			TryStringToEnum<WordType>(String(relatedCardData[0].GetString()), relatedCardKey.type);
			relatedCardKey.russian = ConvertFromUTF8(relatedCardData[1].GetString());
			relatedCardPairs.push_back({card, relatedCardKey});
		}
	}
	if (cardData.HasMember("ex"))
	{
		rapidjson::Value& exampleDataList = cardData["ex"];
		for (auto it = exampleDataList.Begin(); it != exampleDataList.End(); it++)
		{
			card->m_examples.push_back(
				TranslationPair((*it)[0].GetString(), (*it)[1].GetString()));
		}
	}

	return card;
}

void CardDatabase::SerializeCard(rapidjson::Value& outData,
	Card::sptr card, rapidjson::Document::AllocatorType& allocator)
{
	String typeName = EnumToString(card->GetWordType());
	String ru = ConvertToUTF8(card->GetRussian().ToMarkedString());
	String en = ConvertToUTF8(card->GetEnglish().ToMarkedString());
	outData.AddMember("type", rapidjson::Value(
		typeName.c_str(), allocator).Move(), allocator);
	outData.AddMember("ru", rapidjson::Value(ru.c_str(), allocator).Move(), allocator);
	outData.AddMember("en", rapidjson::Value(en.c_str(), allocator).Move(), allocator);
	outData.AddMember("crtd", card->GetCreationTimestamp(), allocator);

	// Related cards
	auto& relatedCards = card->GetRelatedCards();
	if (!relatedCards.empty())
	{
		rapidjson::Value relatedCardsListData(rapidjson::kArrayType);
		for (Card::sptr relatedCard : relatedCards)
		{
			rapidjson::Value jsonRelatedCard(rapidjson::kArrayType);
			SerializeCardKey(jsonRelatedCard, relatedCard->GetKey(), allocator);
			relatedCardsListData.PushBack(jsonRelatedCard.Move(), allocator);
		}
		outData.AddMember("rel", relatedCardsListData, allocator);
	}

	// Card tags
	auto& tags = card->GetTags();
	if (!tags.IsZero())
	{
		rapidjson::Value tagListData(rapidjson::kArrayType);
		for (auto it : tags)
		{
			if (it.second)
			{
				String tagName = EnumToShortString(it.first);
				tagListData.PushBack(rapidjson::Value(
					tagName.c_str(), allocator).Move(), allocator);
			}
		}
		outData.AddMember("attrs", tagListData, allocator);
	}
}

Error CardDatabase::DeserializeCardSet(rapidjson::Value& data, CardSet::sptr& cardSet)
{
	rapidjson::Value& cardSetData = data["card_set"];
	cardSet = cmg::make_shared<CardSet>(cardSetData["name"].GetString());
	String typeName = cardSetData["type"].GetString();
	cmg::string::ToLowerIP(typeName);
	CardSetType cardSetType;
	if (!TryStringToEnum(typeName, cardSetType, false))
	{
		CMG_LOG_ERROR() << "Unknown card set type: " << typeName;
		return CMG_ERROR_FAILURE;
	}
	cardSet->m_cardSetType = cardSetType;

	rapidjson::Value& cardDataList = cardSetData["cards"];
	for (auto it = cardDataList.Begin(); it != cardDataList.End(); it++)
	{
		CardRuKey key;
		TryStringToEnum((*it)[0].GetString(), key.type);
		key.russian = ConvertFromUTF8((*it)[1].GetString());
		std::replace(key.russian.begin(), key.russian.end(), u'ё', u'е');
		ru::ToLowerIP(key.russian);

		Card::sptr card = GetCard(key);
		if (card)
		{
			cardSet->AddCard(card);
			card->m_cardSets.insert(cardSet);
		}
		else
		{
			CMG_LOG_ERROR() << "Unknown card: " << key;
		}
	}
	
	return CMG_ERROR_SUCCESS;
}

CardSetPackage::sptr CardDatabase::LoadCardSetPackage(
	const PathU16& path, const AccentedText& name)
{
	CardSetPackage::sptr package = cmg::make_shared<CardSetPackage>(name);
	package->m_path = path;
	Error loadError;
	//CMG_LOG_INFO() << "Loading package: " << path.wstring();

	for (auto& p : std::filesystem::directory_iterator(
		std::filesystem::path((const wchar_t*) path.c_str())))
	{
		PathU16 subPath(p.path().u16string());

		if (p.is_directory())
		{
			CardSetPackage::sptr subPackage = LoadCardSetPackage(
				subPath, subPath.GetName());
			if (subPackage)
			{
				subPackage->SetParent(package);
				package->AddPackage(subPackage);
			}
		}
		else if (p.is_regular_file())
		{
			auto ext = p.path().extension();
			if (ext.string() == ".json" || ext.string() == ".yaml")
			{
				CardSet::sptr cardSet;
				LoadCardSet(subPath, cardSet);
				if (cardSet)
				{
					cardSet->m_path = subPath;
					cardSet->SetParent(package);
					package->AddCardSet(cardSet);
				}
			}
		}
	}

	return package;
}

Error CardDatabase::LoadCardSet(const PathU16& path, CardSet::sptr& outCardSet)
{
	Error error;
	CMG_LOG_INFO() << "Loading card set: " << path;

	// Open the json file
	rapidjson::Document document;
	error = json::LoadDocumentFromFile(path, document);
	if (error.Failed())
		return error.Uncheck();

	// Deserialize the card set
	DeserializeCardSet(document, outCardSet);
	if (error.Failed())
		return error.Uncheck();

	// Verify it has a unique name
	CardSetKey key = outCardSet->GetKey();
	auto it = m_keyToCardSets.find(key);
	if (it != m_keyToCardSets.end())
	{
		CMG_LOG_ERROR() << "Duplicate card set: " << key.name;
		return CMG_ERROR_FAILURE;
	}

	// Add the card set to the map
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutexCardSets);
		m_keyToCardSets[key] = outCardSet;
	}
	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::SaveCardSet(CardSet::sptr cardSet)
{
	return SaveCardSet(cardSet, cardSet->GetPath());
}

Error CardDatabase::SaveCardSet(CardSet::sptr cardSet, const PathU16& path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCardSets);

	CMG_LOG_INFO() << "Saving card set: " << path;

	rapidjson::Document document(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	
	rapidjson::Value cardSetData(rapidjson::kObjectType);
	auto name = ConvertToUTF8(cardSet->GetName().ToMarkedString());
	auto typeName = EnumToString(cardSet->GetCardSetType());
	cardSetData.AddMember("version", 1, allocator);
	cardSetData.AddMember("name", rapidjson::Value(name.c_str(), allocator).Move(), allocator);
	cardSetData.AddMember("type", rapidjson::Value(typeName.c_str(), allocator).Move(), allocator);

	// Create the list of cards
	rapidjson::Value cardDataList(rapidjson::kArrayType);
	for (Card::sptr card : cardSet->GetCards())
	{
		rapidjson::Value cardData(rapidjson::kArrayType);
		SerializeCardKey(cardData, card->GetKey(), allocator);
		cardDataList.PushBack(cardData.Move(), allocator);
	}
	cardSetData.AddMember("cards", cardDataList, allocator);
	document.AddMember("card_set", cardSetData, allocator);

	// Save to the file
	Error error = json::SaveDocumentToFile(path, document);
	if (error.Failed())
		return error.Uncheck();

	// Remove the card set from the dirty state
	{
		std::lock_guard<std::mutex> guard(m_mutexDirty);
		if (m_dirtyCardSets.find(cardSet) != m_dirtyCardSets.end())
			m_dirtyCardSets.erase(cardSet);
	}

	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::AddCard(Card::sptr card)
{
	CardRuKey ruKey = card->GetRuKey();
	CardEnKey enKey = card->GetEnKey();
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
		if (GetCard(ruKey))
		{
			CMG_LOG_ERROR() << "Duplicate russian key: " << ruKey;
			return CMG_ERROR_FAILURE;
		}
		if (GetCard(enKey))
		{
			CMG_LOG_ERROR() << "Duplicate english key: " << enKey;
			return CMG_ERROR_FAILURE;
		}

		m_russianToCards[ruKey] = card;
		m_englishToCards[enKey] = card;

		MarkCardDirty(card, true);
	}
	m_cardCreated.Emit(card);
	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::ModifyCard(Card::sptr card, const CardData& changes)
{
	CMG_LOG_DEBUG() << "Modifying card '" << card->GetRussian() << "'";
	CardRuKey newRuKey(changes.type, changes.text.russian);
	CardEnKey newEnKey(changes.type, changes.text.english, changes.tags);
	CardRuKey oldRuKey;
	CardEnKey oldEnKey;
	bool ruKeyChanged, enKeyChanged;
	bool changed = false;
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
		
		// Check for and validate key changes
		oldRuKey = card->GetRuKey();
		oldEnKey = card->GetEnKey();
		ruKeyChanged = (newRuKey != oldRuKey);
		enKeyChanged = (newEnKey != oldEnKey);
		if (ruKeyChanged && GetCard(newRuKey))
		{
			CMG_LOG_ERROR() << "Duplicate russian key: " << newRuKey;
			return CMG_ERROR_FAILURE;
		}
		if (enKeyChanged && GetCard(newEnKey))
		{
			CMG_LOG_ERROR() << "Duplicate english key: " << newEnKey;
			return CMG_ERROR_FAILURE;
		}

		// Apply the key change
		if (ruKeyChanged)
		{
			m_russianToCards.erase(oldRuKey);
			m_russianToCards[newRuKey] = card;
		}
		if (enKeyChanged)
		{
			m_englishToCards.erase(oldEnKey);
			m_englishToCards[newEnKey] = card;
		}

		// Apply card data changes
		if (card->m_type != changes.type)
		{
			changed = true;
			card->m_type = changes.type;
		}
		if (card->m_text != changes.text)
		{
			changed = true;
			card->m_text = changes.text;
		}
		if (card->m_tags != changes.tags)
		{
			changed = true;
			card->m_tags = changes.tags;
		}
		if (card->m_relatedCards != changes.relatedCards)
		{
			changed = true;
			Set<Card::sptr> oldRelatedCards = card->GetRelatedCards();
			const Set<Card::sptr>& newRelatedCards = changes.relatedCards;
			for (Card::sptr relatedCard : oldRelatedCards)
			{
				if (!cmg::container::Contains(newRelatedCards, relatedCard))
					UnlinkRelatedCards(card, relatedCard);
			}
			for (Card::sptr relatedCard : newRelatedCards)
			{
				if (!cmg::container::Contains(oldRelatedCards, relatedCard))
					LinkRelatedCards(card, relatedCard);
			}
		}
	}
	if (changed)
		MarkCardDirty(card, ruKeyChanged);
	if (ruKeyChanged)
		m_cardKeyChanged.Emit(card, oldRuKey);
	if (changed)
		m_cardDataChanged.Emit(card);
	return CMG_ERROR_SUCCESS;
}

void CardDatabase::AddCardToSet(Card::sptr card, CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	if (cardSet->AddCard(card))
	{
		card->m_cardSets.insert(cardSet);
		CMG_LOG_DEBUG() << "Added card '" << card->GetRussian() <<
			"' to set '" << cardSet->GetName() << "'";
		MarkCardSetDirty(cardSet);
		m_cardAddedToSet.Emit(card, cardSet);
	}
}

void CardDatabase::RemoveCardFromSet(Card::sptr card, CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	if (cardSet->RemoveCard(card))
	{
		card->m_cardSets.erase(cardSet);
		CMG_LOG_DEBUG() << "Removed card '" << card->GetRussian() <<
			"' from set '" << cardSet->GetName() << "'";
		MarkCardSetDirty(cardSet);
		m_cardRemovedFromSet.Emit(card, cardSet);
	}
}

void CardDatabase::LinkRelatedCards(Card::sptr a, Card::sptr b)
{
	CMG_LOG_DEBUG() << "Linking related cards '" << a->GetRussian() <<
		"' and '" << b->GetRussian() << "'";
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
		a->m_relatedCards.insert(b);
		b->m_relatedCards.insert(a);
	}
	MarkCardDirty(a, false);
	MarkCardDirty(b, false);
	m_cardDataChanged.Emit(a);
	m_cardDataChanged.Emit(b);
}

void CardDatabase::UnlinkRelatedCards(Card::sptr a, Card::sptr b)
{
	CMG_LOG_DEBUG() << "Unlinking related cards '" << a->GetRussian() <<
		"' and '" << b->GetRussian() << "'";
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
		a->m_relatedCards.erase(b);
		b->m_relatedCards.erase(a);
	}
	MarkCardDirty(a, false);
	MarkCardDirty(b, false);
	m_cardDataChanged.Emit(a);
	m_cardDataChanged.Emit(b);
}

CardSet::sptr CardDatabase::CreateCardSet(CardSetPackage::sptr package,
	const AccentedText& name, const unistr& fileName, CardSetType cardSetType)
{
	CardSet::sptr cardSet = cmg::make_shared<CardSet>(name);
	cardSet->m_cardSetType = cardSetType;
	CardSetKey key = cardSet->GetKey();
	CMG_LOG_INFO() << "Creating card set: " << key.name;

	cardSet->m_path = package->GetPath() / (fileName + u".json");

	// Verify it has a unique name
	auto it = m_keyToCardSets.find(key);
	if (it != m_keyToCardSets.end())
	{
		CMG_LOG_ERROR() << "Duplicate card set: " << key.name;
		return nullptr;
	}

	// Add the card set to the map
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutexCardSets);
		m_keyToCardSets[key] = cardSet;
	}

	cardSet->SetParent(package);
	package->AddCardSet(cardSet);

	// Save the card set to create it
	SaveCardSet(cardSet);

	m_cardSetCreated.Emit(cardSet);
	return cardSet;
}

void CardDatabase::SerializeCardKey(rapidjson::Value& arrayValue,
	const CardKey& key, rapidjson::Document::AllocatorType& allocator)
{
	String keyType = EnumToString(key.type);
	String keyRussian = ConvertToUTF8(key.russian);
	String keyEnglish = ConvertToUTF8(key.english);
	arrayValue.PushBack(rapidjson::Value(keyType.c_str(), allocator).Move(), allocator);
	arrayValue.PushBack(rapidjson::Value(keyRussian.c_str(), allocator).Move(), allocator);
	arrayValue.PushBack(rapidjson::Value(keyEnglish.c_str(), allocator).Move(), allocator);
}
