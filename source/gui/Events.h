#pragma once

#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include "AccentedText.h"
#include "Graphics.h"
#include <functional>

// This is the interface for MemberFunctionHandler that each specialization will use
template <typename... T_Args>
class EventHandlerFunctionBase
{
public:
	virtual void Call(T_Args... args) = 0;
	virtual void* GetInstance()
	{
		return nullptr;
	}
};


template<class T_Instance, typename... T_Args>
class MemberFunctionHandler : public EventHandlerFunctionBase<T_Args...>
{
public:
	typedef void (T_Instance::*MemberFunction)(T_Args...);

	MemberFunctionHandler(T_Instance* instance, MemberFunction memberFunction) :
		m_instance{instance},
		m_memberFunction{memberFunction}
	{
	}

	virtual void Call(T_Args... args) override
	{
		(m_instance->*m_memberFunction)(args...);
	}

	void* GetInstance() override
	{
		return m_instance;
	}

private:
	// Pointer to class instance
	T_Instance* m_instance;

	// Pointer to member function
	MemberFunction m_memberFunction;
};


template<typename... T_Args>
class StaticFunctionHandler : public EventHandlerFunctionBase<T_Args...>
{
public:
	using StaticFunction = std::function<void(T_Args...)>;

	template <typename T_Function>
	StaticFunctionHandler(T_Function& function) :
		m_staticFunction(function)
	{
	}

	virtual void Call(T_Args... args) override
	{
		m_staticFunction(args...);
	}

private:
	std::function<void(T_Args...)> m_staticFunction;
};


template <typename... T_Args>
class EventSignal
{
public:
	using Function = void (*)(T_Args...);

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

	template <typename T_Function>
	void Connect(T_Function& function)
	{
		m_handlers.push_back(new StaticFunctionHandler<T_Args...>(
			function));
	}

	template<class T_Instance>
	void Connect(T_Instance& instance, void (T_Instance::*memberFunction)(T_Args...))
	{
		m_handlers.push_back(new MemberFunctionHandler<T_Instance, T_Args...>(
			&instance, memberFunction));
	}

	template<class T_Instance>
	void Connect(T_Instance* instance, void (T_Instance::*memberFunction)(T_Args...))
	{
		m_handlers.push_back(new MemberFunctionHandler<T_Instance, T_Args...>(
			instance, memberFunction));
	}

	void Emit(T_Args... args)
	{
		for (auto handler : m_handlers)
		{
			handler->Call(args...);
		}
	}

private:
	std::list<EventHandlerFunctionBase<T_Args...>*> m_handlers;
};
