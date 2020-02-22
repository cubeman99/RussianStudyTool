#pragma once

#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include "core/Unicode.h"

class AccentedText
{
public:
	static const unichar ACCENT_OUTPUT_CHAR;
	static const unichar ACCENT_RENDER_CHAR;

public:
	AccentedText();
	AccentedText(const char* str);
	AccentedText(const String& str);
	AccentedText(const char16_t* str);
	AccentedText(const unichar* str);
	AccentedText(const unistr& str);
	AccentedText(const std::wstring& str);
	//AccentedText(const char* str);
	//AccentedText(const String& str);
	
	AccentedText& operator+=(const AccentedText& other);
	AccentedText& operator+=(const unistr& str);
	AccentedText operator+(const AccentedText& other) const;
	AccentedText operator+(const unistr& str) const;
	AccentedText operator+(const char* str) const;
	AccentedText operator+(const unichar* str) const;
	unichar operator [](uint32 index) const;
	bool operator==(const AccentedText& other) const;
	bool operator!=(const AccentedText& other) const;
	bool operator<(const AccentedText& other) const;
	bool operator>(const AccentedText& other) const;

	bool empty() const;
	size_t length() const;
	AccentedText substr(size_t offset = 0, size_t count = unistr::npos) const;
	bool HasAccentAt(uint32 index) const;
	const unistr& GetString() const;
	uint32 GetAccentCount() const;
	uint32 GetAccentIndex(uint32 i) const;
	unistr ToMarkedString() const;

	void AddChar(unichar c, bool accent = false);

	std::ostream& StreamOut(std::ostream &out) const;

	static AccentedText FromUTF8(const char* data);

private:
	unistr m_string;
	Array<uint32> m_accents;
};

std::ostream& operator <<(std::ostream &out, const AccentedText& text);
AccentedText operator +(const String& left, const AccentedText& right);
AccentedText operator +(const unistr& left, const AccentedText& right);

