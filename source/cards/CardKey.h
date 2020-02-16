#pragma once

#include "Russian.h"


struct CardEnKey
{
	WordType type;
	unistr english;
	uint32 keyTags = 0;

	CardEnKey();
	CardEnKey(WordType type, const unistr& english, uint32 keyTags);
	CardEnKey(WordType type, const AccentedText& english, const EnumFlags<CardTags>& tags);

	bool operator<(const CardEnKey& other) const;
	bool operator==(const CardEnKey& other) const;
	bool operator!=(const CardEnKey& other) const;
};

struct CardRuKey
{
	WordType type;
	unistr russian;

	CardRuKey();
	CardRuKey(WordType type, const unistr& russian);
	CardRuKey(WordType type, const AccentedText& russian);

	bool operator<(const CardRuKey& other) const;
	bool operator==(const CardRuKey& other) const;
	bool operator!=(const CardRuKey& other) const;
};

struct CardKey
{
	WordType type;
	unistr russian;
	unistr english;

	CardRuKey GetRuKey() const;
	bool operator<(const CardKey& other) const;
	bool operator==(const CardKey& other) const;
	bool operator!=(const CardKey& other) const;
};

std::ostream& operator <<(std::ostream &out, const CardKey& key);
std::ostream& operator <<(std::ostream &out, const CardEnKey& key);
std::ostream& operator <<(std::ostream &out, const CardRuKey& key);

