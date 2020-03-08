#include "Card.h"


Card::Card()
{
}

Card::Card(WordType type, AccentedText russian, AccentedText english) :
	m_type(type),
	m_text(russian, english)
{
}

Card::~Card()
{
}

const TranslationPair & Card::GetText() const
{
	return m_text;
}

const AccentedText & Card::GetText(Language language) const
{
	return m_text.GetText(language);
}

AppTimestamp Card::GetCreationTimestamp() const
{
	return m_creationTimestamp;
}

CardRuKey Card::GetRuKey() const
{
	return CardRuKey(m_type, m_text.russian);
}

CardEnKey Card::GetEnKey() const
{
	return CardEnKey(m_type, m_text.english, m_tags);
}

bool Card::HasRelatedCards(Card::sptr card) const
{
	return cmg::container::Contains(m_relatedCards, card);
}

Set<Card::sptr>& Card::GetRelatedCards()
{
	return m_relatedCards;
}

EnumFlags<CardTags>& Card::GetTags()
{
	return m_tags;
}

Array<unistr> Card::GetWordPatterns() const
{
	const unistr& str = m_text.russian.GetString();
	Array<unistr> patternList;
	uint32 length = str.length();
	unistr word;
	unistr pattern;
	uint32 count = 0;

	for (uint32 i = 0; i <= length; i++)
	{
		unichar c = (i < length ? ru::ToLowerChar(str[i]) : u';');

		if (ru::IsRussian(c) || c == u'-')
		{
			word += c;
		}
		else
		{
			if (word.length() > 0)
			{
				if (!pattern.empty())
					pattern += u' ';
				pattern += word;
				word = u"";
			}
			if (c == u';' && !pattern.empty())
			{
				patternList.push_back(pattern);
				pattern = u"";
			}
		}
	}

	return patternList;
}

Array<unistr> Card::GetWordNames() const
{
	const unistr& str = m_text.russian.GetString();
	Array<unistr> words;
	uint32 length = str.length();
	unistr word;
	uint32 count = 0;

	for (uint32 i = 0; i <= length; i++)
	{
		unichar c = (i < length ? ru::ToLowerChar(str[i]) : u';');

		if (ru::IsRussian(c))
		{
			word += c;
		}
		else if (word.length() > 0)
		{
			words.push_back(word);
			word = u"";
		}
	}

	return words;
}

void Card::SetData(const CardData& data)
{
	m_type = data.type;
	m_text = data.text;
	m_tags = data.tags;
	m_relatedCards = data.relatedCards;
}

CardData Card::GetData()
{
	CardData data;
	data.type = m_type;
	data.text = m_text;
	data.tags = m_tags;
	data.relatedCards = m_relatedCards;
	return data;
}
