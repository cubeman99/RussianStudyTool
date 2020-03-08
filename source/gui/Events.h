#pragma once

#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include "AccentedText.h"
#include "Graphics.h"
#include <functional>
#include "core/Delegate.h"

class EventSignalBase;

class EventSignalListener
{
public:
	template <typename... T_Args>
	friend class EventSignal;

	EventSignalListener();
	virtual ~EventSignalListener();

	void AddSignalConnection(EventSignalBase* signal, DelegateBase* deleg);
	void DisconnectFromAllSignals();

private:
	std::list<EventSignalBase*> m_connectedSignals;
};


class EventSignalBase
{
public:
	virtual void DisconnectAll(EventSignalListener* listener) = 0;

	static int32 s_globalSignalConnectionCount;
};

template <typename... T_Args>
class EventSignal : public EventSignalBase
{
public:
	using delegate_type = Delegate<void, T_Args...>;

	EventSignal()
	{
	}
	virtual ~EventSignal()
	{
		Clear();
	}

	void Clear()
	{
		Set<EventSignalListener*> listeners;
		for (auto it : m_handlers)
		{
			if (it.first)
				listeners.insert(it.first);
			// delete it.second; FIXME
		}
		s_globalSignalConnectionCount -= m_handlers.size();
		m_handlers.clear();
		for (EventSignalListener* listener : listeners)
			cmg::container::EraseIfFound(listener->m_connectedSignals, this);
	}

	template<class T_Instance>
	delegate_type* Connect(T_Instance* instance, void (T_Instance::*method)(T_Args...))
	{
		static_assert(std::is_convertible<T_Instance*, EventSignalListener*>::value,
			"T_Instance must derive from EventSignalListener");
		auto deleg = new MethodDelegate<T_Instance, void, T_Args...>(
			instance, method);
		Connect((EventSignalListener*) instance, deleg);
		return deleg;
	}

	template<class T_Instance, typename T_Capture>
	delegate_type* Connect(T_Instance* instance, T_Capture state,
		void (T_Instance::*method)(T_Capture, T_Args...))
	{
		static_assert(std::is_convertible<T_Instance*, EventSignalListener*>::value,
			"T_Instance must derive from EventSignalListener");
		auto deleg = new CaptureMethodDelegate<T_Instance, T_Capture, void, T_Args...>(
			instance, state, method);
		Connect((EventSignalListener*) instance, deleg);
		return deleg;
	}

	/*delegate_type* Connect(void (*function)(T_Args...))
	{
		auto deleg = new FunctionDelegate<void, T_Args...>(function);
		Connect(nullptr, deleg);
		return deleg;
	}*/

	void Emit(T_Args... args)
	{
		for (auto it : m_handlers)
		{
			it.second->Call(args...);
		}
	}

	void Disconnect(delegate_type* handler)
	{
		cmg::container::EraseIfFound(m_handlers, handler);
	}
	
	virtual void DisconnectAll(EventSignalListener* listener) override
	{
		for (int32 i = 0; i < (int32) m_handlers.size(); i++)
		{
			if (m_handlers[i].first == listener)
			{
				//delete m_handlers[i].second; FIXME
				s_globalSignalConnectionCount--;
				m_handlers.erase(m_handlers.begin() + i);
				i--;
			}
		}
		cmg::container::EraseIfFound(listener->m_connectedSignals, this);
	}

private:
	void Connect(EventSignalListener* listener, delegate_type* handler)
	{
		s_globalSignalConnectionCount++;
		m_handlers.push_back({listener, handler});
		if (listener)
			listener->AddSignalConnection(this, handler);
	}

	std::vector<std::pair<EventSignalListener*, delegate_type*>> m_handlers;
};

