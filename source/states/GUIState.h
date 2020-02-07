#pragma once

#include "ApplicationState.h"
#include "Graphics.h"
#include "gui/gui.h"

class GUIState : public ApplicationState
{
public:
	GUIState(Widget* widget);

	GUIManager& GetGUIManager() { return m_gui; }

	virtual void OnBegin() override;
	virtual void OnEnd() override;
	virtual void OnMouseDown(Window::MouseDownEvent* e) override;
	virtual void OnMouseUp(Window::MouseUpEvent* e) override;
	virtual void OnKeyDown(Window::KeyDownEvent* e) override;
	virtual void OnKeyTyped(Window::KeyTypedEvent* e) override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	GUIManager m_gui;
};
