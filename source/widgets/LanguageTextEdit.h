#pragma once

#include "study/StudySetMetrics.h"
#include "widgets/AppWidget.h"


class LanguageTextEdit : public TextEdit
{
public:
	LanguageTextEdit(Language language);
	LanguageTextEdit(Language language, const AccentedText& text);

	bool ApplyLanguage();

private:
	Language m_language;
};


class RussianTextEdit : public LanguageTextEdit
{
public:
	RussianTextEdit();
	RussianTextEdit(const AccentedText& text);
};


class EnglishTextEdit : public LanguageTextEdit
{
public:
	EnglishTextEdit();
	EnglishTextEdit(const AccentedText& text);
};

