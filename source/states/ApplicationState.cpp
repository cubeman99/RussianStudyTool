#include "ApplicationState.h"
#include "RussianApp.h"

ApplicationState::ApplicationState()
{
}

ApplicationState::~ApplicationState()
{
}

bool ApplicationState::IsActive() const
{ return m_isActive;
}

RussianStudyToolApp* ApplicationState::GetApp()
{
	return m_application;
}

Window* ApplicationState::GetWindow()
{
	return m_application->GetWindow();
}

RenderDevice* ApplicationState::GetRenderDevice()
{
	return m_application->GetRenderDevice();
}

InputManager* ApplicationState::GetInputManager()
{
	return m_application->GetInputManager();
}

cmg::ResourceManager* ApplicationState::GetResourceManager()
{
	return m_application->GetResourceManager();
}

Mouse* ApplicationState::GetMouse()
{
	return m_application->GetMouse();
}

Keyboard* ApplicationState::GetKeyboard()
{
	return m_application->GetKeyboard();
}

void ApplicationState::Begin(RussianStudyToolApp* app)
{
	m_application = app;
	m_isActive = true;
	OnBegin();
}

void ApplicationState::Update(float timeDelta)
{
	OnUpdate(timeDelta);
}

void ApplicationState::End()
{
	m_isActive = false;
	OnEnd();
}

void ApplicationState::Render(AppGraphics& g, float timeDelta)
{
	OnRender(g, timeDelta);
}
