﻿#pragma once

#include "states/ApplicationState.h"
#include "Graphics.h"
#include "gui/GUIManager.h"
#include "gui/Label.h"

class TestState : public ApplicationState
{
public:
	TestState() {}
	~TestState() {}

	void OnBegin() override;
	void OnEnd() override;
	void OnUpdate(float timeDelta) override;
	void OnRender(AppGraphics& g, float timeDelta) override;

private:
	GUIManager m_gui;
};
