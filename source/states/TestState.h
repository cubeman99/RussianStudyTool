#pragma once

#include "cmgApplicationState.h"
#include "Graphics.h"
#include "Scheduler.h"
#include "gui/GUIManager.h"
#include "gui/Label.h"

class TestState : public cmg::ApplicationState<AppGraphics&>
{
public:
	using sptr = cmg::shared_ptr<StudySet>;

	TestState() {}
	~TestState() {}

	void OnBegin() override;
	void OnEnd() override;
	void OnUpdate(float timeDelta) override;
	void OnRender(AppGraphics& g, float timeDelta) override;

private:
	GUIManager m_gui;
};
