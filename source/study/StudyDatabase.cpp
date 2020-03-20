#include "StudyDatabase.h"
#include "Config.h"
#include "core/JsonUtility.h"

StudyDatabase::StudyDatabase(CardDatabase& cardDatabase) :
	m_cardDatabase(cardDatabase)
{
	m_cardDatabase.CardKeyChanged().Connect(this, &StudyDatabase::OnCardKeyChanged);
	m_cardDatabase.CardDeleted().Connect(this, &StudyDatabase::OnCardDeleted);
	m_cardDatabase.CardAddedToSet().Connect(this, &StudyDatabase::OnCardAddedToSet);
	m_cardDatabase.CardRemovedFromSet().Connect(this, &StudyDatabase::OnCardRemovedFrom);
}

StudyDatabase::~StudyDatabase()
{
	Clear();
}

const Path& StudyDatabase::GetStudyDataPath() const
{
	return m_path;
}

CardStudyData& StudyDatabase::GetCardStudyData(Card::sptr card)
{
	return GetCardStudyData(card->GetRuKey());
}

CardStudyData& StudyDatabase::GetCardStudyData(const CardRuKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardStudyData.find(key);
	if (it != m_cardStudyData.end())
		return m_cardStudyData[key];
	return CreateCardStudyData(key);
}

CardStudyData& StudyDatabase::CreateCardStudyData(const CardRuKey& key)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CardStudyData data;
	m_cardStudyData[key] = data;
	return m_cardStudyData[key];
}

StudySetMetrics StudyDatabase::GetStudyMetrics(CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardSetMetrics.find(cardSet);
	if (it != m_cardSetMetrics.end())
		return m_cardSetMetrics[cardSet];
	return CalcStudyMetrics(cardSet);
}

StudySetMetrics StudyDatabase::GetStudyMetrics(CardSetPackage::sptr package)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_packageMetrics.find(package);
	if (it != m_packageMetrics.end())
		return m_packageMetrics[package];
	return CalcStudyMetrics(package);
}

StudySetMetrics StudyDatabase::GetStudySetMetrics(const IStudySet* studySet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	StudySetMetrics metrics;
	uint32 totalCount = 0;
	for (Card::sptr card : studySet->GetCards())
	{
		auto& cardStudyData = GetCardStudyData(card);
		metrics.Add(cardStudyData);
		totalCount++;
	}
	return metrics;
}

void StudyDatabase::RecalcStudySetMetrics(CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_cardSetMetrics.erase(cardSet);
	if (cardSet->GetParent())
		RecalcStudySetMetrics(cardSet->GetParent());
}

void StudyDatabase::RecalcStudySetMetrics(CardSetPackage::sptr package)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_packageMetrics.erase(package);
	if (package->GetParent())
		RecalcStudySetMetrics(package->GetParent());
}

StudySetMetrics& StudyDatabase::CalcStudyMetrics(CardSet::sptr cardSet)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_cardSetMetrics[cardSet] = GetStudySetMetrics((IStudySet*) cardSet.get());
	return m_cardSetMetrics[cardSet];
}

StudySetMetrics& StudyDatabase::CalcStudyMetrics(CardSetPackage::sptr package)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyMetrics);
	m_packageMetrics[package] = GetStudySetMetrics((IStudySet*) package.get());
	return m_packageMetrics[package];
}

void StudyDatabase::Clear()
{
	std::lock_guard<std::recursive_mutex> guardData(m_mutexStudyData);
	std::lock_guard<std::recursive_mutex> guardMetrics(m_mutexStudyMetrics);
	m_cardStudyData.clear();
	m_cardSetMetrics.clear();
	m_packageMetrics.clear();
}

void StudyDatabase::MarkCard(Card::sptr card, bool knewIt, Language shownSide)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto& studyData = GetCardStudyData(card);
	studyData.AddToHistory(knewIt, GetAppTimestamp(), shownSide);
	OnCardStudyDataChanged(card);
}

