#pragma once

#include "gui/Widget.h"

class Layout : public GUIObject
{
public:
	Layout();
	virtual ~Layout();

	virtual void CalcSizes();
	virtual void Update(float timeDelta);
	virtual void Render(AppGraphics& g, float timeDelta);

private:
};

