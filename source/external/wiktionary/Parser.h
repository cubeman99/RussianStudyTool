#pragma once

#include "cards/CardSetPackage.h"
#include "rapidjson/document.h"
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "gui/Events.h"
#include "external/wiktionary/Term.h"
#include "external/RequestInterface.h"

namespace wiki
{

class Parser
{
public:
	Parser(RequestInterface& requests);
	~Parser();

	Term::sptr DownloadTerm(const unistr& text);

private:
	RequestInterface& m_requests;
};


} // namespace wiki
