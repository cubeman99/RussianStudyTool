#pragma once

#include "Unicode.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"

namespace json
{

Error LoadDocumentFromFile(const Path& path,
	rapidjson::Document& outDocument);
Error LoadDocumentFromFile(const PathU16& path,
	rapidjson::Document& outDocument);
Error ParseDocument(rapidjson::Document& outDocument, const char* str);


Error SaveDocumentToFile(const Path& path,
	const rapidjson::Document& document);
Error SaveDocumentToFile(const PathU16& path,
	const rapidjson::Document& document);
Error SaveDocumentToFile(FILE* file, const rapidjson::Document& document);

};