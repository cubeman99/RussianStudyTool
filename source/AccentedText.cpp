﻿#include "AccentedText.h"
#include "russian/Russian.h"


static const unichar ACCENT_INPUT_CHARS[] =
	{ u'\'', u'´', u'`', u'’', u'\u0301'};
const unichar AccentedText::ACCENT_OUTPUT_CHAR = u'\'';
const unichar AccentedText::ACCENT_RENDER_CHAR = u'´';


AccentedText::AccentedText()
{
}

AccentedText::AccentedText(const char* str) :
	AccentedText(ConvertFromUTF8(str))
{
}

AccentedText::AccentedText(const String& str) :
	AccentedText(str.c_str())
{
}

AccentedText::AccentedText(const char16_t* str) :
	AccentedText(unistr(str))
{
}

AccentedText::AccentedText(const unichar* str) :
	AccentedText(unistr((const char16_t*) str))
{
}

AccentedText::AccentedText(const unistr& str)
{
	m_string.clear();
	m_accents.clear();
	uint32 index = 0;
	bool isAccent = false;
	unichar prevChar = 0;
	for (unichar c : str)
	{
		isAccent = false;

		//if (ru::IsRussian(prevChar))
		{
			for (unichar accentChar : ACCENT_INPUT_CHARS)
			{
				if (c == accentChar)
				{
					isAccent = true;
					m_accents.push_back(index - 1);
					break;
				}
			}
		}

		if (!isAccent)
		{
			m_string += c;
			prevChar = c;
			index++;
		}
	}
}
AccentedText::AccentedText(const std::wstring & str) :
	AccentedText(unistr(str.begin(), str.end()))
{
}
/*
AccentedText::AccentedText(const String & str) :
	m_string(str)
{
}

AccentedText::AccentedText(const char * str) :
	m_string(str)
{
}*/

AccentedText& AccentedText::operator+=(const AccentedText& other)
{
	uint32 length = m_string.length();
	m_string += other.m_string;
	for (uint32 index : other.m_accents)
		m_accents.push_back(length + index);
	return *this;
}

AccentedText & AccentedText::operator+=(const unistr & str)
{
	return (*this += AccentedText(str));
}

AccentedText AccentedText::operator+(const AccentedText& other) const
{
	AccentedText result = *this;
	uint32 length = result.m_string.length();
	result.m_string += other.m_string;
	for (uint32 index : other.m_accents)
		result.m_accents.push_back(length + index);
	return result;
}

AccentedText AccentedText::operator+(const unistr & str) const
{
	return (*this + AccentedText(str));
}

AccentedText AccentedText::operator+(const char* str) const
{
	return (*this + AccentedText(str));
}

AccentedText AccentedText::operator+(const unichar* str) const
{
	return (*this + AccentedText(str));
}

unichar AccentedText::operator[](uint32 index) const
{
	return m_string[index];
}

bool AccentedText::operator==(const AccentedText& other) const
{
	return (m_string == other.m_string && m_accents == other.m_accents);
}

bool AccentedText::operator!=(const AccentedText& other) const
{
	return !(*this == other);
}

bool AccentedText::operator<(const AccentedText& other) const
{
	return m_string < other.m_string;
}

bool AccentedText::operator>(const AccentedText& other) const
{
	return m_string > other.m_string;
}

bool AccentedText::empty() const
{
	return m_string.empty();
}

size_t AccentedText::length() const
{
	return m_string.length();
}

AccentedText AccentedText::substr(size_t offset, size_t count) const
{
	AccentedText substring;
	substring.m_string = m_string.substr(offset, count);
	for (uint32 accentIndex : m_accents)
	{
		if (accentIndex >= offset && accentIndex < offset + count)
			substring.m_accents.push_back(accentIndex);
	}
	return substring;
}

bool AccentedText::HasAccentAt(uint32 index) const
{
	return cmg::container::Contains(m_accents, index);
}

const unistr& AccentedText::GetString() const
{
	return m_string;
}

uint32 AccentedText::GetAccentCount() const
{
	return m_accents.size();
}

uint32 AccentedText::GetAccentIndex(uint32 i) const
{
	return m_accents[i];
}

unistr AccentedText::ToMarkedString() const
{
	unistr str = m_string;
	for (uint32 i = 0; i < m_accents.size(); i++)
		str.insert(m_accents[i] + i + 1, 1, ACCENT_OUTPUT_CHAR);
	return str;
}

void AccentedText::AddChar(unichar c, bool accent)
{
	if (accent)
		m_accents.push_back(m_string.length());
	m_string += c;
}

std::ostream& AccentedText::StreamOut(std::ostream& out) const
{
	return (out << ConvertToUTF8(m_string));
	//std::wstring_convert<std::codecvt_utf8<unichar>, unichar> convert;
	//uint32 index = 0;
	//uint32 accentIndex = 0;
	//for (unichar c : m_string)
	//{
	//	if (accentIndex < m_accents.size() && index == m_accents[accentIndex])
	//	{
	//		out << convert.to_bytes(&AccentedText::ACCENT_OUTPUT_CHAR,
	//			&AccentedText::ACCENT_OUTPUT_CHAR + 1);
	//		accentIndex++;
	//	}
	//	out << convert.to_bytes(&c, &c + 1);
	//	index++;
	//}
	//
	//return out;
}

AccentedText AccentedText::FromUTF8(const char* data)
{
	return AccentedText(ConvertFromUTF8(data));
}

std::ostream& operator<<(std::ostream& out, const AccentedText& text)
{
	return text.StreamOut(out);
}

AccentedText operator+(const String& left, const AccentedText& right)
{
	return AccentedText(left) + right;
}


AccentedText operator+(const unistr& left, const AccentedText& right)
{
	return AccentedText(left) + right;
}
