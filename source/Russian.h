#pragma once

#include "Types.h"
#include "AccentedText.h"


struct TranslationPair
{
public:
	AccentedText russian;
	AccentedText english;

	TranslationPair();
	TranslationPair(const AccentedText& russian, const AccentedText& english);

	const AccentedText& GetText(Language language) const;
	bool operator==(const TranslationPair& other) const;
	bool operator!=(const TranslationPair& other) const;
};


namespace ru
{

bool IsRussian(unichar c);
bool IsSoft(unichar c);
bool IsHard(unichar c);
bool IsVowel(unichar c);
bool IsConsonant(unichar c);
bool ToHard(unichar c);
bool ToSoft(unichar c);
unichar ToLowerChar(unichar c);
unichar ToUpperChar(unichar c);
unistr ToLower(unistr str);
unistr ToUpper(unistr str);
void ToLowerIP(unistr& str);
void ToUpperIP(unistr& str);

bool TryPredictWordType(const unistr& text, WordType& outWordType);

} // namespace ru

