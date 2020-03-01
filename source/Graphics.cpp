#include "Graphics.h"

void AppGraphics::DrawAccentedText(Font* font, const AccentedText& text,
	const Vector2f& position, const Color& color, TextAlign align)
{
	const auto& string = text.GetString();
	Texture* texture = font->GetGlyphTexture();
	const Glyph* glyph = nullptr;
	const Glyph* accentGlyph = font->GetGlyph(
		(uint32) AccentedText::ACCENT_RENDER_CHAR);

	Vector2f penPosition = position;
	float lineSpacing = font->GetSize() * 2.0f;

	if (align != TextAlign::BOTTOM_LEFT)
	{
		Rect2f bounds = MeasureString(font, string);
		if ((int) align & (int) TextAlign::RIGHT)
			penPosition.x -= bounds.GetRight();
		else if (!((int) align & (int) TextAlign::LEFT))
			penPosition.x -= bounds.GetRight() -
			(float) ((int) bounds.GetWidth() / 2);
		if ((int) align & (int) TextAlign::TOP)
			penPosition.y -= bounds.GetTop();
		else if (!((int) align & (int) TextAlign::BOTTOM))
			penPosition.y -= bounds.GetTop() +
			(float) ((int) bounds.GetHeight() / 2);
	}

	Vector2f penPositionStart = penPosition;
	BeginDraw(texture);
	uint32 accentIndex = 0;

	// Draw each glyph in the string of characters
	for (uint32 charIndex = 0; charIndex < string.length(); charIndex++)
	{
		unichar c = string[charIndex];
		uint32 charCode = static_cast<uint32>(c);
		glyph = font->GetGlyph(charCode);
		//if (!glyph)
		//	glyph = font->GetGlyph((uint32) u'¿');

		if (charCode == (uint32) '\n')
		{
			penPosition.x = penPositionStart.x;
			penPosition.y += lineSpacing;
		}
		else if (glyph)
		{
			DrawGlyph(texture, *glyph, penPosition, color);
		}

		// Draw accent mark
		if (glyph && accentIndex < text.GetAccentCount() &&
			charIndex == text.GetAccentIndex(accentIndex))
		{
			float xx = (glyph->GetMinX() + glyph->GetMaxX()) * 0.5f;
			xx -= (accentGlyph->GetMinX() + accentGlyph->GetMaxX()) * 0.5f;
			DrawGlyph(texture, *accentGlyph, penPosition + Vector2f(xx, 0), color);
			accentIndex++;
		}

		// Advance the pen position
		if (glyph)
			penPosition.x += glyph->GetAdvance();
	}

	EndDraw();
}
