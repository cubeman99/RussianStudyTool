#pragma once

#include <cmgGraphics/cmg_graphics.h>
#include <cmgMath/cmg_math.h>
#include "AccentedText.h"


class AppGraphics : public Graphics2D
{
public:
	AppGraphics(Window* window) :
		Graphics2D(window)
	{
	}

	void DrawAccentedText(Font* font, const AccentedText& text,
		const Vector2f& position, const Color& color = Color::WHITE,
		TextAlign align = TextAlign::TOP_LEFT);

};

