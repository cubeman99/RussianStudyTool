#pragma once

#include <cmgApplication/cmgApplication.h>

namespace cmg {

class ApplicationStateBase
{
public:
	ApplicationStateBase() {}
	virtual ~ApplicationStateBase() {}

	void Begin(Application* app);
	void Update(float timeDelta);

	virtual void OnBegin() {}
	virtual void OnEnd() {}
	virtual void OnUpdate(float timeDelta) {}

	Application* GetApp() { return m_application; }
	inline Window* GetWindow() { return m_application->GetWindow(); }
	inline RenderDevice* GetRenderDevice() { return m_application->GetRenderDevice(); }
	inline InputManager* GetInputManager() { return m_application->GetInputManager(); }
	inline cmg::ResourceManager* GetResourceManager() { return m_application->GetResourceManager(); }
	inline Mouse* GetMouse() { return m_application->GetMouse(); }
	inline Keyboard* GetKeyboard() { return m_application->GetKeyboard(); }

protected:
	Application* m_application = nullptr;
};


template <class T_RenderArg>
class ApplicationState : public ApplicationStateBase
{
public:
	ApplicationState() {}
	virtual ~ApplicationState() {}

	virtual void OnRender(T_RenderArg arg, float timeDelta) {}

	void Render(T_RenderArg arg, float timeDelta);

private:
};

template<class T_RenderArg>
inline void ApplicationState<T_RenderArg>::Render(T_RenderArg arg, float timeDelta)
{
	OnRender(arg, timeDelta);
}

};