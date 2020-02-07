#include "BoxLayout.h"

BoxLayout::BoxLayout(uint32 axis) :
	m_axis(axis)
{
	SetMargins(2);
	SetSpacing(2);
}

uint32 BoxLayout::GetNumChildren() const
{
	return m_children.size();
}

GUIObject* BoxLayout::GetChild(uint32 index)
{
	return m_children[index].object;
}

void BoxLayout::CalcSizes()
{
	m_maxSize = Vector2f(GUIObject::DEFAULT_MAX_SIZE);
	m_minSize = Vector2f::ZERO;
	for (auto& item : m_children)
	{
		if (item.object->IsVisible())
		{
			Vector2f childMaxSize = item.object->GetMaxSize();
			m_maxSize[m_axis] += childMaxSize[m_axis];
			m_maxSize[1 - m_axis] = Math::Min(
				m_maxSize[1 - m_axis], childMaxSize[1 - m_axis]);
			Vector2f childMinSize = item.object->GetMinSize();
			m_minSize[m_axis] += childMinSize[m_axis];
			m_minSize[1 - m_axis] = Math::Max(
				m_minSize[1 - m_axis], childMinSize[1 - m_axis]);
		}
	}
	m_minSize += m_marginTopLeft + m_marginBottomRight +
		(m_spacing * ((float) m_children.size() - 1));
	m_maxSize += m_marginTopLeft + m_marginBottomRight +
		(m_spacing * ((float) m_children.size() - 1));
}

void BoxLayout::Update(float timeDelta)
{
	if (m_children.empty())
		return;

	Rect2f rect = GetBounds();
	rect.Inflate(-m_marginTopLeft.x, -m_marginTopLeft.y,
		-m_marginBottomRight.x, -m_marginBottomRight.y);

	float stretchSpace = rect.size[m_axis];
	stretchSpace -= m_spacing[m_axis] * ((float) m_children.size() - 1);
	float stretchFactorSum = 0.0f;
	bool fixedStretch = false;
	for (auto& item : m_children)
	{
		stretchFactorSum += item.stretch;
		item.isMaxed = false;
		item.isMinned = false;
	}
	if (stretchFactorSum == 0.0f)
	{
		stretchFactorSum = (float) m_children.size();
		fixedStretch = true; 
	}

	for (uint32 k = 0; k < 5; k++)
	{
		float offset = 0.0f;
		for (uint32 i = 0; i < m_children.size(); i++)
		{
			auto& item = m_children[i];
			if (!item.isMaxed && !item.isMinned)
			{
				float maxSize = item.object->GetMaxSize()[m_axis];
				float minSize = item.object->GetMinSize()[m_axis];
				float percent = 1.0f / stretchFactorSum;
				if (!fixedStretch)
					percent = item.stretch / stretchFactorSum;
				item.size = stretchSpace * percent;
				item.isMaxed = item.size > maxSize;
				item.isMinned = item.size < minSize;
				item.size = Math::Clamp(item.size, minSize, maxSize);
				if (item.isMaxed || item.isMinned)
				{
					stretchSpace -= item.size;
					stretchFactorSum -= fixedStretch ? 1.0f : item.stretch;
				}
			}
			item.offset = offset;
			offset += item.size + m_spacing[m_axis];
		}
	}

	for (auto& item : m_children)
	{
		Rect2f childRect;
		childRect.position[m_axis] = rect.position[m_axis] + item.offset;
		childRect.size[m_axis] = item.size;
		childRect.position[1 - m_axis] = rect.position[1 - m_axis];
		childRect.size[1 - m_axis] = rect.size[1 - m_axis];
		item.object->SetBounds(childRect);
	}

	for (auto& item : m_children)
		item.object->Update(timeDelta);
}

void BoxLayout::Render(AppGraphics& g, float timeDelta)
{
	for (uint32 i = 0; i < m_children.size(); i++)
	{
		auto& item = m_children[i];

		if (m_showBackgroundColors)
		{
			Color color = GUIConfig::color_background;
			if (i % 2 == 1)
				color = GUIConfig::color_background_alternate;
			g.FillRect(item.object->GetBounds(), color);
		}

		item.object->Render(g, timeDelta);
	}
}

void BoxLayout::Clear()
{
	m_children.clear();
	for (auto& item : m_children)
		item.object->SetParent(nullptr);
}

void BoxLayout::Add(GUIObject* child, float stretch)
{
	BoxChild item;
	item.object = child;
	item.stretch = stretch;
	m_children.push_back(item);
	item.object->SetParent(this);
}

void BoxLayout::Add(const Array<GUIObject*>& children)
{
	for (GUIObject* child : children)
		Add(child);
}

void BoxLayout::AddStretch(float stretch)
{
	Add(new Widget(), stretch);
}

void BoxLayout::SetStretch(GUIObject * child, float stretch)
{
	for (BoxChild& item : m_children)
	{
		if (item.object == child)
		{
			item.stretch = stretch;
			return;
		}
	}
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

void BoxLayout::SetItemBackgroundColors(bool enabled)
{
	m_showBackgroundColors = enabled;
}

HBoxLayout::HBoxLayout() :
	BoxLayout(0)
{
}

VBoxLayout::VBoxLayout() :
	BoxLayout(1)
{
}
