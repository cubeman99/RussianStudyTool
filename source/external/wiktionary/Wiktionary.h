#pragma once

#include "cards/CardSetPackage.h"
#include "rapidjson/document.h"
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "gui/Events.h"
#include "external/wiktionary/Term.h"
#include "external/wiktionary/Parser.h"
#include "external/RequestInterface.h"
#include <cmgInput\cmgJoystick.h>

namespace wiki
{

class Wiktionary
{
public:
	Wiktionary(RequestInterface& requests);
	~Wiktionary();

	const Path& GetDataPath() const;
	Term::sptr GetTerm(const unistr& text);
	Term::sptr DownloadTerm(const unistr& text);

	void SetDataPath(const Path& path);
	Error Load();
	Error Load(const Path& path);
	Error Save();
	Error Save(const Path& path);

	static unistr GetTermURL(Term::sptr term, bool russianSection);

private:
	Wiktionary(const Wiktionary& copy) = delete;
	Wiktionary& operator=(const Wiktionary& copy) = delete;

private:
	Path m_savePath;
	Path m_soundsDir;
	Map<unistr, Term::sptr> m_terms;
	Parser m_parser;

	Set<unistr> m_errorTerms;
	Set<unistr> m_404Terms;
	Set<unistr> m_noWordTerms;

	std::recursive_mutex m_mutex;
};


} // namespace wiki
