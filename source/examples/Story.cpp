#include "Story.h"

const AccentedText& Story::GetTitle() const
{
	return m_title;
}

const unistr& Story::GetURL() const
{
	return m_url;
}

Chapter::sptr Story::AddChapter(const AccentedText& title)
{
	return AddChapter(m_chapters.size() + 1, title);
}

Chapter::sptr Story::AddChapter(uint32 number, const AccentedText& title)
{
	Chapter::sptr chapter = cmg::make_shared<Chapter>(title, number);
	m_chapters.push_back(chapter);
	return chapter;
}

const AccentedText& Chapter::GetTitle() const
{
	return m_title;
}

uint32 Chapter::GetNumber() const
{
	return m_number;
}

Paragraph::sptr Chapter::AddParagraph(const AccentedText& text)
{
	Paragraph::sptr paragraph = cmg::make_shared<Paragraph>(text);
	m_paragraphs.push_back(paragraph);
	return paragraph;
}

Paragraph::Paragraph(const AccentedText& text) :
	m_text(text)
{
	unistr str = text.GetString();
	uint32 length = str.length();
	unistr sentence;
	bool sentenceDone = false;

	for (uint32 i = 0; i < length; i++)
	{
		unichar c = str[i];

		if (c == '.' || c == '!' || c == '?')
		{
			sentenceDone = true;
		}
		else if (sentenceDone)
		{
			sentenceDone = false;
			if (sentence.length() > 0)
			{
				m_sentences.push_back(sentence);
				sentence = u"";
			}
		}
		sentence += c;
	}

	if (sentence.length() > 0)
		m_sentences.push_back(sentence);
}

const AccentedText& Paragraph::GetText() const
{
	return m_text;
}
