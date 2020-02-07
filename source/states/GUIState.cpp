#include "GUIState.h"
#include "Resources.h"
#include "gui/gui.h"
#include "RussianApp.h"

GUIState::GUIState(Widget* widget)
{
	m_gui.SetRootWidget(widget);
}

void GUIState::OnBegin()
{
	m_gui.SetApplication(GetApp());
	m_gui.Begin();
}

void GUIState::OnEnd()
{
	m_gui.End();
}

void GUIState::OnMouseDown(Window::MouseDownEvent * e)
{
	m_gui.OnMouseDown(e);
}

void GUIState::OnMouseUp(Window::MouseUpEvent * e)
{
	m_gui.OnMouseUp(e);
}

void GUIState::OnKeyDown(Window::KeyDownEvent * e)
{
	m_gui.OnKeyDown(e);
}

void GUIState::OnKeyTyped(Window::KeyTypedEvent * e)
{
	m_gui.OnKeyTyped(e);
}

void GUIState::OnUpdate(float timeDelta)
{
	if (!m_gui.GetRootWidget() || !m_gui.GetRootWidget()->IsVisible())
	{
		End();
		return;
	}

	m_gui.Update(timeDelta);

	if (!m_gui.GetRootWidget() || !m_gui.GetRootWidget()->IsVisible())
	{
		End();
		return;
	}
}

void GUIState::OnRender(AppGraphics& g, float timeDelta)
{
	m_gui.Render(g, timeDelta);
}

