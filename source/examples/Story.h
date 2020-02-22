#pragma once

#include "Russian.h"

class ExampleDatabase;


class Paragraph
{
public:
	friend class ExampleDatabase;
	using sptr = cmg::shared_ptr<Paragraph>;

	Paragraph() {}
	Paragraph(const AccentedText& text);

	const AccentedText& GetText() const;


private:
	AccentedText m_text;
	Array<AccentedText> m_sentences;
};


class Chapter
{
public:
	friend class ExampleDatabase;
	using sptr = cmg::shared_ptr<Chapter>;

	Chapter() {}
	Chapter(const AccentedText& title, uint32 number) :
		m_title(title),
		m_number(number)
	{
	}

	const AccentedText& GetTitle() const;
	uint32 GetNumber() const;
	Paragraph::sptr AddParagraph(const AccentedText& text);


private:
	AccentedText m_title;
	uint32 m_number = 1;
	Array<Paragraph::sptr> m_paragraphs;
};


class Story
{
public:
	friend class ExampleDatabase;
	using sptr = cmg::shared_ptr<Story>;

	Story() {}

	const AccentedText& GetTitle() const;
	const unistr& GetURL() const;

	Chapter::sptr AddChapter(const AccentedText& title);
	Chapter::sptr AddChapter(uint32 number, const AccentedText& title);

private:
	AccentedText m_title;
	unistr m_url;
	Array<Chapter::sptr> m_chapters;
};
