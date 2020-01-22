#include "GUIObject.h"

GUIObject::GUIObject()
{
}

GUIObject::~GUIObject()
{
}

void GUIObject::SetParent(GUIObject * parent)
{
	m_parent = parent;
}

void GUIObject::SetBounds(const Rect2f & bounds)
{
	m_bounds = bounds;
}
