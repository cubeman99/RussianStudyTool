#include "Button.h"

Button::Button() :
	Label()
{
}

Button::Button(const AccentedText& text, Font::sptr font,
	const Color& color, TextAlign align) :
	Label(text, font, color, align)
{
	SetFocusable(true);
}

void Button::CalcSizes()
{
	Label::CalcSizes();
}

void Button::OnUpdate(float timeDelta)
{
	Label::OnUpdate(timeDelta);
}

void Button::OnRender(AppGraphics& g, float timeDelta)
{
	Label::OnRender(g, timeDelta);
}
