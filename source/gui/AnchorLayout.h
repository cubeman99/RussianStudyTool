#pragma once

#include "gui/Layout.h"

class AnchorLayout : public Layout
{
public:
	AnchorLayout();

	void Clear();
	void Add(GUIObject* child);
	void Add(GUIObject* child, const Vector2f& position);
	void Add(GUIObject* child, const Vector2f& percentMin, const Vector2f& percentMax,
		const Vector2f& offset = Vector2f::ZERO, TextAlign align = TextAlign::TOP_LEFT);
	void Add(GUIObject* child, const Vector2f& percentMin, const Vector2f& percentMax,
		const Vector2f& offsetMin, const Vector2f& offsetMax, TextAlign align = TextAlign::TOP_LEFT);

	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes() override;
	virtual void Update(float timeDelta) override;
	virtual void Render(AppGraphics& g, float timeDelta) override;

private:
	struct AnchorChild
	{
		Vector2f percentMin = Vector2f::ZERO;
		Vector2f percentMax = Vector2f::ONE;
		Vector2f offsetMin = Vector2f::ZERO;
		Vector2f offsetMax = Vector2f::ZERO;
		TextAlign align = TextAlign::TOP_LEFT;
		GUIObject* object = nullptr;
	};

	void Add(const AnchorChild& item);

	Array<AnchorChild> m_children;
};
