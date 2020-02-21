#include "Unicode.h"
#include <Windows.h>


std::string ConvertToUTF8(std::u16string str)
{
	int sizeNeeded = WideCharToMultiByte(
		CP_UTF8, 0, (const wchar_t*) &str[0], str.size(), NULL, 0, NULL, NULL);
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*) str.c_str(),
		str.length(), &result[0], sizeNeeded, NULL, NULL);
	return result;
}

unistr ConvertFromUTF8(const char* data)
{
	int numChars = MultiByteToWideChar(CP_UTF8, 0, data, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[numChars];
	MultiByteToWideChar(CP_UTF8, 0, data, -1, wstr, numChars);
	unistr result((const char16_t*) wstr);
	delete[] wstr;
	return result;
}

unistr ConvertFromUTF8(const std::string& str)
{
	return ConvertFromUTF8(str.data());
}

std::ostream& operator<<(std::ostream& out, const unistr& text)
{
	return (out << ConvertToUTF8(text));
}
