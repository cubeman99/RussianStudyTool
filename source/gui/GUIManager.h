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

	void GetFocusableWidgets(GUIObject* object, Array<Widget*>& outWidgets);
	Widget* CycleFocus(bool reverse = false);

	void Update(float timeDelta);
	void Render(AppGraphics& g, float timeDelta);

protected:
	Application* m_app = nullptr;
	Rect2f m_bounds;
	Widget* m_rootWidget = nullptr;
	Widget* m_focusedWidget = nullptr;
	Array<Widget*> m_focusableWidgets;
	Widget* m_cursorWidget = nullptr;
	float m_cursorPosition = 0.0f;
};
