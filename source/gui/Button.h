#pragma once

#include "gui/Label.h"

class Button : public Label
{
public:
	Button();
	Button(const AccentedText& text, Font::sptr font = nullptr,
		const Color& color = Color::WHITE);

	virtual bool OnMouseDown(MouseButtons::value_type buttons, const Vector2f& location) override;
	virtual void OnMouseUp(MouseButtons::value_type buttons, const Vector2f& location) override;
	virtual void CalcSizes() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

	virtual void OnPress() override { m_clicked.Emit(); }

	EventSignal<>& Clicked() { return m_clicked; }

private:
	EventSignal<> m_clicked;
	bool m_isDown = false;
};
