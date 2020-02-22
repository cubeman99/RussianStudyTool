#include "ExampleTextWidget.h"
#include "RussianApp.h"

ExampleTextWidget::ExampleTextWidget(const SentenceMatch& match)
{
	m_matchLocations = match.locations;
	SetText(match.sentence);
}

void ExampleTextWidget::OnRender(AppGraphics& g, float timeDelta)
{
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
	Vector2f cursor = rect.position + anchor * rect.size;
	uint32 index = 0;
	for (auto& location : m_matchLocations)
	{
		if (location.first > index)
		{
			AccentedText subText = m_text.substr(index, location.first);
			g.DrawAccentedText(m_font.get(), subText, cursor, m_color, m_align);
			cursor.x += g.MeasureString(m_font.get(), subText.GetString()).GetRight();
		}

		AccentedText matchText = m_text.GetString().substr(
			location.first, location.second - location.first);
		g.DrawAccentedText(m_font.get(), matchText, cursor, Color::RED, m_align);
		cursor.x += g.MeasureString(m_font.get(), matchText.GetString()).GetRight();
		index = location.second;
	}

	if (index < m_text.length())
	{
		AccentedText subText = m_text.substr(index);
		g.DrawAccentedText(m_font.get(), subText, cursor, m_color, m_align);
	}
}
