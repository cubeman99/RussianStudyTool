#pragma once

#include <cmgMath/cmg_math.h>
#include "AccentedText.h"
#include "Graphics.h"
#include "Events.h"
#include "Config.h"

class Layout;
class GUIManager;
class GUIObject;


class GUIObjectIterator
{
public:
	using value_type = GUIObject*;

	GUIObjectIterator(GUIObject* object);

	GUIObject* operator->() const;
	GUIObject* operator*() const;
	GUIObjectIterator& operator++();
	GUIObjectIterator operator++(int);
	bool operator==(const GUIObjectIterator& other) const;
	bool operator!=(const GUIObjectIterator& other) const;

private:
	GUIObject* m_object = nullptr;
	Array<uint32> m_indices;
};

class GUIObject : public cmg::EventSubscriber, public EventSignalListener
{
public:
	friend class Widget;
	friend class Layout;
	friend class GUIManager;

	using iterator = GUIObjectIterator;

	GUIObject();
	virtual ~GUIObject();

	const Rect2f& GetBounds() const { return m_bounds; }
	const Vector2f& GetMinSize() const { return m_minSize; }
	const Vector2f& GetMaxSize() const { return m_maxSize; }
	GUIObject* GetParent() const { return m_parent; }
	Array<GUIObject*> GetArrayOfChildren();
	virtual uint32 GetNumChildren() const;
	virtual GUIObject* GetChild(uint32 index);

	void Initialize(GUIManager* guiManager);
	void Uninitialize();
	virtual void OnInitialize() {}
	virtual void OnUninitialize() {}
	virtual void OnClose() {}
	virtual bool IsWidget() const { return false; }
	virtual bool IsVisible() const { return true; }
	virtual bool IsEnabled() const { return true; }
	void SetParent(GUIObject* parent);
	void SetBounds(const Rect2f& bounds);

	template<class T_Class, typename... T_Args>
	T_Class* AllocateObject(T_Args... args);
	void AllocateObject(GUIObject* object);
	void DeallocateObject(GUIObject* object);

	virtual void OnPress() {}
	virtual void CalcSizes() = 0;
	virtual void Update(float timeDelta) = 0;
	virtual void Render(AppGraphics& g, float timeDelta) = 0;

	GUIObjectIterator objects_begin();
	GUIObjectIterator objects_end();

	static int32 s_globalAllocationCount;

protected:
	static constexpr float DEFAULT_MIN_SIZE = 10;
	static constexpr float DEFAULT_MAX_SIZE = 16777215;

	GUIManager* m_guiManager = nullptr;
	GUIObject* m_parent = nullptr;
	Rect2f m_bounds = Rect2f::ZERO;
	Vector2f m_minSize = Vector2f(10, 10);
	Vector2f m_maxSize = Vector2f(DEFAULT_MAX_SIZE, DEFAULT_MAX_SIZE);
	Array<GUIObject*> m_allocatedObjects;
};

template<class T_Class, typename ...T_Args>
inline T_Class* GUIObject::AllocateObject(T_Args ...args)
{
	T_Class* object = new T_Class(args...);
	AllocateObject(object);
	return object;
}
