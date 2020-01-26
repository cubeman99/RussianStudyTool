#pragma once

#include "gui/gui.h"
#include "Types.h"

class RussianStudyToolApp;

class AppWidget : public Widget
{
public:
	AppWidget();

	RussianStudyToolApp* GetApp() const;

private:
};