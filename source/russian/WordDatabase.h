#pragma once

#include "cards/CardSetPackage.h"
#include "rapidjson/document.h"
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "gui/Events.h"
#include "external/wiktionary/Wiktionary.h"


class WordDatabase
{
public:
	WordDatabase(wiki::Wiktionary& wiktionary);
	~WordDatabase();

	wiki::Wiktionary& GetWiktionary();
	bool GetWordFromCard(Card::sptr card, wiki::Term::sptr& outTerm, wiki::Word::sptr& outWord);

private:
	WordDatabase(const WordDatabase& copy) = delete;
	WordDatabase& operator=(const WordDatabase& copy) = delete;

private:
	wiki::Wiktionary& m_wiktionary;
};
