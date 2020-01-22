#pragma once

#include <cmgApplication/cmg_application.h>
#include "gui/Widget.h"

class Layout;

class GUIManager
{
public:
	GUIManager();
	~GUIManager();

	void SetApplication(Application* app);
	void SetRootWidget(Widget* rootWidget);

	void Update(float timeDelta);
	void Render(AppGraphics& g, float timeDelta);

protected:
	Application* m_app = nullptr;
	Rect2f m_bounds;
	Widget* m_rootWidget;
};
