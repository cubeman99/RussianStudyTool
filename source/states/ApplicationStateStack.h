#pragma once

#include "states/ApplicationState.h"


class ApplicationStateStack : public ApplicationState
{
public:
	ApplicationStateStack();
	ApplicationStateStack(const Array<ApplicationState*>& states);
	virtual ~ApplicationStateStack();

	ApplicationState* GetTopState();

	void Push(ApplicationState* state);
	void Pop(uint32 count = 1);
	void Clear();

	virtual void OnBegin() override;
	virtual void OnEnd() override;
	virtual void OnMouseDown(Window::MouseDownEvent* e) override;
	virtual void OnMouseUp(Window::MouseUpEvent* e) override;
	virtual void OnKeyDown(Window::KeyDownEvent* e) override;
	virtual void OnKeyTyped(Window::KeyTypedEvent* e) override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

protected:
	void DeleteInactiveStates();

	Array<ApplicationState*> m_states;
};
