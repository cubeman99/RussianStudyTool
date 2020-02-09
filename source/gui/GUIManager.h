#pragma once

#include <cmgApplication/cmg_application.h>
#include "gui/Widget.h"

class Layout;

class GUIManager : public cmg::EventSubscriber
{
public:
	GUIManager();
	~GUIManager();

	void SetApplication(Application* app);
	void SetRootWidget(Widget* rootWidget);

	Widget* GetRootWidget();
	void GetFocusableWidgets(GUIObject* object, Array<Widget*>& outWidgets);
	template <class Func>
	bool IterateObjects(GUIObject* root, Func callback);
	Widget* GetWidgetAtPoint(const Vector2f& point);
	Widget* GetWidgetAtPoint(GUIObject* object, const Vector2f& point);
	Widget* CycleFocus(bool reverse = false);
	void SetFocus(Widget* widget);

	void OnMouseDown(Window::MouseDownEvent* e);
	void OnMouseUp(Window::MouseUpEvent* e);
	bool TriggerMouseDown(GUIObject* object, Window::MouseDownEvent* e);
	void OnKeyDown(Window::KeyDownEvent* e);
	void OnKeyTyped(Window::KeyTypedEvent* e);

	void Begin();
	void End();
	void InitializeObjects(GUIObject* object);
	void UninitializeObjects(GUIObject* object);
	void Update(float timeDelta);
	void Render(AppGraphics& g, float timeDelta);

protected:
	void CalcSizes(GUIObject* object);

	Application* m_app = nullptr;
	Rect2f m_bounds;
	Widget* m_rootWidget = nullptr;
	Widget* m_focusedWidget = nullptr;
	Widget* m_cursorWidget = nullptr;
	float m_cursorPosition = 0.0f;
	Array<Widget*> m_mouseDownWidgets;

	// Cached values
	Array<Widget*> m_allWidgets;
	Array<Widget*> m_focusableWidgets;
};

template<class Func>
inline bool GUIManager::IterateObjects(GUIObject* object, Func callback)
{
	uint32 numChildren = object->GetNumChildren();
	if (callback(object))
		return true;
	for (uint32 i = 0; i < numChildren; i++)
	{
		auto child = object->GetChild(i);
		if (child && IterateObjects(child, callback))
			return true;
	}
	return false;
}
