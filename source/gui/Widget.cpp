#include "Widget.h"
#include "Layout.h"

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

uint32 Widget::GetNumChildren() const
{
	return 1;
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
		m_layout->SetBounds(m_bounds);
		m_layout->CalcSizes();
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
	//else
	//	g.DrawRect(m_bounds, GUIConfig::color_outline);
}
