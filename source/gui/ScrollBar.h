#pragma once

#include "gui/Widget.h"


class ScrollBar : public Widget
{
public:
	ScrollBar(uint32 axis);

	uint32 GetAxis() const;
	float GetValue() const;
	float GetPageStep() const;

	void SetMinimumValue(float minValue);
	void SetMaximumValue(float maxValue);
	void SetPageStep(float pageStep);
	void SetValue(float value);
	void PageUp();
	void PageDown();

	virtual void CalcSizes() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	float m_value = 0.0f;
	float m_minimum = 0.0f;
	float m_maximum = 100.0f;
	float m_pageStep = 10.0f;
	uint32 m_axis = 0;
	float m_endSize = 20.0f;
};

