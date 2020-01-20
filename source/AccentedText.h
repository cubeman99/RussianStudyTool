#pragma once

#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include "Russian.h"


class AccentedText
{
public:
	static const unichar ACCENT_INPUT_CHARS[5];
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
	
	AccentedText& operator +=(const AccentedText& other);
	AccentedText& operator +=(const unistr& str);
	AccentedText operator +(const AccentedText& other);
	AccentedText operator +(const unistr& str);
	unichar operator [](uint32 index) const;

	const unistr& GetString() const { return m_string; }
	uint32 GetAccentCount() const { return m_accents.size(); }
	uint32 GetAccentIndex(uint32 i) const { return m_accents[i]; }

	std::ostream& StreamOut(std::ostream &out) const;

	static AccentedText FromUTF8(const char* data);

private:
	unistr m_string;
	Array<uint32> m_accents;
};

std::ostream& operator <<(std::ostream &out, const AccentedText& text);


struct TranslationPair
{
public:
	AccentedText russian;
	AccentedText english;

	TranslationPair()
	{
	}

	TranslationPair(const AccentedText& russian, const AccentedText& english) :
		russian(russian),
		english(english)
	{
	}

	const AccentedText& GetText(Language language) const
	{
		if (language == Language::k_english)
			return english;
		else
			return russian;
	}
};

