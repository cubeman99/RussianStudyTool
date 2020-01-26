#pragma once

#include <cmgApplication/cmgApplication.h>
#include "Graphics.h"

class RussianStudyToolApp;

class ApplicationState
{
public:
	ApplicationState();
	virtual ~ApplicationState();

	bool IsActive() const;
	RussianStudyToolApp* GetApp();
	Window* GetWindow();
	RenderDevice* GetRenderDevice();
	InputManager* GetInputManager();
	cmg::ResourceManager* GetResourceManager();
	Mouse* GetMouse();
	Keyboard* GetKeyboard();

	void Begin(RussianStudyToolApp* app);
	void End();
	void Update(float timeDelta);
	void Render(AppGraphics& g, float timeDelta);

	virtual void OnBegin() {}
	virtual void OnEnd() {}
	virtual void OnUpdate(float timeDelta) {}
	virtual void OnRender(AppGraphics& g, float timeDelta) {}

protected:
	bool m_isActive = false;
	RussianStudyToolApp* m_application = nullptr;
};
