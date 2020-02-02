#pragma once

#include "study/StudySetMetrics.h"
#include "study/CardStudyData.h"
#include "cards/CardDatabase.h"
#include "rapidjson/document.h"
#include <mutex>

class StudyDatabase
{
public:
	StudyDatabase(CardDatabase& cardDatabase);
	~StudyDatabase();

	CardStudyData& GetCardStudyData(Card::sptr card);
	CardStudyData& GetCardStudyData(const CardKey& key);
	CardStudyData& CreateCardStudyData(const CardKey& key);
	StudySetMetrics GetStudyMetrics(CardSet::sptr cardSet);
	StudySetMetrics GetStudyMetrics(CardSetPackage::sptr package);
	StudySetMetrics GetStudySetMetrics(const IStudySet* studySet);

	void SetReadOnly(bool readOnly) { m_readOnly = readOnly; }
	void RecalcStudySetMetrics(CardSet::sptr cardSet);
	void RecalcStudySetMetrics(CardSetPackage::sptr package);
	StudySetMetrics& CalcStudyMetrics(CardSet::sptr cardSet);
	StudySetMetrics& CalcStudyMetrics(CardSetPackage::sptr package);

	void Clear();
	void MarkCard(Card::sptr card, bool knewIt);


	Error LoadStudyData(const Path& path);
	Error SaveStudyData();
	Error SaveStudyData(const Path& path);

	static Error DeserializeCardStudyData(rapidjson::Value& data,
		CardKey& outKey, CardStudyData& outCardStudyData);

private:
	void OnCardKeyChanged(Card::sptr card, CardRuKey oldKey);
	void OnCardDeleted(Card::sptr card);
	void OnCardStudyDataChanged(Card::sptr card);
	void MarkDirty(bool dirty = true);

	StudyDatabase(const StudyDatabase& copy) = delete;
	StudyDatabase& operator=(const StudyDatabase& copy) = delete;

private:
	CardDatabase& m_cardDatabase;

	Path m_path;
	bool m_readOnly = false;

	// TODO: use RU key
	Map<CardKey, CardStudyData> m_cardStudyData;
	Map<CardSet::sptr, StudySetMetrics> m_cardSetMetrics;
	Map<CardSetPackage::sptr, StudySetMetrics> m_packageMetrics;

	std::recursive_mutex m_mutexStudyData;
	std::mutex m_mutexDity;
	bool m_isDirty = false;
};
