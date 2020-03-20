#pragma once

#include "Config.h"
#include <cmgCore/cmg_core.h>


class StudyParams
{
public:
	StudyParams()
	{
	}

	uint32 GetMaxRepetitions() const { return m_maxRepetitions; }
	uint32 GetNewItemInterval() const { return m_newItemInterval; }
	uint32 GetMinRepeatInterval() const { return m_minRepeatInterval; }

private:
	uint32 m_newItemInterval = Config::k_newCardInterval;
	uint32 m_minRepeatInterval = Config::k_minRepeatInterval;
	uint32 m_maxRepetitions = 0;
};


template <typename T_Item, class T_Set>
struct ItemSchedulingInfo
{
	T_Item item;
	int32 rep = 0;
	uint32 shownCount = 0;
	int32 minRepeatInterval = 8;
	bool done = false;
	int32 age = 0;
};


template <typename T_Item, class T_Set>
class SchedulerBase
{
public:
	using scheduling_info = typename ItemSchedulingInfo<T_Item, T_Set>;
	using scheduling_info_sptr = cmg::shared_ptr<scheduling_info>;

	SchedulerBase();
	virtual ~SchedulerBase() {}

	virtual bool IsItemEncountered(T_Item item) = 0;
	virtual int32 GetRepeatInterval(T_Item item) = 0;

	const StudyParams& GetStudyParams() const { return m_params; }
	T_Set* GetStudySet() const { return m_studySet; }

	void Begin(T_Set* studySet, const StudyParams& params = StudyParams());
	void MarkItem(T_Item item);
	T_Item NextItem();

protected:

	virtual T_Item GetNextItem();
	scheduling_info_sptr GetItemSchedulingInfo(T_Item item);

	T_Set* m_studySet;
	StudyParams m_params;
	int32 m_rep = 1;
	Map<T_Item, scheduling_info_sptr> m_itemInfoMap;
};

template<typename T_Item, class T_Set>
inline SchedulerBase<T_Item, T_Set>::SchedulerBase()
{
}

template<typename T_Item, class T_Set>
inline void SchedulerBase<T_Item, T_Set>::Begin(T_Set * studySet, const StudyParams & params)
{
	m_studySet = studySet;
	m_params = params;
	m_itemInfoMap.clear();

	m_rep = 1;
	for (auto item : m_studySet->GetCards())
	{
		auto info = GetItemSchedulingInfo(item);
		if (info->done)
			continue;
		info->minRepeatInterval = GetRepeatInterval(item);
		m_rep = Math::Max(m_rep, info->minRepeatInterval);
	}
}

template<typename T_Item, class T_Set>
inline void SchedulerBase<T_Item, T_Set>::MarkItem(T_Item item)
{
	auto info = GetItemSchedulingInfo(item);
	info->rep = m_rep;
	info->shownCount++;

	// Check if we have shown this card too many times
	if (m_params.GetMaxRepetitions() > 0 &&
		info->shownCount >= m_params.GetMaxRepetitions())
	{
		info->done = true;
	}
}

template<typename T_Item, class T_Set>
inline T_Item SchedulerBase<T_Item, T_Set>::NextItem()
{
	T_Item item = GetNextItem();
	m_rep++;
	return item;
}

template<typename T_Item, class T_Set>
inline T_Item SchedulerBase<T_Item, T_Set>::GetNextItem()
{
	// Calculate item ages and find new items
	Set<T_Item> newItems;
	int32 maxAge = INT_MIN;
	for (auto item : m_studySet->GetCards())
	{
		auto info = GetItemSchedulingInfo(item);
		if (info->done)
			continue;
		info->minRepeatInterval = GetRepeatInterval(item);
		info->age = (int32) m_rep - ((int32) info->rep + (int32) info->minRepeatInterval);
		maxAge = Math::Max(maxAge, info->age);
		if (!IsItemEncountered(item))
			newItems.insert(item);
	}
	maxAge = Math::Min(maxAge, 0);

	// Show a new item every new item interval
	if (!newItems.empty() && m_rep % m_params.GetNewItemInterval() == 1)
		return Random::ChooseFromSet(newItems);

	// Calculate item 'weights' from their ages
	//CMG_LOG_DEBUG() << "SCHEDULER: rep = " << m_rep;
	Array<T_Item> readyItems;
	Array<float> itemWeights;
	for (auto item : m_studySet->GetCards())
	{
		auto info = GetItemSchedulingInfo(item);
		if (info->done)
			continue;
		int32 adjustedAge = info->age - maxAge;
		if (IsItemEncountered(item) && adjustedAge >= 0)
		{
			float weight = Math::Pow((float) (adjustedAge + 1), 2.5f);
			readyItems.push_back(item);
			itemWeights.push_back(weight);
		}

		/*
		int32 count = 30 - item->GetRussian().GetString().length();
		String spacing = "";
		String ageText = "";
		for (int32 i = 0; i < count; i++)
			spacing += " ";
		for (int32 i = 0; i <= adjustedAge; i++)
			ageText += "-";
		CMG_LOG_DEBUG() << "  " <<
			spacing << item->GetRussian() << ": " <<
			(!IsItemEncountered(item) ? "Y" : " ") <<
			std::setw(7) << info->rep <<
			std::setw(7) << info->minRepeatInterval <<
			std::setw(7) << info->age << " : " << ageText;*/
	}

	if (!readyItems.empty())
		return Random::ChooseWeighted<T_Item, float>(readyItems, itemWeights);
	if (!newItems.empty())
		return Random::ChooseFromSet(newItems);

	return T_Item();
}

template<typename T_Item, class T_Set>
inline typename SchedulerBase<T_Item, T_Set>::scheduling_info_sptr
SchedulerBase<T_Item, T_Set>::GetItemSchedulingInfo(T_Item item)
{
	if (m_itemInfoMap.find(item) == m_itemInfoMap.end())
	{
		scheduling_info_sptr info = cmg::make_shared<scheduling_info>();
		info->item = item;
		info->minRepeatInterval = GetRepeatInterval(item);
		info->rep = 0;
		m_itemInfoMap[item] = info;
		return info;
	}
	return m_itemInfoMap[item];
}
