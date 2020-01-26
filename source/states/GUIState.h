#pragma once

#include "ApplicationState.h"
#include "Graphics.h"
#include "gui/gui.h"

class GUIState : public ApplicationState
{
public:
	GUIState(Widget* widget);

	GUIManager& GetGUIManager() { return m_gui; }

	void OnBegin() override;
	void OnEnd() override;
	void OnUpdate(float timeDelta) override;
	void OnRender(AppGraphics& g, float timeDelta) override;

private:
	GUIManager m_gui;
};
