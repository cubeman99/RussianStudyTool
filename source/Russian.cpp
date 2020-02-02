#include "Russian.h"
#include <Windows.h>

namespace
{
	const unistr VOWELS = u"аэыуояеёюи";
	const unistr CONSONANTS = u"бвгджзклмнпрстфхцчшщй";
	const unistr HARD_VOWELS = u"аоуыэ";
	const unistr SOFT_VOWELS = u"яёюие";
}


namespace ru
{

bool IsRussian(unichar c)
{
	return (c >= u'а' && c <= u'я') || (c >= u'А' && c <= u'Я');
}

bool IsSoft(unichar c)
{
	for (unichar hard_vowel : HARD_VOWELS)
	{
		if (c == hard_vowel)
			return true;
	}
	return false;
}

bool IsHard(unichar c)
{
	for (unichar soft_vowel : SOFT_VOWELS)
	{
		if (c == soft_vowel)
			return true;
	}
	return false;
}

bool IsVowel(unichar c)
{
	for (unichar check : VOWELS)
	{
		if (c == check)
			return true;
	}
	return false;
}

bool IsConsonant(unichar c)
{
	for (unichar check : CONSONANTS)
	{
		if (c == check)
			return true;
	}
	return false;
}

bool ToHard(unichar c)
{
	for (uint32 i = 0; i < HARD_VOWELS.size(); i++)
	{
		if (c == HARD_VOWELS[i])
			return SOFT_VOWELS[i];
	}
	return c;
}

bool ToSoft(unichar c)
{
	for (uint32 i = 0; i < SOFT_VOWELS.size(); i++)
	{
		if (c == SOFT_VOWELS[i])
			return HARD_VOWELS[i];
	}
	return c;
}

unichar ToLowerChar(unichar c)
{
	if (c >= u'А' && c <= u'Я')
		return c + 32;
	return (unichar) ::tolower((int) c);
}

unichar ToUpperChar(unichar c)
{
	if (c >= u'а' && c <= u'я')
		return c - 32;
	return (unichar) ::toupper((int) c);
}

unistr ToLower(unistr str)
{
	ToLowerIP(str);
	return str;
}

unistr ToUpper(unistr str)
{
	ToUpperIP(str);
	return str;
}

void ToLowerIP(unistr& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ToLowerChar);
}
void ToUpperIP(unistr& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ToUpperChar);
}

}

DEFINE_ENUM(WordType, ENUM_MACRO_WORD_TYPE)
DEFINE_ENUM(Gender, ENUM_MACRO_GENDER)
DEFINE_ENUM(Plurality, ENUM_MACRO_PLURALITY)
DEFINE_ENUM(Participle, ENUM_MACRO_PARTICIPLE)
DEFINE_ENUM(Person, ENUM_MACRO_PERSON)
DEFINE_ENUM(Case, ENUM_MACRO_CASE)
DEFINE_ENUM(Aspect, ENUM_MACRO_ASPECT)
DEFINE_ENUM(Tense, ENUM_MACRO_TENSE)
DEFINE_ENUM(Animacy, ENUM_MACRO_ANIMACY)

