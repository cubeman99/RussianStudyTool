#pragma once

#include <cmgCore/cmg_core.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <string>
#include <regex>
#include <filesystem>

using unichar = uint16_t;
using unistr = StringU16;
using uniregex = std::basic_regex<char16_t>;

std::string ConvertToUTF8(std::u16string str);
unistr ConvertFromUTF8(const char* data);
unistr ConvertFromUTF8(const std::string& data);
std::ostream& operator <<(std::ostream &out, const unistr& text);
