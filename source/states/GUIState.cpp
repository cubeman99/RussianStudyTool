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

