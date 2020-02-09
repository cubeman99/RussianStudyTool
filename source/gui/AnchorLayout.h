#pragma once

#include "gui/Layout.h"

struct AnchorChild
{
public:
	using sptr = cmg::shared_ptr<AnchorChild>;
	friend class AnchorLayout;

	AnchorChild& Pin(float x, float y);
	AnchorChild& Pin(float x, float y, TextAlign align);
	AnchorChild& Pin(const Vector2f& percentage);
	AnchorChild& Pin(float left, float top, float right, float bottom);
	AnchorChild& Offset(const Vector2f& offset);
	AnchorChild& Offset(float x, float y);
	AnchorChild& Offset(float left, float top, float right, float bottom);
	AnchorChild& Rect(float left, float top, float width, float height);
	AnchorChild& Rect(const Rect2f& rect);
	AnchorChild& Align(TextAlign align);

private:
	Vector2f percentMin = Vector2f::ZERO;
	Vector2f percentMax = Vector2f::ONE;
	Vector2f offsetMin = Vector2f::ZERO;
	Vector2f offsetMax = Vector2f::ZERO;
	TextAlign align = TextAlign::TOP_LEFT;
	GUIObject* object = nullptr;
};

class AnchorLayout : public Layout
{
public:
	AnchorLayout();

	void Clear();
	AnchorChild& Add(GUIObject* child);
	AnchorChild& Add(GUIObject* child, const Vector2f& position);
	AnchorChild& Add(GUIObject* child, const Vector2f& percentMin, const Vector2f& percentMax,
		const Vector2f& offset = Vector2f::ZERO, TextAlign align = TextAlign::TOP_LEFT);
	AnchorChild& Add(GUIObject* child, const Vector2f& percentMin, const Vector2f& percentMax,
		const Vector2f& offsetMin, const Vector2f& offsetMax, TextAlign align = TextAlign::TOP_LEFT);

	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes() override;
	virtual void Update(float timeDelta) override;
	virtual void Render(AppGraphics& g, float timeDelta) override;

private:

	AnchorChild::sptr Add(AnchorChild::sptr item);

	Array<AnchorChild::sptr> m_children;
};
