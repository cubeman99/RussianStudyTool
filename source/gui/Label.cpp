#include "Label.h"
#include "RussianApp.h"

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
	if (!m_font)
		m_font = RussianStudyToolApp::GetInstance()->GetResourceManager()->Get<Font>(Res::FONT);
	m_minSize.y = m_font->GetSize() * 2.0f;
	m_maxSize.y = m_minSize.y;
	Rect2f box = Graphics2D::MeasureString(m_font.get(), m_text.GetString());
	m_minSize.x = box.GetRight();
	m_maxSize.x = DEFAULT_MAX_SIZE;
}

void Label::OnUpdate(float timeDelta)
{
}

void Label::OnRender(AppGraphics& g, float timeDelta)
{
	if (!m_font)
		m_font = RussianStudyToolApp::GetInstance()->GetResourceManager()->Get<Font>(Res::FONT);

	Vector2f anchor = Vector2f::ZERO;

	if (m_align != TextAlign::TOP_LEFT)
	{
		if ((int) m_align & (int) TextAlign::RIGHT)
			anchor.x = 1.0f;
		else if (!((int) m_align & (int) TextAlign::LEFT))
			anchor.x = 0.5f;
		if ((int) m_align & (int) TextAlign::BOTTOM)
			anchor.y = 1.0f;
		else if (!((int) m_align & (int) TextAlign::TOP))
			anchor.y = 0.5f;
	}

	auto rect = GetBounds();
	Vector2f point = rect.position + anchor * rect.size;
	g.DrawAccentedText(m_font.get(), m_text, point, m_color, m_align);
}
