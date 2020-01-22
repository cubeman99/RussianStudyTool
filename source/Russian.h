#pragma once

#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include <codecvt>
#include <locale>
#include <iostream>
#include <string>
#include "Macros.h"

using unichar = uint16_t;
using unistr = std::u16string;

std::string ConvertToUTF8(std::u16string str);
unistr ConvertFromUTF8(const char* data);
unistr ConvertFromUTF8(const std::string& data);

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

};

#define ENUM_MACRO_LANGUAGE(_context, _ex) \
	_ex(_context, russian, = 0, ru) \
	_ex(_context, english, = 1, en)
DECLARE_ENUM_WITH_COUNT(Language, ENUM_MACRO_LANGUAGE)

#define ENUM_MACRO_WORD_TYPE(_context, _ex) \
	_ex(_context, noun, , n) \
	_ex(_context, verb, , v) \
	_ex(_context, adjective, , adj) \
	_ex(_context, preposition, , prep) \
	_ex(_context, adverb, , adv) \
	_ex(_context, conjunction, , cj) \
	_ex(_context, pronoun, , pro) \
	_ex(_context, other, , oth) \
	_ex(_context, phrase, , ph) \
	_ex(_context, interjection, , int) \
	_ex(_context, particle, , part) \
	_ex(_context, numeral, , num)
DECLARE_ENUM_WITH_COUNT(WordType, ENUM_MACRO_WORD_TYPE)

#define ENUM_MACRO_GENDER(_context, _ex) \
	_ex(_context, masculine, , m) \
	_ex(_context, femanine, , f) \
	_ex(_context, neuter, , n)
DECLARE_ENUM_WITH_COUNT(Gender, ENUM_MACRO_GENDER)

#define ENUM_MACRO_PLURALITY(_context, _ex) \
	_ex(_context, singular, , sing) \
	_ex(_context, plural, , pl)
DECLARE_ENUM_WITH_COUNT(Plurality, ENUM_MACRO_PLURALITY)

#define ENUM_MACRO_PARTICIPLE(_context, _ex) \
	_ex(_context, active, , m) \
	_ex(_context, passive, , f) \
	_ex(_context, adverbial, , pl)
DECLARE_ENUM_WITH_COUNT(Participle, ENUM_MACRO_PARTICIPLE)

#define ENUM_MACRO_PERSON(_context, _ex) \
	_ex(_context, first, , 1st) \
	_ex(_context, second, , 2nd) \
	_ex(_context, third, , 3rd)
DECLARE_ENUM_WITH_COUNT(Person, ENUM_MACRO_PERSON)

#define ENUM_MACRO_CASE(_context, _ex) \
	_ex(_context, nominative, , nom) \
	_ex(_context, accusative, , acc) \
	_ex(_context, genetive, , gen) \
	_ex(_context, dative, , dat) \
	_ex(_context, instrumental, , instr) \
	_ex(_context, prepositional, , prep)
DECLARE_ENUM_WITH_COUNT(Case, ENUM_MACRO_CASE)

#define ENUM_MACRO_ASPECT(_context, _ex) \
	_ex(_context, imperfective, , impf) \
	_ex(_context, perfective, , pf)
DECLARE_ENUM_WITH_COUNT(Aspect, ENUM_MACRO_ASPECT)

#define ENUM_MACRO_TENSE(_context, _ex) \
	_ex(_context, present, , pres) \
	_ex(_context, past, , past) \
	_ex(_context, future, , fut)
DECLARE_ENUM_WITH_COUNT(Tense, ENUM_MACRO_TENSE)

#define ENUM_MACRO_ANIMACY(_context, _ex) \
	_ex(_context, animate, , anim) \
	_ex(_context, inanimate, , inanim)
DECLARE_ENUM_WITH_COUNT(Animacy, ENUM_MACRO_ANIMACY)

#define ENUM_MACRO_CARD_SET_TYPE(_context, _ex) \
	_ex(_context, other, , oth) \
	_ex(_context, categorical, , categ) \
	_ex(_context, grammatical, , gram) \
	_ex(_context, media, , media)
DECLARE_ENUM_WITH_COUNT(CardSetType, ENUM_MACRO_CARD_SET_TYPE)
