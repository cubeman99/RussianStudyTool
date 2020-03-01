#include "RelatedWordList.h"
#include "RussianApp.h"

RelatedWordList::RelatedWordList()
{
	m_layoutWords.SetSpacing(8.0f);
	m_layoutWords.SetMargins(0.0f);
	m_layout.SetSpacing(8.0f);
	m_layout.SetMargins(0.0f);
	m_layout.Add(&m_label);
	m_layout.Add(&m_layoutWords);
	m_layout.AddStretch();
	SetLayout(&m_layout);
}

void RelatedWordList::Clear()
{
	m_layoutWords.Clear();
}

void RelatedWordList::SetLabel(const AccentedText & text)
{
	m_label.SetText(text);
}

RelatedWordWidget* RelatedWordList::AddWord(const AccentedText& text)
{
	RelatedWordWidget* widget = new RelatedWordWidget(text);
	widget->m_label.SetFont(m_font);
	m_layoutWords.Add(widget);
	return widget;
}

void RelatedWordList::OnInitialize()
{
	m_font = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);
	m_label.SetFont(m_font);
}

RelatedWordWidget::RelatedWordWidget(const AccentedText& text) :
	m_label(text)
{
	m_label.SetFocusable(false);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.Add(&m_label);
	SetLayout(&m_mainLayout);
	m_label.SetBackgroundColor(GUIConfig::color_background_alternate);
}

void RelatedWordWidget::OnInitialize()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	auto& wiktionary = GetApp()->GetWiktionary();

	m_card = nullptr;
	m_term = nullptr;
	unistr text = m_label.GetText().GetString();
	ru::ToLowerIP(text);

	for (WordType wordType : EnumValues<WordType>())
	{
		CardRuKey key(wordType, text);
		cmg::string::ReplaceAll(key.russian, u"ё", u"е");
		m_card = cardDatabase.GetCard(key);
		if (m_card)
			break;
	}
	m_term = wiktionary.GetTerm(text);

	if (m_card)
	{
		m_label.SetBackgroundColor(Color::DARK_GREEN);
	}
	else if (m_term)
	{
		m_label.SetBackgroundColor(Color::DARK_BLUE);
	}
	else
	{
		m_label.SetBackgroundColor(GUIConfig::color_background_alternate);
	}
}
