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
	_ex(_context, numeral, , num) \
	_ex(_context, suffix, , suf) \
	_ex(_context, prefix, , pref)
DECLARE_ENUM_WITH_COUNT(WordType, ENUM_MACRO_WORD_TYPE)

// Includes 'plural' as a gender to make declension easier
#define ENUM_MACRO_GENDER(_context, _ex) \
	_ex(_context, masculine, , m) \
	_ex(_context, feminine, , f) \
	_ex(_context, neuter, , n) \
	_ex(_context, plural, , pl) \
	_ex(_context, unknown, , unknown)
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

#define ENUM_MACRO_TRANSITIVITY(_context, _ex) \
	_ex(_context, transitive, , trans) \
	_ex(_context, intransitive, , intrans) \
	_ex(_context, reflexive, , reflex) \
	_ex(_context, unknown, , unknown)
DECLARE_ENUM_WITH_COUNT(Transitivity, ENUM_MACRO_TRANSITIVITY)

#define ENUM_MACRO_TENSE(_context, _ex) \
	_ex(_context, present, , pres) \
	_ex(_context, past, , past) \
	_ex(_context, future, , fut)
DECLARE_ENUM_WITH_COUNT(Tense, ENUM_MACRO_TENSE)

#define ENUM_MACRO_ANIMACY(_context, _ex) \
	_ex(_context, animate, , anim) \
	_ex(_context, inanimate, , inanim) \
	_ex(_context, unknown, , unknown)
DECLARE_ENUM_WITH_COUNT(Animacy, ENUM_MACRO_ANIMACY)

#define ENUM_MACRO_CARD_SET_TYPE(_context, _ex) \
	_ex(_context, other, , oth) \
	_ex(_context, categorical, , categ) \
	_ex(_context, grammatical, , gram) \
	_ex(_context, media, , media)
DECLARE_ENUM_WITH_COUNT(CardSetType, ENUM_MACRO_CARD_SET_TYPE)

// Values 0 to 31 are key tags, which differentiate
// cards with the same english text and word type.
#define ENUM_MACRO_CARD_TAG(_context, _ex) \
	_ex(_context, perfective, = 0    , pf) \
	_ex(_context, imperfective,      , impf) \
	_ex(_context, unidirectional,    , uni) \
	_ex(_context, multidirectional,  , multi) \
	_ex(_context, masculine, = 32    , m) \
	_ex(_context, feminine,          , f) \
	_ex(_context, neuter,            , n) \
	_ex(_context, singular,          , sing) \
	_ex(_context, plural,            , pl) \
	_ex(_context, past,              , past) \
	_ex(_context, infinitive,        , infinitive) \
	_ex(_context, non_past,          , nonpast) \
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
	_ex(_context, locative,          , loc) \
	_ex(_context, short,             , short) \
	_ex(_context, long,              , long) \
	_ex(_context, nopural,           , nopl) \
	_ex(_context, plural_only,       , pluronly) \
	_ex(_context, singular_only,     , singonly) \
	_ex(_context, indeclinable,      , indec) \
	_ex(_context, irregular,         , irreg) \
	_ex(_context, animate,           , anim) \
	_ex(_context, inanimate,         , inanim) \
	_ex(_context, transitive,        , trans) \
	_ex(_context, intransitive,      , intrans) \
	_ex(_context, reflexive,         , reflex) \
	_ex(_context, impersonal,        , impers) \
	_ex(_context, predicative,       , predic) \
	_ex(_context, participle,        , participle) \
	_ex(_context, adjective_as_noun, , adjasnoun) \
	_ex(_context, curse,             , curse) \
	_ex(_context, idiom,             , idiom) \
	_ex(_context, diminutive,        , dim) \
	_ex(_context, verb_class_1,      , verbclass1) \
	_ex(_context, verb_class_2,      , verbclass2) \
	_ex(_context, verb_class_3,      , verbclass3) \
	_ex(_context, verb_class_3o,     , verbclass3o) \
	_ex(_context, verb_class_4,      , verbclass4) \
	_ex(_context, verb_class_5,      , verbclass5) \
	_ex(_context, verb_class_5o,     , verbclass5o) \
	_ex(_context, verb_class_6,      , verbclass6) \
	_ex(_context, verb_class_6o,     , verbclass6o) \
	_ex(_context, verb_class_7,      , verbclass7) \
	_ex(_context, verb_class_8,      , verbclass8) \
	_ex(_context, verb_class_9,      , verbclass9) \
	_ex(_context, verb_class_10,     , verbclass10) \
	_ex(_context, verb_class_11,     , verbclass11) \
	_ex(_context, verb_class_12,     , verbclass12) \
	_ex(_context, verb_class_13,     , verbclass13) \
	_ex(_context, verb_class_14,     , verbclass14) \
	_ex(_context, verb_class_15,     , verbclass15) \
	_ex(_context, verb_class_16,     , verbclass16) \
	_ex(_context, stem_stressed,     , stemstress) \
	_ex(_context, ending_stressed,   , endstress) \
	_ex(_context, shifting_stress,   , shiftstress) \
	_ex(_context, verb_of_motion,    , verb_of_motion)
DECLARE_ENUM_WITH_COUNT(CardTags, ENUM_MACRO_CARD_TAG)

#define ENUM_MACRO_VERB_ACCENT_PATTERN(_context, _ex) \
	_ex(_context, stem_stressed,   , a) \
	_ex(_context, ending_stressed, , b) \
	_ex(_context, changing_stress, , c)
DECLARE_ENUM_WITH_COUNT(VerbAccentPattern, ENUM_MACRO_VERB_ACCENT_PATTERN)

AppTimestamp GetAppTimestamp();
bool IsKeyCardTag(CardTags tag);

