#include "WordDatabase.h"

WordDatabase::WordDatabase(wiki::Wiktionary& wiktionary) :
	m_wiktionary(wiktionary)
{
}

WordDatabase::~WordDatabase()
{
}

wiki::Wiktionary& WordDatabase::GetWiktionary()
{
	return m_wiktionary;
}
