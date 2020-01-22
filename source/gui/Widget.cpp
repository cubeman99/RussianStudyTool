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
	if (m_layout)
		m_layout->Render(g, timeDelta);

	OnRender(g, timeDelta);

	if (m_isFocused)
		g.DrawRect(m_bounds, Color::BLUE);
	else
		g.DrawRect(m_bounds, Color::GRAY);
}
