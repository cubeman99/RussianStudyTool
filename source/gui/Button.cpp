#include "Button.h"

Button::Button() :
	Button("", nullptr, GUIConfig::color_text)
{
}

Button::Button(const AccentedText& text, Font::sptr font,
	const Color& color) :
	Label(text, font, color, TextAlign::CENTERED)
{
	SetFocusable(true);
	SetBackgroundColor(GUIConfig::color_button_background);
}

bool Button::OnMouseDown(MouseButtons::value_type buttons, const Vector2f& location)
{
	m_isDown = true;
	return true;
}

void Button::OnMouseUp(MouseButtons::value_type buttons, const Vector2f& location)
{
	m_isDown = false;
	OnPress();
}

void Button::CalcSizes()
{
	Label::CalcSizes();
	m_maxSize.x = DEFAULT_MAX_SIZE;
}

void Button::OnUpdate(float timeDelta)
{
	Label::OnUpdate(timeDelta);
}

void Button::OnRender(AppGraphics& g, float timeDelta)
{
	Color color = GetBackgroundColor();
	if (m_isDown)
	{
		auto rect = GetBounds();
		g.FillRect(rect, Color::Lerp(color, Color::WHITE, 0.2f));
	}

	Label::OnRender(g, timeDelta);
}
