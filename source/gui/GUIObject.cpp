#include "GUIObject.h"
#include "GUIManager.h"

int32 GUIObject::s_globalAllocationCount = 0;

GUIObject::GUIObject()
{
}

GUIObject::~GUIObject()
{
	// Disconnect from parent
	SetParent(nullptr);

	// Disconnect from children
	auto children = GetArrayOfChildren();
	for (GUIObject* child : children)
		child->SetParent(nullptr);

	// Deallocate all objects
	Array<GUIObject*> allocatedObjects = m_allocatedObjects;
	for (GUIObject* object : allocatedObjects)
		DeallocateObject(object);
	m_allocatedObjects.clear();
}

Array<GUIObject*> GUIObject::GetArrayOfChildren()
{
	uint32 numChildren = GetNumChildren();
	Array<GUIObject*> children;
	for (uint32 i = 0; i < numChildren; i++)
	{
		GUIObject* childObject = GetChild(i);
		if (childObject)
			children.push_back(childObject);
	}
	return children;
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

void GUIObject::Initialize(GUIManager* guiManager)
{
	CMG_ASSERT(guiManager != nullptr);
	m_guiManager = guiManager;

	// Initialize the object and all its children
	auto children = GetArrayOfChildren();
	for (GUIObject* childObject : children)
		childObject->Initialize(guiManager);

	OnInitialize();

	m_guiManager->OnObjectInitialized(this);
}

void GUIObject::Uninitialize()
{
	// Uninitialize all children
	auto children = GetArrayOfChildren();
	for (GUIObject* childObject : children)
		childObject->Uninitialize();

	OnUninitialize();

	m_guiManager->OnObjectUninitialized(this);
	m_parent = nullptr;
	m_guiManager = nullptr;
}

void GUIObject::SetParent(GUIObject* parent)
{
	if (parent)
	{
		// Added to a GUI manager
		m_parent = parent;
		if (m_guiManager != parent->m_guiManager)
			Initialize(parent->m_guiManager);
	}
	else if (m_parent)
	{
		// Removed from a GUI manager
		if (m_guiManager)
			Uninitialize();
		m_parent = nullptr;
	}
}

void GUIObject::SetBounds(const Rect2f& bounds)
{
	m_bounds = bounds;
}

void GUIObject::AllocateObject(GUIObject* object)
{
	if (!cmg::container::Contains(m_allocatedObjects, object))
	{
		//CMG_LOG_DEBUG() << "Allocating object.";
		m_allocatedObjects.push_back(object);
		s_globalAllocationCount++;
	}
}

void GUIObject::DeallocateObject(GUIObject* object)
{
	if (cmg::container::EraseIfFound(m_allocatedObjects, object))
	{
		//CMG_LOG_DEBUG() << "Deallocating object.";
		object->SetParent(nullptr);
		delete object;
		s_globalAllocationCount--;
	}
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
