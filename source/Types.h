#pragma once

#include "Russian.h"

class Card;
class CardSet;
class CardSetPackage;
class StudySet;
class RussianStudyToolApp;

using AppTimestamp = double;


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
	_ex(_context, femanine,          , f) \
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
	_ex(_context, genetive,          , gen) \
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

