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
	void GetTerm(const unistr& text, Term::sptr& outTerm, bool& needsDownload);
	Term::sptr GetTerm(const unistr& text, bool download = false);
	EventSignal<Term::sptr>& TermLoaded() { return m_eventTermLoaded; }
	Map<unistr, Term::sptr>& GetAllTerms();

	Term::sptr DownloadTerm(Term::sptr term);
	Term::sptr DownloadTerm(const unistr& text);
	void SetDataPath(const Path& path);
	Error Load();
	Error Load(const Path& path);
	Error Save();
	Error Save(const Path& path);
	void ProcessEvents();

	static unistr GetTermURL(Term::sptr term, bool russianSection);

	bool TermNeedsReDownload(wiki::Term::sptr term);
private:

	Wiktionary(const Wiktionary& copy) = delete;
	Wiktionary& operator=(const Wiktionary& copy) = delete;

private:
	Parser m_parser;

	Path m_savePath;
	Path m_soundsDir;
	Map<unistr, Term::sptr> m_terms;
	Set<unistr> m_errorTerms;
	Set<unistr> m_404Terms;
	Set<unistr> m_noWordTerms;

	EventSignal<Term::sptr> m_eventTermLoaded;
	Array<Term::sptr> m_loadedTermsQueue;

	std::recursive_mutex m_mutex;
	std::mutex m_mutexLoadedTermsQueue;
};


} // namespace wiki
