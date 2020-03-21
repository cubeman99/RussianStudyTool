#pragma once

#include "gui/Layout.h"

class BoxLayout : public Layout
{
public:
	BoxLayout(uint32 axis);

	void Clear();
	void Insert(uint32 index, GUIObject* child, float stretch = 0.0f);
	void Add(GUIObject* child, float stretch = 0.0f);
	void Add(const Array<GUIObject*>& children);
	void Remove(GUIObject* child);
	void AddStretch(float stretch = 1.0f);
	void SetStretch(GUIObject* child, float stretch);

	void SetSpacing(float spacing);
	void SetMargins(float margins);

	void SetItemBackgroundColors(bool enabled);

	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes() override;
	virtual void Update(float timeDelta) override;
	virtual void Render(AppGraphics& g, float timeDelta) override;

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
};


class VBoxLayout : public BoxLayout
{
public:
	VBoxLayout();
};
