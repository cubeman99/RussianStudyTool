#include "WordDatabase.h"

WordDatabase::WordDatabase(wiki::Wiktionary& wiktionary) :
	m_wiktionary(wiktionary)
{
	m_isRunning = true;
	m_thread = new std::thread(&WordDatabase::ThreadEntryFunction, this);
}

WordDatabase::~WordDatabase()
{
	StopThread();
}

wiki::Wiktionary& WordDatabase::GetWiktionary()
{
	return m_wiktionary;
}

CardWordMatch WordDatabase::GetWordFromCard(Card::sptr card,
	bool download, bool downloadInThread)
{
	return GetWordFromCard(
		card->GetRuKey(), card, download, downloadInThread);
}

CardWordMatch WordDatabase::GetWordFromCard(const CardRuKey& key,
	bool download, bool downloadInThread)
{
	return GetWordFromCard(key, nullptr, download, downloadInThread);
}

CardWordMatch WordDatabase::GetWordFromCard(
	const CardRuKey& key, Card::sptr card,
	bool download, bool downloadInThread)
{
	const unistr& str = key.russian;
	uint32 length = str.length();
	CardWordMatch match;
	match.key = key;
	match.card = card;
	CardVariantWordMatch variant;
	CardVariantWordMatchPart part;

	for (uint32 i = 0; i <= length; i++)
	{
		unichar c = (i < length ? ru::ToLowerChar(str[i]) : u';');

		if (ru::IsRussian(c) || c == u'-')
		{
			part.text += c;
		}
		else
		{
			if (part.text.length() > 0)
			{
				// Part complete
				if (!variant.text.empty())
					variant.text += u' ';
				variant.text += part.text;
				variant.parts.push_back(part);
				// Begin a new part
				part = CardVariantWordMatchPart();
			}
			if (c == u';' && !variant.text.empty())
			{
				// Create a word-part for the whole pattern if has multiple words
				if (variant.parts.size() > 1)
				{
					part.text = variant.text;
					variant.parts.insert(variant.parts.begin(), part);
				}
				if (!variant.parts.empty())
					variant.parts.begin()->isMatchingText = true;
				// Pattern completed
				match.variants.push_back(variant);
				// Begin a new variant and part
				variant = CardVariantWordMatch();
				part = CardVariantWordMatchPart();
			}
		}
	}

	// No word parts! Only happens if no russian text in card.
	if (match.Empty())
		return match;

	// Get the available terms each part
	for (auto& variant : match.variants)
	{
		for (auto& part : variant.parts)
		{
			bool needsDownload = false;
			m_wiktionary.GetTerm(part.text, part.term, needsDownload);
			part.isLoaded = !needsDownload;
			part.SetWord(match.key.type);
		}
	}

	// If any part is not loaded, then add a task to load it
	if (!match.IsFullyLoaded())
	{
		if (download && downloadInThread)
			AddTask(match);
		else if (download)
			LoadCardWordMatch(match);
	}

	return match;
}

void WordDatabase::RunThread()
{
	CardWordMatch task;
	while (m_isRunning)
	{
		if (GetNextTask(task))
		{
			LoadCardWordMatch(task);
			{
				std::lock_guard<std::mutex> guard(m_mutexTaskOutputList);
				m_taskOutputList.push_back(task);
			}
		}

		std::chrono::milliseconds timespan(100);
		std::this_thread::sleep_for(timespan);
	}
}

void WordDatabase::ProcessEvents()
{
	CardWordMatch task;
	while (true)
	{
		// Get the next task output
		{
			std::lock_guard<std::mutex> guard(m_mutexTaskOutputList);
			if (m_taskOutputList.empty())
				return;
			task = m_taskOutputList.front();
			m_taskOutputList.erase(m_taskOutputList.begin());
		}

		CMG_LOG_DEBUG() << "Completed word task: " << task.key;
		m_termDownloaded.Emit(task);
	}
}

void WordDatabase::StopThread()
{
	m_isRunning = false;
	if (m_thread)
	{
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
	}
}

void WordDatabase::ThreadEntryFunction(WordDatabase* database)
{
	database->RunThread();
}

void WordDatabase::AddTask(const CardWordMatch& task)
{
	std::lock_guard<std::mutex> guard(m_mutexWordTaskList);
	m_wordTaskList.push_back(task);
}

bool WordDatabase::GetNextTask(CardWordMatch& outTask)
{
	std::lock_guard<std::mutex> guard(m_mutexWordTaskList);
	if (m_wordTaskList.empty())
		return false;
	outTask = m_wordTaskList.front();
	m_wordTaskList.erase(m_wordTaskList.begin());
	return true;
}

void WordDatabase::LoadCardWordMatch(CardWordMatch& match)
{
	CMG_LOG_DEBUG() << "Loading word task: " << match.key;

	// Load each unloaded part
	for (auto& variant : match.variants)
	{
		for (auto& part : variant.parts)
		{
			if (!part.isLoaded)
			{
				bool needsDownload = false;
				part.term = m_wiktionary.GetTerm(part.text, true);
				part.isLoaded = true;
				part.SetWord(match.key.type);
			}
		}
	}
}
