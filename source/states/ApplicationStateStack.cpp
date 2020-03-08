#include "ApplicationStateStack.h"


ApplicationStateStack::ApplicationStateStack()
{
}
ApplicationStateStack::ApplicationStateStack(
	const Array<ApplicationState*>& states) :
	m_states(states)
{
}

ApplicationStateStack::~ApplicationStateStack()
{
}

ApplicationState* ApplicationStateStack::GetTopState()
{
	return m_states.back();
}

void ApplicationStateStack::Push(ApplicationState* state)
{
	if (IsActive())
	{
		DeleteInactiveStates();
		if (!IsActive())
			return;
	}

	m_states.push_back(state);

	if (IsActive())
	{
		state->Begin(m_application);
		DeleteInactiveStates();
	}
}

void ApplicationStateStack::Pop(uint32 count)
{
	if (IsActive())
	{
		DeleteInactiveStates();
		if (!IsActive())
			return;
	}

	for (uint32 i = 0; i < count && !m_states.empty(); i++)
	{
		ApplicationState* lastState = m_states.back();
		m_states.pop_back();
		if (IsActive())
			lastState->End();
		delete lastState;
		if (IsActive())
		{
			DeleteInactiveStates();
			if (!IsActive())
				return;
		}
	}
}

void ApplicationStateStack::Clear()
{
	while (!m_states.empty())
		Pop(1);
}

void ApplicationStateStack::OnBegin()
{
	// Begin all states from the bottom up
	for (uint32 i = 0; i < m_states.size(); i++)
	{
		m_states[i]->Begin(m_application);
		if (!m_states[i]->IsActive())
			break;
	}
	DeleteInactiveStates();
}

void ApplicationStateStack::OnEnd()
{
}

void ApplicationStateStack::OnMouseDown(Window::MouseDownEvent * e)
{
	DeleteInactiveStates();
	if (IsActive() && m_states.back() == m_currentState)
		m_states.back()->OnMouseDown(e);
}

void ApplicationStateStack::OnMouseUp(Window::MouseUpEvent * e)
{
	DeleteInactiveStates();
	if (IsActive() && m_states.back() == m_currentState)
		m_states.back()->OnMouseUp(e);
}

void ApplicationStateStack::OnKeyDown(Window::KeyDownEvent * e)
{
	DeleteInactiveStates();
	if (IsActive() && m_states.back() == m_currentState)
		m_states.back()->OnKeyDown(e);
}

void ApplicationStateStack::OnKeyTyped(Window::KeyTypedEvent * e)
{
	DeleteInactiveStates();
	if (IsActive() && m_states.back() == m_currentState)
		m_states.back()->OnKeyTyped(e);
}

void ApplicationStateStack::OnUpdate(float timeDelta)
{
	DeleteInactiveStates();
	if (!IsActive())
		return;

	// Only update the top state
	m_currentState = m_states.back();
	m_states.back()->Update(timeDelta);

	DeleteInactiveStates();
}

void ApplicationStateStack::OnRender(AppGraphics& g, float timeDelta)
{
	// Draw all states from the bottom up
	for (uint32 i = 0; i < m_states.size(); i++)
		m_states[i]->Render(g, timeDelta);
}

void ApplicationStateStack::DeleteInactiveStates()
{
	for (uint32 i = 0; i < m_states.size(); i++)
	{
		ApplicationState* state = m_states[i];
		if (!state->IsActive())
		{
			m_states.erase(m_states.begin() + i);
			state->End();
			delete state;  // FIXME: this is a bit nasty
			i--;
		}
	}
	if (m_states.empty())
		End();
}
