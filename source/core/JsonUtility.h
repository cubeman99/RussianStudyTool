#pragma once

#include "Unicode.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"
#include "AccentedText.h"

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

class Object
{
public:
	Object(rapidjson::Value& data, rapidjson::Document::AllocatorType& allocator);

	template <typename T_Value>
	void AddMember(const char* key, const T_Value& value)
	{
		m_data.AddMember(rapidjson::Value::StringRefType(key),
			rapidjson::Value(str.c_str(), allocator).Move(), allocator);
	}

	template <typename T_Value>
	void AddMember(const String& key, const T_Value& value)
	{
		m_data.AddMember(
			rapidjson::Value(key.c_str(), allocator).Move(),
			rapidjson::Value(str.c_str(), allocator).Move(), 
			allocator);
	}

	template <typename T_Value>
	void PushBack(const char* key, T_Value value);

private:
	rapidjson::Value& m_data;
	rapidjson::Document::AllocatorType& m_allocator;
};

//template <template T_Value>
//void Serialize(rapidjson::Value& data, const String& key, T_Value value)
//{
//	Serialize
//}

using Value = rapidjson::Value;
using Allocator = rapidjson::Document::AllocatorType;

void Serialize(Value& data, const char* key, const AccentedText& text, Allocator& allocator);
void Deserialize(Value& data, const char* key, AccentedText& outValue);


template <class T_Key, typename T_Value>
void AddMember(Value& data, const T_Key& key, const T_Value& value, Allocator & allocator)
{
	data.AddMember(Val(key, allocator).Move(),
		Val(value, allocator).Move(), allocator);
}

template <typename T_Value>
void AddMember(Value& data, const char * key, const T_Value& value, Allocator & allocator)
{
	data.AddMember(Value::StringRefType(key),
		Val(value, allocator).Move(), allocator);
}

template <typename T_Value>
void PushBack(Value& data, const T_Value& value, Allocator & allocator)
{
	data.PushBack(Val(value, allocator).Move(), allocator);
}

Value::StringRefType Val(const char* text, Allocator& allocator);
Value Val(const String& str, Allocator& allocator);
Value Val(const AccentedText& text, Allocator& allocator);
Value& Val(const Value& value, Allocator& allocator);

template <class T_Enum>
Value Val(const T_Enum& value, Allocator& allocator)
{
	String name = EnumToString<T_Enum>(value);
	return Value(name.c_str(), allocator);
}


template <class T_Enum>
void SerializeEnum(Value& data, const char* key, const T_Enum& value,
	Allocator& allocator)
{
	String name = EnumToString<T_Enum>(value);
	data.AddMember(Value::StringRefType(key),
		Value(name.c_str(), allocator).Move(), allocator);
}

template <class T_Enum>
void DeserializeEnum(Value& data, const char* key, T_Enum& outValue)
{
	if (data.HasMember(key))
		outValue = StringToEnum<T_Enum>(data[key].GetString());
}

template <class T_Enum>
void DeserializeShortEnum(Value& data, const char* key, T_Enum& outValue)
{
	if (data.HasMember(key))
		outValue = StringToEnum<T_Enum>(data[key].GetString(), true);
}

void DeserializeBool(rapidjson::Value& data, const char* key, bool& outValue);

};