#pragma once

#include "cards/CardSetPackage.h"
#include "rapidjson/document.h"
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "gui/Events.h"
#include "external/wiktionary/Term.h"

namespace wiki
{

class Wiktionary
{
public:
	Wiktionary();
	~Wiktionary();

	Term::sptr GetTerm(const unistr& text);

	Error Load(const Path& path);
	Error Save();
	Error Save(const Path& path);

private:
	Wiktionary(const Wiktionary& copy) = delete;
	Wiktionary& operator=(const Wiktionary& copy) = delete;

private:
	Path m_savePath;
	Path m_soundsDir;
	Map<unistr, Term::sptr> m_terms;

	Set<unistr> m_errorTerms;
	Set<unistr> m_404Terms;
	Set<unistr> m_noWordTerms;

	std::recursive_mutex m_mutex;
};


} // namespace wiki
