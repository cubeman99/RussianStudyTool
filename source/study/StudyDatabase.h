#pragma once

#include "Card.h"
#include "rapidjson/document.h"
#include <mutex>


#define ENUM_MACRO_PROFICIENCY_LEVEL(_context, _ex) \
	_ex(_context, new, , new) \
	_ex(_context, hard, , hard) \
	_ex(_context, medium, , medium) \
	_ex(_context, easy, , easy) \
	_ex(_context, learned, , learned)
DECLARE_ENUM_WITH_COUNT(ProficiencyLevel, ENUM_MACRO_PROFICIENCY_LEVEL)


class CardStudyData
{
public:
	friend class StudyDatabase;

	CardStudyData() {}

	ProficiencyLevel GetProficiencyLevel() const { return m_proficiencyLevel; }
	float GetLastEncounterTime() const { return m_lastEncounterTime; }

private:
	Array<bool> m_history;
	float m_lastEncounterTime = -1.0f;
	ProficiencyLevel m_proficiencyLevel = ProficiencyLevel::k_new;
};


class StudyDatabase
{
public:
	StudyDatabase();
	~StudyDatabase();

	CardStudyData GetCardStudyData(Card::sptr card);
	CardStudyData GetCardStudyData(const CardKey& key);
	CardStudyData CreateCardStudyData(const CardKey& key);
	void Clear();
	void MarkCard(Card::sptr card, bool knewIt);

	Error LoadStudyData(const Path& path);
	Error SaveStudyData(const Path& path);

	static Error DeserializeCardStudyData(rapidjson::Value& data,
		CardKey& outKey, CardStudyData& outCardStudyData);

private:
	Map<CardKey, CardStudyData> m_cardStudyData;
	std::recursive_mutex m_mutexStudyData;
};
