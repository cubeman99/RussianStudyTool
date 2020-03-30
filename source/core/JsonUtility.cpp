#include "JsonUtility.h"
#include <fstream>

namespace json
{

Error LoadDocumentFromFile(const Path& path,
	rapidjson::Document& outDocument)
{
	String str;
	rapidjson::Document document;
	Error openError = File::OpenAndGetContents(path, str);
	if (openError.Failed())
		return openError.Check();
	return ParseDocument(outDocument, str.c_str());
}

Error LoadDocumentFromFile(const PathU16& path,
	rapidjson::Document& outDocument)
{
	rapidjson::Document document;
	std::ifstream file((const wchar_t*) path.c_str());
	std::string str((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	return ParseDocument(outDocument, str.c_str());
}

Error ParseDocument(rapidjson::Document& outDocument, const char* str)
{
	outDocument.Parse(str);
	if (outDocument.HasParseError())
	{
		rapidjson::ParseResult result = (rapidjson::ParseResult) outDocument;
		CMG_LOG_ERROR() << "JSON Parse error!";
		return CMG_ERROR(Error::kFileCorrupt);
	}
	return CMG_ERROR_SUCCESS;
}

Error SaveDocumentToFile(const Path& path,
	const rapidjson::Document& document)
{
	FILE* file = nullptr;
	errno_t fopenError = fopen_s(&file, path.c_str(), "wb");
	if (fopenError != 0)
		return CMG_ERROR_FROM_ERRNO(fopenError);
	Error error = SaveDocumentToFile(file, document);
	fclose(file);
	return error;
}

Error SaveDocumentToFile(const PathU16& path,
	const rapidjson::Document& document)
{
	FILE* file = nullptr;
	errno_t fopenError = _wfopen_s(&file, (const wchar_t*) path.c_str(), L"wb");
	if (fopenError != 0)
		return CMG_ERROR_FROM_ERRNO(fopenError);
	Error error = SaveDocumentToFile(file, document);
	fclose(file);
	return error;
}

Error SaveDocumentToFile(FILE* file, const rapidjson::Document& document)
{
	char writeBuffer[65536];
	rapidjson::FileWriteStream os(file, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	writer.SetIndent('\t', 1);
	document.Accept(writer);
	return CMG_ERROR_SUCCESS;
}

void Serialize(Value& data, const char * key,
	const AccentedText & text, Allocator& allocator)
{
	String str = ConvertToUTF8(text.ToMarkedString());
	data.AddMember(Value::StringRefType(key),
		Value(str.c_str(), allocator).Move(), allocator);
}

void Deserialize(Value & data, const char * key, AccentedText & outValue)
{
	outValue = data[key].GetString();
}

Value::StringRefType Val(const char * text, Allocator & allocator)
{
	return Value::StringRefType(text);
}

Value Val(const String & str, Allocator & allocator)
{
	return rapidjson::Value(str.c_str(), allocator);
}

Value Val(const AccentedText & text, Allocator& allocator)
{
	String str = ConvertToUTF8(text.ToMarkedString());
	return rapidjson::Value(str.c_str(), allocator);
}

Value& Val(const Value& value, Allocator & allocator)
{
	return (Value&) value;
}

void DeserializeBool(Value & data, const char * key, bool & outValue)
{
	if (data.HasMember(key))
		outValue = data[key].GetBool();
}

} // namespace json
