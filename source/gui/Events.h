#pragma once

#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include "AccentedText.h"
#include "Graphics.h"
#include <functional>
#include "core/Delegate.h"


template <typename... T_Args>
class EventSignal
{
public:
	EventSignal()
	{
	}
	virtual ~EventSignal()
	{
		Clear();
	}

	void Clear()
	{
		for (auto handler : m_handlers)
			delete handler;
		m_handlers.clear();
	}

	void Connect(Delegate<void, T_Args...>* function)
	{
		m_handlers.push_back(function);
	}

	template<class T_Instance>
	void Connect(T_Instance* instance, void (T_Instance::*method)(T_Args...))
	{
		Connect(new MethodDelegate<T_Instance, void, T_Args...>(
			instance, method));
	}

	template<class T_Instance, typename T_Capture>
	void Connect(T_Instance* instance, T_Capture state,
		void (T_Instance::*method)(T_Capture, T_Args...))
	{
		Connect(new CaptureMethodDelegate<T_Instance, T_Capture, void, T_Args...>(
			instance, state, method));
	}

	void Connect(void (*function)(T_Args...))
	{
		Connect(new FunctionDelegate<void, T_Args...>(function));
	}

	void Emit(T_Args... args)
	{
		for (auto handler : m_handlers)
		{
			handler->Call(args...);
		}
	}

private:
	std::list<Delegate<void, T_Args...>*> m_handlers;
};
