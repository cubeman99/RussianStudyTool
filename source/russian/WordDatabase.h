#pragma once

#include "cards/CardSetPackage.h"
#include "rapidjson/document.h"
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "gui/Events.h"
#include "external/wiktionary/Wiktionary.h"
#include "russian/CardWordMatch.h"


class WordDatabase
{
public:
	WordDatabase(wiki::Wiktionary& wiktionary);
	~WordDatabase();

	wiki::Wiktionary& GetWiktionary();
	CardWordMatch GetWordFromCard(Card::sptr card,
		bool download = true, bool downloadInThread = false);
	CardWordMatch GetWordFromCard(const CardRuKey& card,
		bool download = true, bool downloadInThread = false);

	EventSignal<const CardWordMatch&>& TermDownloaded() { return m_termDownloaded; }

	void RunThread();
	void ProcessEvents();

private:
	WordDatabase(const WordDatabase& copy) = delete;
	WordDatabase& operator=(const WordDatabase& copy) = delete;

	void StopThread();

	static void ThreadEntryFunction(WordDatabase* database);
	
	void AddTask(const CardWordMatch& task);
	bool GetNextTask(CardWordMatch& outTask);
	void LoadCardWordMatch(CardWordMatch& task);

	wiki::Wiktionary& m_wiktionary;

	bool m_isRunning = false;
	std::thread* m_thread = nullptr;
	std::vector<CardWordMatch> m_wordTaskList;
	std::vector<CardWordMatch> m_taskOutputList;
	std::mutex m_mutexWordTaskList;
	std::mutex m_mutexTaskOutputList;

	EventSignal<const CardWordMatch&> m_termDownloaded;
};
