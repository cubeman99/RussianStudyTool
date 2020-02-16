#include "Russian.h"
#include <Windows.h>

namespace
{
	const unistr VOWELS = u"аэыуояеёюи";
	const unistr CONSONANTS = u"бвгджзклмнпрстфхцчшщй";
	const unistr HARD_VOWELS = u"аоуыэ";
	const unistr SOFT_VOWELS = u"яёюие";
}


TranslationPair::TranslationPair()
{
}

TranslationPair::TranslationPair(
	const AccentedText& russian, const AccentedText& english) :
	russian(russian),
	english(english)
{
}

const AccentedText& TranslationPair::GetText(Language language) const
{
	if (language == Language::k_english)
		return english;
	else
		return russian;
}

bool TranslationPair::operator==(const TranslationPair& other) const
{
	return (english == other.english && russian == other.russian);
}

bool TranslationPair::operator!=(const TranslationPair& other) const
{
	return !(*this == other);
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

bool TryPredictWordType(const unistr& text, WordType& outWordType)
{
	if (text.find(u' ') != unistr::npos)
	{
		outWordType = WordType::k_phrase;
		return true;
	}
	using pair = std::pair<WordType, Array<unistr>>;
	for (const pair& pair : {
		pair({WordType::k_adjective, {u"ый", u"ой", u"ий"}}),
		pair({WordType::k_verb, {u"ть", u"ться", u"ечь", u"сти", u"иьс"}}),
		pair({WordType::k_noun, {u"ство", u"ие", u"ость",
			u"а", u"н", u"к", u"ц", u"г", u"з", u"р", u"ль", u"ф",
			u"з", u"т", u"д", u"ь", u"б", u"ж", u"п", u"х"}}),
		pair({WordType::k_adverb, {u"о"}}),
	})
	{
		for (const unistr& ending : pair.second) 
		{
			if (cmg::string::EndsWith(text, ending))
			{
				outWordType = pair.first;
				return true;
			}
		}
	}
	return false;
}

} // namespace ru
