#pragma once

#include "core/Unicode.h"
#include "Macros.h"

class Card;
class CardSet;
class CardSetPackage;
class StudySet;
class RussianStudyToolApp;

using AppTimestamp = double;


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

// Includes 'plural' as a gender to make declension easier
#define ENUM_MACRO_GENDER(_context, _ex) \
	_ex(_context, masculine, , m) \
	_ex(_context, feminine, , f) \
	_ex(_context, neuter, , n) \
	_ex(_context, plural, , pl)
DECLARE_ENUM_WITH_COUNT(Gender, ENUM_MACRO_GENDER)

#define ENUM_MACRO_PLURALITY(_context, _ex) \
	_ex(_context, singular, , sing) \
	_ex(_context, plural, , pl)
DECLARE_ENUM_WITH_COUNT(Plurality, ENUM_MACRO_PLURALITY)

#define ENUM_MACRO_PARTICIPLE(_context, _ex) \
	_ex(_context, active, , act) \
	_ex(_context, passive, , pass) \
	_ex(_context, adverbial, , adv)
DECLARE_ENUM_WITH_COUNT(Participle, ENUM_MACRO_PARTICIPLE)

#define ENUM_MACRO_PERSON(_context, _ex) \
	_ex(_context, first, , 1st) \
	_ex(_context, second, , 2nd) \
	_ex(_context, third, , 3rd)
DECLARE_ENUM_WITH_COUNT(Person, ENUM_MACRO_PERSON)

#define ENUM_MACRO_CASE(_context, _ex) \
	_ex(_context, nominative, , nom) \
	_ex(_context, accusative, , acc) \
	_ex(_context, genitive, , gen) \
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

#define ENUM_MACRO_PROFICIENCY_LEVEL(_context, _ex) \
	_ex(_context, new, , new) \
	_ex(_context, hard, , hard) \
	_ex(_context, medium, , medium) \
	_ex(_context, easy, , easy) \
	_ex(_context, learned, , learned)
DECLARE_ENUM_WITH_COUNT(ProficiencyLevel, ENUM_MACRO_PROFICIENCY_LEVEL)


// Values 0 to 31 are key tags, which differentiate
// cards with the same english text and word type.
#define ENUM_MACRO_CARD_TAG(_context, _ex) \
	_ex(_context, perfective, = 0    , pf) \
	_ex(_context, imperfective,      , impf) \
	_ex(_context, unidirectional,    , uni) \
	_ex(_context, multidirectional,  , multi) \
	_ex(_context, masculine, = 32     , m) \
	_ex(_context, feminine,          , f) \
	_ex(_context, neuter,            , n) \
	_ex(_context, singular,          , sing) \
	_ex(_context, plural,            , pl) \
	_ex(_context, nopural,           , nopl) \
	_ex(_context, plural_only,       , pluronly) \
	_ex(_context, singular_only,     , singonly) \
	_ex(_context, first_person,      , 1st) \
	_ex(_context, second_person,     , 2nd) \
	_ex(_context, third_person,      , 3rd) \
	_ex(_context, informal,          , informal) \
	_ex(_context, formal,            , formal) \
	_ex(_context, nominative,        , nom) \
	_ex(_context, accusative,        , acc) \
	_ex(_context, genitive,          , gen) \
	_ex(_context, dative,            , dat) \
	_ex(_context, prepositional,     , prep) \
	_ex(_context, instrumental,      , instr) \
	_ex(_context, indeclinable,      , indec) \
	_ex(_context, irregular,         , irreg) \
	_ex(_context, animate,           , anim) \
	_ex(_context, inanimate,         , inanim) \
	_ex(_context, transative,        , trans) \
	_ex(_context, intransative,      , intrans) \
	_ex(_context, reflexive,         , reflex) \
	_ex(_context, short,             , short) \
	_ex(_context, long,              , long) \
	_ex(_context, infinitive,        , infinitive) \
	_ex(_context, verb_suffix_ai,    , verb.suffix.ai  ) \
	_ex(_context, verb_suffix_ei,    , verb.suffix.ei  ) \
	_ex(_context, verb_suffix_ova,   , verb.suffix.ova ) \
	_ex(_context, verb_suffix_nu,    , verb.suffix.nu  ) \
	_ex(_context, verb_suffix_nu2,   , verb.suffix.nu2 ) \
	_ex(_context, verb_suffix_a1,    , verb.suffix.a1  ) \
	_ex(_context, verb_suffix_a2,    , verb.suffix.a2  ) \
	_ex(_context, verb_suffix_a3,    , verb.suffix.a3  ) \
	_ex(_context, verb_suffix_avai,  , verb.suffix.avai) \
	_ex(_context, verb_suffix_o,     , verb.suffix.o   ) \
	_ex(_context, verb_suffix_i,     , verb.suffix.i   ) \
	_ex(_context, verb_suffix_e,     , verb.suffix.e   ) \
	_ex(_context, verb_suffix_zha,   , verb.suffix.zha ) \
	_ex(_context, resonant_stem,     , stem.resonant   ) \
	_ex(_context, obstruent_stem,    , stem.obstruent  ) \
	_ex(_context, past,              , past) \
	_ex(_context, non_past,          , nonpast) \
	_ex(_context, adjective_as_noun, , adjasnoun) \
	_ex(_context, verb_of_motion,    , verb_of_motion) \
	_ex(_context, curse,             , curse) \
	_ex(_context, idiom,             , idiom)
DECLARE_ENUM_WITH_COUNT(CardTags, ENUM_MACRO_CARD_TAG)


AppTimestamp GetAppTimestamp();
bool IsKeyCardTag(CardTags tag);

