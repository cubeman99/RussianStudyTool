#include "Russian.h"
#include <Windows.h>

namespace
{
	const unistr VOWELS = u"аэыуояеёюи";
	const unistr CONSONANTS = u"бвгджзклмнпрстфхцчшщй";
	const unistr HARD_VOWELS = u"аоуыэ";
	const unistr SOFT_VOWELS = u"яёюие";
	std::map<unichar, unichar> keyRussianToEnglish = {
		{u'й', u'q'},
		{u'ц', u'w'},
		{u'у', u'e'},
		{u'к', u'r'},
		{u'е', u't'},
		{u'н', u'y'},
		{u'г', u'u'},
		{u'ш', u'i'},
		{u'щ', u'o'},
		{u'з', u'p'},
		{u'х', u'['},
		{u'ъ', u']'},
		{u'ф', u'a'},
		{u'ы', u's'},
		{u'в', u'd'},
		{u'а', u'f'},
		{u'п', u'g'},
		{u'р', u'h'},
		{u'о', u'j'},
		{u'л', u'k'},
		{u'д', u'l'},
		{u'ж', u';'},
		{u'э', u'\''},
		{u'я', u'z'},
		{u'ч', u'x'},
		{u'с', u'c'},
		{u'м', u'v'},
		{u'и', u'b'},
		{u'т', u'n'},
		{u'ь', u'm'},
		{u'б', u','},
		{u'Б', u'<'},
		{u'ю', u','},
		{u'Ю', u'>'},
		{u'.', u'/'},
		{u',', u'?'},
		{u'"', u'@'},
		{u'№', u'#'},
		{u';', u'$'},
		{u'%', u'%'},
		{u':', u'^'},
		{u'?', u'&'},
	};

	std::map<unichar, unichar> keyEnglishToRussian = {
		{u'q', u'й'},
		{u'w', u'ц'},
		{u'e', u'у'},
		{u'r', u'к'},
		{u't', u'е'},
		{u'y', u'н'},
		{u'u', u'г'},
		{u'i', u'ш'},
		{u'o', u'щ'},
		{u'p', u'з'},
		{u'[', u'х'},
		{u']', u'ъ'},
		{u'a', u'ф'},
		{u's', u'ы'},
		{u'd', u'в'},
		{u'f', u'а'},
		{u'g', u'п'},
		{u'h', u'р'},
		{u'j', u'о'},
		{u'k', u'л'},
		{u'l', u'д'},
		{u';', u'ж'},
		{u'\'', u'э'},
		{u'z', u'я'},
		{u'x', u'ч'},
		{u'c', u'с'},
		{u'v', u'м'},
		{u'b', u'и'},
		{u'n', u'т'},
		{u'm', u'ь'},
		{u',', u'б'},
		{u'.', u'ю'},
		{u'`', u'ё'},
	};
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


namespace ru {

bool StringMatch(const unistr& left, const unistr& right)
{
	uint32 lengthLeft = left.length();
	uint32 lengthRight = right.length();
	if (lengthLeft != lengthRight)
		return false;
	for (uint32 i = 0; i < lengthRight; i++)
	{
		unichar a = ru::ToLowerChar(left[i]);
		unichar b = ru::ToLowerChar(right[i]);
		if (a == u'ё')
			a = u'е';
		if (b == u'ё')
			b = u'е';
		if (a != b)
			return false;
	}
	return true;
}

bool IsRussian(unichar c)
{
	return ((c >= u'а' && c <= u'я') || (c >= u'А' && c <= u'Я') ||
		c == u'ё' || c == u'Ё');
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
	if (c == u'Ё')
		return u'ё';
	return (unichar) ::tolower((int) c);
}

unichar ToUpperChar(unichar c)
{
	if (c >= u'а' && c <= u'я')
		return c - 32;
	if (c == u'ё')
		return u'Ё';
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

unistr ToRussianKeyboard(const unistr& str)
{
	unistr result = str;
	uint32 length = result.length();
	for (uint32 i = 0; i < length; i++)
	{
		unichar c = result[i];
		auto it = keyEnglishToRussian.find(c);
		if (it != keyEnglishToRussian.end())
			result[i] = it->second;
		else
		{
			it = keyEnglishToRussian.find(ru::ToLowerChar(c));
			if (it != keyEnglishToRussian.end())
				result[i] = it->second;
		}
	}
	return result;
}

unistr ToEnglishKeyboard(const unistr& str)
{
	unistr result = str;
	uint32 length = result.length();
	for (uint32 i = 0; i < length; i++)
	{
		unichar c = result[i];
		auto it = keyRussianToEnglish.find(c);
		if (it != keyRussianToEnglish.end())
			result[i] = it->second;
		else
		{
			it = keyRussianToEnglish.find(ru::ToLowerChar(c));
			if (it != keyRussianToEnglish.end())
				result[i] = it->second;
		}
	}
	return result;
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
		pair({WordType::k_verb, {u"ать", u"ить", u"еть", u"ять", u"уть", u"ыть", u"ться", u"ечь", u"сти", u"ись"}}),
		pair({WordType::k_noun, {u"ство", u"ие", u"ость",
			u"а", u"н", u"к", u"ц", u"г", u"з", u"р", u"ль", u"ф",
			u"з", u"т", u"д", u"б", u"ж", u"п", u"х"}}),
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
