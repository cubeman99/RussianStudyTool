#pragma once

#include "gui/Layout.h"

class BoxLayout : public Layout
{
public:
	BoxLayout(uint32 axis);

	void Clear();
	void Add(GUIObject* child, float stretch = 0.0f);
	void AddStretch(float stretch = 1.0f);

	void SetSpacing(float spacing);
	void SetMargins(float margins);

	void SetItemBackgroundColors(bool enabled);

	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes();
	virtual void Update(float timeDelta);
	virtual void Render(AppGraphics& g, float timeDelta);

private:
	struct BoxChild
	{
		float stretch = 0.0f;
		GUIObject* object = nullptr;

		// used internally for layout
		float offset = 0.0f;
		float size = 0.0f;
		bool isMaxed = false;
		bool isMinned = false;
	};

	bool m_showBackgroundColors = false;
	Array<BoxChild> m_children;
	uint32 m_axis = 1;
	Vector2f m_marginTopLeft;
	Vector2f m_marginBottomRight;
	Vector2f m_spacing;
};


class HBoxLayout : public BoxLayout
{
public:
	HBoxLayout();

	template <class... T>
	HBoxLayout(const Array<GUIObject*>& children) :
		HBoxLayout()
	{
		for (GUIObject* child : children)
			Add(child);
	}
};


class VBoxLayout : public BoxLayout
{
public:
	VBoxLayout();

	template <class... T>
	VBoxLayout(const Array<GUIObject*>& children) :
		VBoxLayout()
	{
		for (GUIObject* child : children)
			Add(child);
	}
};
