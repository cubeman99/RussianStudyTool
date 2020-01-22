#include "AnchorLayout.h"

AnchorLayout::AnchorLayout()
{
}

void AnchorLayout::CalcSizes()
{
	for (auto item : m_children)
	{
		item.object->CalcSizes();
	}
}

void AnchorLayout::Update(float timeDelta)
{
	Rect2f rect = GetBounds();

	for (auto item : m_children)
	{
		Rect2f itemRect = rect;
		itemRect.position = item.position;
		itemRect.size = item.object->GetMinSize();
		item.object->SetBounds(itemRect);
	}

	for (auto item : m_children)
	{
		item.object->Update(timeDelta);
	}
}

void AnchorLayout::Render(AppGraphics& g, float timeDelta)
{
	for (auto child : m_children)
	{
		child.object->Render(g, timeDelta);
	}
}

void AnchorLayout::Add(const AnchorChild& item)
{
	item.object->SetParent(this);
	m_children.push_back(item);
}

void AnchorLayout::Clear()
{
	for (auto child : m_children)
		child.object->SetParent(nullptr);
	m_children.clear();
}

void AnchorLayout::Add(GUIObject* child, const Vector2f& position)
{
	AnchorChild item;
	item.object = child;
	item.position = position;
	Add(item);
}
