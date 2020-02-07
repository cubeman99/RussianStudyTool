#pragma once

#include "gui/Widget.h"

class Layout : public GUIObject
{
public:
	Layout();
	virtual ~Layout();

	virtual void CalcSizes() override;
	virtual void Update(float timeDelta) override;
	virtual void Render(AppGraphics& g, float timeDelta) override;

private:
};

