#pragma once

#include "states/ApplicationState.h"


class ApplicationStateStack : public ApplicationState
{
public:
	ApplicationStateStack();
	ApplicationStateStack(const Array<ApplicationState*>& states);
	virtual ~ApplicationStateStack();

	void Push(ApplicationState* state);
	void Pop(uint32 count = 1);
	void Clear();

	virtual void OnBegin();
	virtual void OnEnd();
	virtual void OnUpdate(float timeDelta);
	virtual void OnRender(AppGraphics& g, float timeDelta);

protected:
	void DeleteInactiveStates();

	Array<ApplicationState*> m_states;
};
