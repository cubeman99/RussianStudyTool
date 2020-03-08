#include "LanguageTextEdit.h"

LanguageTextEdit::LanguageTextEdit(Language language) :
	LanguageTextEdit(language, AccentedText())
{
}

LanguageTextEdit::LanguageTextEdit(Language language, const AccentedText& text) :
	TextEdit(text),
	m_language(language)
{
	AddKeyShortcut("Ctrl+Q", [this]() {
		return ApplyLanguage();
	});
}

bool LanguageTextEdit::ApplyLanguage()
{
	const unistr& text = GetText();
	unistr modifiedText;
	if (m_language == Language::k_russian)
		modifiedText = ru::ToRussianKeyboard(text);
	else
		modifiedText = ru::ToEnglishKeyboard(text);
	if (text != modifiedText)
	{
		SetText(modifiedText);
		return true;
	}
	return false;
}

RussianTextEdit::RussianTextEdit() :
	LanguageTextEdit(Language::k_russian)
{
}

RussianTextEdit::RussianTextEdit(const AccentedText& text) :
	LanguageTextEdit(Language::k_russian, text)
{
}

EnglishTextEdit::EnglishTextEdit() :
	LanguageTextEdit(Language::k_english)
{
}

EnglishTextEdit::EnglishTextEdit(const AccentedText& text) :
	LanguageTextEdit(Language::k_english, text)
{
}
