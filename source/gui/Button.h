#pragma once

#include "gui/Label.h"

class Button : public Label
{
public:
	Button();
	Button(const AccentedText& text, Font::sptr font,
		const Color& color = Color::WHITE, TextAlign align = TextAlign::TOP_LEFT);

	virtual void CalcSizes();
	virtual void OnUpdate(float timeDelta);
	virtual void OnRender(AppGraphics& g, float timeDelta);

	virtual void OnPress() override { m_clicked.Emit(); }

	EventSignal<>& Clicked() { return m_clicked; }

private:
	EventSignal<> m_clicked;
};
