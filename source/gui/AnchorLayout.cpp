#include "AnchorLayout.h"

AnchorLayout::AnchorLayout()
{
}

uint32 AnchorLayout::GetNumChildren() const
{
	return m_children.size();
}

GUIObject* AnchorLayout::GetChild(uint32 index)
{
	return m_children[index]->object;
}

void AnchorLayout::CalcSizes()
{
}

void AnchorLayout::Update(float timeDelta)
{
	Rect2f rect = GetBounds();

	for (auto item : m_children)
	{
		Rect2f itemRect = rect;
		Vector2f mins = rect.position + (item->percentMin * rect.size) + item->offsetMin;
		Vector2f maxs = rect.position + (item->percentMax * rect.size) + item->offsetMax;
		Vector2f minSize = item->object->GetMinSize();
		itemRect.position = mins;
		itemRect.size = maxs - mins;
		Vector2f newSize(
			Math::Max(minSize.x, itemRect.size.x),
			Math::Max(minSize.y, itemRect.size.y));
		Vector2f anchor = Vector2f::ZERO;

		if (item->align != TextAlign::TOP_LEFT)
		{
			if ((int) item->align & (int) TextAlign::RIGHT)
				anchor.x = 1.0f;
			else if (!((int) item->align & (int) TextAlign::LEFT))
				anchor.x = 0.5f;
			if ((int) item->align & (int) TextAlign::BOTTOM)
				anchor.y = 1.0f;
			else if (!((int) item->align & (int) TextAlign::TOP))
				anchor.y = 0.5f;
		}

		itemRect.Resize(newSize, anchor);
		item->object->SetBounds(itemRect);
	}

	for (auto item : m_children)
	{
		item->object->Update(timeDelta);
	}
}

void AnchorLayout::Render(AppGraphics& g, float timeDelta)
{
	for (auto item : m_children)
	{
		item->object->Render(g, timeDelta);
	}
}

AnchorChild::sptr AnchorLayout::Add(AnchorChild::sptr item)
{
	m_children.push_back(item);
	item->object->SetParent(this);
	return item;
}

void AnchorLayout::Clear()
{
	m_children.clear();
	for (auto item : m_children)
		item->object->SetParent(nullptr);
}

AnchorChild& AnchorLayout::Add(GUIObject* child)
{
	AnchorChild::sptr item = cmg::make_shared<AnchorChild>();
	item->object = child;
	item->percentMin = Vector2f::ZERO;
	item->percentMax = Vector2f::ONE;
	Add(item);
	return *item;
}

AnchorChild& AnchorLayout::Add(
	GUIObject* child, const Vector2f& position)
{
	AnchorChild::sptr item = cmg::make_shared<AnchorChild>();
	item->object = child;
	item->offsetMin = position;
	item->offsetMax = position;
	Add(item);
	return *item;
}

AnchorChild& AnchorLayout::Add(
	GUIObject* child, const Vector2f& percentMin, const Vector2f& percentMax,
	const Vector2f& offset, TextAlign align)
{
	AnchorChild::sptr item = cmg::make_shared<AnchorChild>();
	item->object = child;
	item->percentMin = percentMin;
	item->percentMax = percentMax;
	item->offsetMin = offset;
	item->offsetMax = offset;
	item->align = align;
	Add(item);
	return *item;
}

AnchorChild& AnchorLayout::Add(
	GUIObject* child, const Vector2f& percentMin, const Vector2f& percentMax,
	const Vector2f& offsetMin, const Vector2f& offsetMax, TextAlign align)
{
	AnchorChild::sptr item = cmg::make_shared<AnchorChild>();
	item->object = child;
	item->percentMin = percentMin;
	item->percentMax = percentMax;
	item->offsetMin = offsetMin;
	item->offsetMax = offsetMax;
	item->align = align;
	Add(item);
	return *item;
}

AnchorChild& AnchorChild::Pin(const Vector2f & percentage)
{
	percentMin = percentage;
	percentMax = percentage;
	return *this;
}

AnchorChild& AnchorChild::Pin(float x, float y)
{
	percentMin = Vector2f(x, y);
	percentMax = Vector2f(x, y);
	return *this;
}

AnchorChild & AnchorChild::Pin(float x, float y, TextAlign align)
{
	percentMin = Vector2f(x, y);
	percentMax = Vector2f(x, y);
	this->align = align;
	return *this;
}

AnchorChild& AnchorChild::Pin(float left, float top, float right, float bottom)
{
	percentMin = Vector2f(left, top);
	percentMax = Vector2f(right, bottom);
	return *this;
}

AnchorChild& AnchorChild::Offset(const Vector2f& offset)
{
	offsetMin = offset;
	offsetMax = offset;
	return *this;
}

AnchorChild& AnchorChild::Offset(float x, float y)
{
	offsetMin = Vector2f(x, y);
	offsetMax = Vector2f(x, y);
	return *this;
}

AnchorChild & AnchorChild::Offset(float left, float top, float right, float bottom)
{
	offsetMin = Vector2f(left, top);
	offsetMax = Vector2f(right, bottom);
	return *this;
}

AnchorChild& AnchorChild::Rect(float left, float top, float width, float height)
{
	offsetMin = Vector2f(left, top);
	offsetMax = Vector2f(left + width, top + height);
	return *this;
}

AnchorChild& AnchorChild::Rect(const Rect2f& rect)
{
	offsetMin = rect.GetTopLeft();
	offsetMin = rect.GetBottomRight();
	return *this;
}

AnchorChild & AnchorChild::Align(TextAlign align)
{
	this->align = align;
	return *this;
}
