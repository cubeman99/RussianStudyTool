#include "Label.h"

Label::Label()
{
}

Label::Label(const AccentedText& text, Font::sptr font,
	const Color& color, TextAlign align) :
	m_text(text),
	m_font(font),
	m_color(color),
	m_align(align)
{
}

void Label::CalcSizes()
{
	m_minSize.y = m_font->GetSize() * 2.0f;
	m_maxSize.y = m_minSize.y;
	Rect2f box = Graphics2D::MeasureString(m_font.get(), m_text.GetString());
	m_minSize.x = box.GetRight();
}

void Label::OnUpdate(float timeDelta)
{
}

void Label::OnRender(AppGraphics& g, float timeDelta)
{
	g.DrawAccentedText(m_font.get(), m_text, GetBounds().GetTopLeft(), m_color, m_align);
}
