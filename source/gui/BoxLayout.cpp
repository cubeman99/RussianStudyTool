#include "BoxLayout.h"

BoxLayout::BoxLayout(uint32 axis) :
	m_axis(axis)
{
	SetMargins(8);
	SetSpacing(4);
}

void BoxLayout::CalcSizes()
{
	for (auto child : m_children)
	{
		child->CalcSizes();
	}
}

void BoxLayout::Update(float timeDelta)
{
	Rect2f rect = GetBounds();
	rect.Inflate(-m_marginTopLeft.x, -m_marginTopLeft.y,
		-m_marginBottomRight.x, -m_marginBottomRight.y);

	float y = m_spacing[m_axis];
	for (uint32 i = 0; i < m_children.size(); i++)
	{
		auto child = m_children[i];
		rect.position[m_axis] = y;
		rect.size.x = child->GetMinSize().x;
		rect.size.y = child->GetMinSize().y;
		m_children[i]->SetBounds(rect);
		y += rect.size[m_axis] + m_spacing[m_axis];
	}

	for (auto child : m_children)
	{
		child->Update(timeDelta);
	}
}

void BoxLayout::Render(AppGraphics& g, float timeDelta)
{
	for (auto child : m_children)
	{
		child->Render(g, timeDelta);
	}
}

void BoxLayout::Clear()
{
	for (auto child : m_children)
		child->SetParent(nullptr);
	m_children.clear();
}

void BoxLayout::Add(GUIObject* child)
{
	child->SetParent(this);
	m_children.push_back(child);
}

void BoxLayout::SetSpacing(float spacing)
{
	m_spacing = Vector2f(spacing);
}

void BoxLayout::SetMargins(float margins)
{
	m_marginBottomRight = Vector2f(margins);
	m_marginTopLeft = Vector2f(margins);
}

HBoxLayout::HBoxLayout() :
	BoxLayout(0)
{
}

VBoxLayout::VBoxLayout() :
	BoxLayout(1)
{
}
