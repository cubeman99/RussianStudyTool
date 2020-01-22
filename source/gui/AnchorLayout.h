#pragma once

#include "gui/Layout.h"

class AnchorLayout : public Layout
{
public:
	AnchorLayout();

	void Clear();
	void Add(GUIObject* child, const Vector2f& position);

	virtual void CalcSizes();
	virtual void Update(float timeDelta);
	virtual void Render(AppGraphics& g, float timeDelta);

private:
	struct AnchorChild
	{
		Vector2f position;
		GUIObject* object;
	};

	void Add(const AnchorChild& item);

	Array<AnchorChild> m_children;
};
