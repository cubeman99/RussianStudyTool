#include "RelatedWordList.h"
#include "RussianApp.h"

RelatedWordList::RelatedWordList()
{
	m_layoutWords.SetSpacing(2.0f);
	m_layoutWords.SetMargins(0.0f);
	m_layout.SetSpacing(4.0f);
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

void RelatedWordList::AddWord(const AccentedText & text)
{
	RelatedWordWidget* label = new RelatedWordWidget();
	label->SetText(text);
	label->SetFont(m_font);
	m_layoutWords.Add(label);
}

void RelatedWordList::OnInitialize()
{
	m_font = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);
	m_label.SetFont(m_font);
}

RelatedWordWidget::RelatedWordWidget()
{
	SetBackgroundColor(GUIConfig::color_background_alternate);
}
