#include "GUIObject.h"
#include "GUIManager.h"

GUIObject::GUIObject()
{
}

GUIObject::~GUIObject()
{
	// Disconnect from parent
	SetParent(nullptr);

	// Disconnect from children
	uint32 numChildren = GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		GUIObject* child = GetChild(i);
		if (child)
			child->SetParent(nullptr);
	}
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

void GUIObject::SetParent(GUIObject* parent)
{
	if (parent)
	{
		// Added to a GUI manager
		m_parent = parent;
		if (parent->m_guiManager)
			parent->m_guiManager->InitializeObjects(this);
	}
	else
	{
		// Removed from a GUI manager
		if (m_guiManager)
			m_guiManager->UninitializeObjects(this);
		m_parent = nullptr;
	}
}

void GUIObject::SetBounds(const Rect2f& bounds)
{
	m_bounds = bounds;
}

GUIObjectIterator GUIObject::objects_begin()
{
	return GUIObjectIterator(this);
}

GUIObjectIterator GUIObject::objects_end()
{
	return GUIObjectIterator(nullptr);
}




GUIObjectIterator::GUIObjectIterator(GUIObject* object) :
	m_object(object)
{
}

GUIObject* GUIObjectIterator::operator->() const
{
	return m_object;
}

GUIObject* GUIObjectIterator::operator*() const
{
	return m_object;
}

GUIObjectIterator& GUIObjectIterator::operator++()
{
	if (m_object)
	{
		if (m_object->GetNumChildren() > 0)
		{
			// Traverse into a child node
			m_object = m_object->GetChild(0);
			m_indices.push_back(0);
		}
		else
		{
			// Traverse up until we can reach a new sibling node
			m_object = m_object->GetParent();
			while (m_object)
			{
				uint32 nextSiblingIndex = m_indices.back() + 1;
				if (nextSiblingIndex < m_object->GetNumChildren())
				{
					m_object = m_object->GetChild(nextSiblingIndex);
					m_indices.back() = nextSiblingIndex;
					break;
				}
				else
				{
					m_object = m_object->GetParent();
					m_indices.pop_back();
					if (m_indices.empty())
						m_object = nullptr;
				}
			}
		}
	}
	return *this;
}
GUIObjectIterator GUIObjectIterator::operator++(int)
{
	const auto temp(*this);
	++(*this);
	return temp;
}

bool GUIObjectIterator::operator==(const GUIObjectIterator& other) const
{
	return (m_object == other.m_object);
}
bool GUIObjectIterator::operator!=(const GUIObjectIterator& other) const
{
	return (m_object != other.m_object);
}
