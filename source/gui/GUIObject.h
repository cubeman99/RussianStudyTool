#pragma once

#include <cmgMath/cmg_math.h>
#include "AccentedText.h"
#include "Graphics.h"

class Layout;

class GUIObject : public cmg::EventSubscriber
{
public:
	friend class Widget;
	friend class Layout;
	friend class GUIManager;

	GUIObject();
	virtual ~GUIObject();

	const Rect2f& GetBounds() const { return m_bounds; }
	const Vector2f& GetMinSize() const { return m_minSize; }
	const Vector2f& GetMaxSize() const { return m_maxSize; }
	GUIObject* GetParent() const { return m_parent; }

	virtual uint32 GetNumChildren() const;
	virtual GUIObject* GetChild(uint32 index);

	virtual bool IsWidget() const { return false; }
	void SetParent(GUIObject* parent);
	void SetBounds(const Rect2f& bounds);

	virtual void CalcSizes() = 0;
	virtual void Update(float timeDelta) = 0;
	virtual void Render(AppGraphics& g, float timeDelta) = 0;

protected:
	GUIObject* m_parent = nullptr;
	Rect2f m_bounds = Rect2f::ZERO;
	Vector2f m_minSize = Vector2f(10, 10);
	Vector2f m_maxSize = Vector2f(16777215, 16777215);
};