#pragma once

#include "gui/Layout.h"

class BoxLayout : public Layout
{
public:
	BoxLayout(uint32 axis);

	void Clear();
	void Add(GUIObject* child);

	void SetSpacing(float spacing);
	void SetMargins(float margins);

	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes();
	virtual void Update(float timeDelta);
	virtual void Render(AppGraphics& g, float timeDelta);

private:
	Array<GUIObject*> m_children;
	uint32 m_axis = 1;
	Vector2f m_marginTopLeft;
	Vector2f m_marginBottomRight;
	Vector2f m_spacing;
};


class HBoxLayout : public BoxLayout
{
public:
	HBoxLayout();
};


class VBoxLayout : public BoxLayout
{
public:
	VBoxLayout();
};