void StudyDatabase::SetStudyDataPath(const Path& path)
{
	m_path = path;
}

Error StudyDatabase::LoadStudyData()
{
	return LoadStudyData(m_path);
}

Error StudyDatabase::LoadStudyData(const Path& path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CMG_LOG_INFO() << "Loading study data";

	m_cardSetMetrics.clear();
	m_packageMetrics.clear();
	m_cardStudyData.clear();

	// Open the json file
	m_path = path;
	rapidjson::Document document;
	Error error = json::LoadDocumentFromFile(path, document);
	if (error.Failed())
		return error.Uncheck();

	// Gather all card data
	rapidjson::Value& cardDataList = document["cards"];
	for (auto it = cardDataList.Begin(); it != cardDataList.End(); it++)
	{
		CardRuKey key;
		CardStudyData studyData;
		error = studyData.Deserialize(*it, key);
		if (error.Failed())
			return error.Uncheck();
		m_cardStudyData[key] = studyData;
	}

	MarkDirty(false);
	return CMG_ERROR_SUCCESS;
}

Error StudyDatabase::SaveStudyData()
{
	return SaveStudyData(m_path);
}

Error StudyDatabase::SaveStudyData(const Path& path)
{
	if (m_readOnly)
		return CMG_ERROR_SUCCESS;

	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);

	rapidjson::Document document(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// Save the list of card study data
	rapidjson::Value studyDataList(rapidjson::kArrayType);
	document.AddMember("save_time", GetAppTimestamp(), allocator);
	uint32_t savedCardCount = 0;
	for (auto it : m_cardStudyData)
	{
		const CardRuKey& key = it.first;
		const CardStudyData& studyData = it.second;
		if (it.second.IsEncountered())
		{
			rapidjson::Value jsonStudyData(rapidjson::kObjectType);
			studyData.Serialize(jsonStudyData, allocator, key);
			studyDataList.PushBack(jsonStudyData.Move(), allocator);
			savedCardCount++;
		}
	}
	document.AddMember("cards", studyDataList, allocator);

	// Save to the file
	MarkDirty(false);
	return json::SaveDocumentToFile(path, document);
}

Error StudyDatabase::SaveChanges()
{
	bool saveStudyData = false;

	// Retreive dirty state
	{
		std::lock_guard<std::mutex> guardDirty(m_mutexDirty);
		saveStudyData = m_isDirty;
	}

	// Save study data
	if (saveStudyData)
	{
		return SaveStudyData();
	}

	return CMG_ERROR_SUCCESS;
}

void StudyDatabase::OnCardKeyChanged(Card::sptr card, CardRuKey oldKey)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	auto it = m_cardStudyData.find(oldKey);
	if (it != m_cardStudyData.end())
	{
		m_cardStudyData[card->GetRuKey()] = it->second;
		m_cardStudyData.erase(oldKey);
	}
	MarkDirty();
}

void StudyDatabase::OnCardDeleted(Card::sptr card)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexStudyData);
	CardRuKey key = card->GetRuKey();
	auto it = m_cardStudyData.find(key);
	if (it != m_cardStudyData.end())
		m_cardStudyData.erase(key);
	MarkDirty();
}

void StudyDatabase::OnCardAddedToSet(Card::sptr card, CardSet::sptr cardSet)
{
	RecalcStudySetMetrics(cardSet);
}

void StudyDatabase::OnCardRemovedFrom(Card::sptr card, CardSet::sptr cardSet)
{
	RecalcStudySetMetrics(cardSet);
}

void StudyDatabase::OnCardStudyDataChanged(Card::sptr card)
{
	MarkDirty();
	for (CardSet::sptr cardSet : m_cardDatabase.GetCardSetsWithCard(card))
		RecalcStudySetMetrics(cardSet);
	m_cardStudyDataChanged.Emit(card);
}

void StudyDatabase::MarkDirty(bool dirty)
{
	std::lock_guard<std::mutex> guard(m_mutexDirty);
	m_isDirty = dirty;
}
