#include "ScrollBar.h"

ScrollBar::ScrollBar(uint32 axis) :
	m_axis(axis)
{
}

uint32 ScrollBar::GetAxis() const
{
	return m_axis;
}

float ScrollBar::GetValue() const
{
	return m_value;
}

float ScrollBar::GetPageStep() const
{
	return m_pageStep;
}

void ScrollBar::SetMinimumValue(float minValue)
{
	m_minimum = minValue;
	m_value = Math::Clamp(m_value, m_minimum, m_maximum);
}

void ScrollBar::SetMaximumValue(float maxValue)
{
	m_maximum = maxValue;
	m_value = Math::Clamp(m_value, m_minimum, m_maximum);
}

void ScrollBar::SetPageStep(float pageStep)
{
	m_pageStep = pageStep;
}

void ScrollBar::SetValue(float value)
{
	m_value = Math::Clamp(value, m_minimum, m_maximum);
}

void ScrollBar::PageUp()
{
	SetValue(m_value - m_pageStep);
}

void ScrollBar::PageDown()
{
	SetValue(m_value + m_pageStep);
}

void ScrollBar::CalcSizes()
{
	Widget::CalcSizes();
	float w = 20.0f;
	m_minSize[1 - m_axis] = w;
	m_maxSize[1 - m_axis] = w;
}

void ScrollBar::OnUpdate(float timeDelta)
{
}

void ScrollBar::OnRender(AppGraphics& g, float timeDelta)
{
	Color color = GUIConfig::color_background_light;
	SetBackgroundColor(color);

	Rect2f barRect = GetBounds();

	float spanSize = barRect.size[m_axis];
	float percent = (m_value - m_minimum) / (m_maximum - m_minimum);
	float barSize = (spanSize * m_pageStep) /
		(m_maximum - m_minimum + m_pageStep);
	float barOffset = (spanSize - barSize) * percent;

	barRect.position[m_axis] += barOffset;
	barRect.size[m_axis] = barSize;
	barRect.Inflate(-3, -3);

	color = GUIConfig::color_button_background;
	g.FillRect(barRect, color);
}
