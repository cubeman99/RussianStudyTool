#include "CardDatabase.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"

CardDatabase::CardDatabase()
{
}

CardDatabase::~CardDatabase()
{
}

Card::sptr CardDatabase::GetCard(const CardKey & key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	auto it = m_cards.find(key);
	if (it != m_cards.end())
		return it->second;
	return nullptr;
}

Card::sptr CardDatabase::GetCard(const CardEnKey & enKey)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	auto it = m_englishToCards.find(enKey);
	if (it != m_englishToCards.end())
		return it->second;
	return nullptr;
}

Card::sptr CardDatabase::GetCard(const CardRuKey & ruKey)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	auto it = m_russianToCards.find(ruKey);
	if (it != m_russianToCards.end())
		return it->second;
	return nullptr;
}

CardSet::sptr CardDatabase::GetCardSet(const CardSetKey & key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCardSets);
	auto it = m_keyToCardSets.find(key);
	if (it != m_keyToCardSets.end())
		return it->second;
	return nullptr;
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


Error CardDatabase::LoadCardData(const Path& path)
{
	CMG_LOG_INFO() << "Loading cards";

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

	CMG_LOG_INFO() << "Loaded " << m_cards.size() << " cards!";
	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::SaveCardData(const Path& path)
{
	return CMG_ERROR_SUCCESS;
}

Error CardDatabase::LoadCardSets(const Path& path)
{
	CMG_LOG_INFO() << "Loading card sets from directory: " << path;

	LoadCardSetPackage(String(path), u"root");
	return CMG_ERROR_SUCCESS;
}

Card::sptr CardDatabase::DeserializeCard(rapidjson::Value& cardData,
	Array<std::pair<Card::sptr, CardKey>>& relatedCardPairs)
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
		card->m_creationTimestamp = cardData["crtd"].GetFloat();
	}
	if (cardData.HasMember("rel"))
	{
		rapidjson::Value& relatedCardsDataList = cardData["rel"];
		for (auto it = relatedCardsDataList.Begin(); it != relatedCardsDataList.End(); it++)
		{
			rapidjson::Value& relatedCardData = *it;
			CardKey relatedCardKey;
			TryStringToEnum<WordType>(String(relatedCardData[0].GetString()), relatedCardKey.type);
			relatedCardKey.russian = ConvertFromUTF8(relatedCardData[1].GetString());
			relatedCardKey.english = ConvertFromUTF8(relatedCardData[2].GetString());
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

CardSet::sptr CardDatabase::DeserializeCardSet(rapidjson::Value& data)
{
	rapidjson::Value& cardSetData = data["card_set"];
	CardSet::sptr cardSet = cmg::make_shared<CardSet>(cardSetData["name"].GetString());

	rapidjson::Value& cardDataList = cardSetData["cards"];
	for (auto it = cardDataList.Begin(); it != cardDataList.End(); it++)
	{
		CardKey key;
		TryStringToEnum((*it)[0].GetString(), key.type);
		key.russian = ConvertFromUTF8((*it)[1].GetString());
		ru::ToLowerIP(key.russian);
		key.english = ConvertFromUTF8((*it)[2].GetString());
		ru::ToLowerIP(key.english);

		Card::sptr card = GetCard(key);
		if (card)
		{
			cardSet->AddCard(card);
		}
		else
		{
			CMG_LOG_ERROR() << "Unknown card: " << key;
		}
	}
	
	return cardSet;
}

CardSetPackage::sptr CardDatabase::LoadCardSetPackage(
	const std::filesystem::path& path, const AccentedText& name)
{
	CardSetPackage::sptr package = cmg::make_shared<CardSetPackage>(name);
	auto str = path.wstring();
	//CMG_LOG_INFO() << "Loading package: " << path.wstring();

	for(auto& p: std::filesystem::directory_iterator(path))
	{
		if (p.is_directory())
		{
			CardSetPackage::sptr subPackage = LoadCardSetPackage(
				p.path(), p.path().filename().wstring());
			if (subPackage)
				package->AddPackage(subPackage);
		}
		else if (p.is_regular_file())
		{
			auto ext = p.path().extension();
			if (ext.string() == ".json")
			{
				CardSet::sptr cardSet = LoadCardSet(p.path());
				if (cardSet)
					package->AddCardSet(cardSet);
			}
		}
	}

	return package;
}

CardSet::sptr CardDatabase::LoadCardSet(const std::filesystem::path& path)
{
	auto str = path.wstring();
	//CMG_LOG_INFO() << "Loading card set: " << str;

	// Open the json file
	rapidjson::Document document;
	std::ifstream file(path);
	std::string json((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	document.Parse(json.c_str());
	if (document.HasParseError())
	{
		rapidjson::ParseResult result = (rapidjson::ParseResult) document;
		CMG_LOG_ERROR() << "JSON Parse error!";
		return nullptr;
	}

	CardSet::sptr cardSet = DeserializeCardSet(document);
	auto key = cardSet->GetKey();
	auto it = m_keyToCardSets.find(key);
	if (it != m_keyToCardSets.end())
	{
		CMG_LOG_ERROR() << "Duplicate card set: " << key.name;
		return nullptr;
	}

	{
		std::lock_guard<std::recursive_mutex> guard(m_mutexCardSets);
		m_keyToCardSets[key] = cardSet;
	}
	return cardSet;
}

Error CardDatabase::AddCard(Card::sptr card)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexCards);
	
	//std::cout << card.m_text.russian << " -- " << card.m_text.russian << std::endl;
	CardKey key = card->GetKey();
	CardRuKey ruKey = card->GetRuKey();
	CardEnKey enKey = card->GetEnKey();

	if (GetCard(key))
		CMG_LOG_ERROR() << "Duplicate key: " << key;
	if (GetCard(ruKey))
		CMG_LOG_ERROR() << "Duplicate russian key: " << ruKey;
	if (GetCard(enKey))
		CMG_LOG_ERROR() << "Duplicate english key: " << enKey;

	m_cards[key] = card;
	m_russianToCards[ruKey] = card;
	m_englishToCards[enKey] = card;
	return CMG_ERROR_SUCCESS;
}
