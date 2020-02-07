#pragma once

#include "gui/Widget.h"

class Label : public Widget
{
public:
	Label();
	Label(const AccentedText& text, Font::sptr font = nullptr,
		const Color& color = GUIConfig::color_text,
		TextAlign align = TextAlign::MIDDLE_LEFT);

	const AccentedText& GetText() const { return m_text; }
	void SetText(const AccentedText& text) { m_text = text; }
	void SetFont(Font::sptr font) { m_font = font; }
	void SetColor(const Color& color) { m_color = color; }
	void SetAlign(TextAlign align) { m_align = align; }

	virtual void CalcSizes() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	AccentedText m_text;
	Font::sptr m_font;
	Color m_color = Color::WHITE;
	TextAlign m_align = TextAlign::MIDDLE_LEFT;
};
