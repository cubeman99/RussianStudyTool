#include "Widget.h"
#include "Layout.h"
#include "GUIManager.h"

Widget::Widget()
{
}

Widget::~Widget()
{
}

void Widget::SetLayout(Layout* layout)
{
	if (m_layout)
		m_layout->SetParent(nullptr);
	m_layout = layout;
	if (m_layout)
		m_layout->SetParent(this);
}

void Widget::Close()
{
	m_isVisible = false;
}

void Widget::Focus()
{
	if (m_guiManager && m_isFocusable && !m_isFocused
		&& m_isEnabled && m_isVisible)
	{
		m_guiManager->SetFocus(this);
	}
}

uint32 Widget::GetNumChildren() const
{
	return (m_layout ? 1 : 0);
}

GUIObject* Widget::GetChild(uint32 index)
{
	CMG_ASSERT(index < 1);
	return m_layout;
}

void Widget::CalcSizes()
{
	if (m_layout)
	{
		m_minSize = m_layout->GetMinSize();
		m_maxSize = m_layout->GetMaxSize();
	}
}

void Widget::Update(float timeDelta)
{
	OnUpdate(timeDelta);

	if (m_layout)
	{
		m_layout->SetBounds(m_bounds);
		m_layout->Update(timeDelta);
	}
}

void Widget::Render(AppGraphics& g, float timeDelta)
{
	if (!m_isVisible)
		return;
	if (m_backgroundColor.a > 0)
		g.FillRect(m_bounds, m_backgroundColor);

	if (m_layout)
		m_layout->Render(g, timeDelta);
	
	OnRender(g, timeDelta);

	if (m_isFocused)
		g.DrawRect(m_bounds, GUIConfig::color_outline_focused);
	else
		g.DrawRect(m_bounds, GUIConfig::color_outline);

	if (!m_isEnabled)
	{
		g.FillRect(m_bounds, GUIConfig::color_background * 0.5f);
	}
}
