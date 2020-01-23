﻿#include "AnchorLayout.h"

AnchorLayout::AnchorLayout()
{
}

uint32 AnchorLayout::GetNumChildren() const
{
	return m_children.size();
}

GUIObject* AnchorLayout::GetChild(uint32 index)
{
	return m_children[index].object;
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
		Vector2f mins = rect.position + (item.percentMin * rect.size);
		Vector2f maxs = rect.position + (item.percentMax * rect.size);
		Vector2f minSize = item.object->GetMinSize();
		itemRect.position = mins + item.offset;
		itemRect.size = maxs - mins;
		Vector2f newSize(
			Math::Max(minSize.x, itemRect.size.x),
			Math::Max(minSize.y, itemRect.size.y));
		Vector2f anchor = Vector2f::ZERO;

		if (item.align != TextAlign::TOP_LEFT)
		{
			if ((int) item.align & (int) TextAlign::RIGHT)
				anchor.x = 1.0f;
			else if (!((int) item.align & (int) TextAlign::LEFT))
				anchor.x = 0.5f;
			if ((int) item.align & (int) TextAlign::BOTTOM)
				anchor.y = 1.0f;
			else if (!((int) item.align & (int) TextAlign::TOP))
				anchor.y = 0.5f;
		}

		itemRect.Resize(newSize, anchor);
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

void AnchorLayout::Add(GUIObject * child)
{
	AnchorChild item;
	item.object = child;
	item.percentMin = Vector2f::ZERO;
	item.percentMax = Vector2f::ONE;
	Add(item);
}

void AnchorLayout::Add(GUIObject* child, const Vector2f& position)
{
	AnchorChild item;
	item.object = child;
	item.offset = position;
	Add(item);
}

void AnchorLayout::Add(GUIObject* child, const Vector2f& percentMin, const Vector2f& percentMax,
	const Vector2f& offset, TextAlign align)
{
	AnchorChild item;
	item.object = child;
	item.percentMin = percentMin;
	item.percentMax = percentMax;
	item.offset = offset;
	item.align = align;
	Add(item);
}