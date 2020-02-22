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

private:
	WordDatabase(const WordDatabase& copy) = delete;
	WordDatabase& operator=(const WordDatabase& copy) = delete;

private:
	wiki::Wiktionary& m_wiktionary;
};
