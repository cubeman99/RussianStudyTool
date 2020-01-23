#include "GUIObject.h"

GUIObject::GUIObject()
{
}

GUIObject::~GUIObject()
{
}

uint32 GUIObject::GetNumChildren() const
{
	return 0;
}

GUIObject* GUIObject::GetChild(uint32 index)
{
	CMG_ASSERT_FALSE("No children!");
	return nullptr;
}

void GUIObject::SetParent(GUIObject * parent)
{
	m_parent = parent;
}

void GUIObject::SetBounds(const Rect2f & bounds)
{
	m_bounds = bounds;
}
