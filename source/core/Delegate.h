#pragma once


// This is the interface for MemberFunctionHandler that each specialization will use
template<class T_Return, typename... T_Args>
class Delegate
{
public:
	Delegate() {}

	virtual T_Return Call(T_Args... args) = 0;
};


template<class T_Return, typename... T_Args>
class FunctionDelegate : public Delegate<T_Return, T_Args...>
{
public:
	using Function = T_Return (*)(T_Args...);

	FunctionDelegate(Function function) :
		m_function{function}
	{
	}

	virtual T_Return Call(T_Args... args) override
	{
		return (*m_function)(args...);
	}

private:
	// Pointer to function
	Function m_function;
};


template<class T_Instance, class T_Return, typename... T_Args>
class MethodDelegate : public Delegate<T_Return, T_Args...>
{
public:
	using Method = T_Return (T_Instance::*)(T_Args...);

	MethodDelegate(T_Instance* instance, Method method) :
		m_instance{instance},
		m_method{method}
	{
	}

	virtual T_Return Call(T_Args... args) override
	{
		return (m_instance->*m_method)(args...);
	}

private:
	// Pointer to class instance
	T_Instance* m_instance;

	// Pointer to class method
	Method m_method;
};


template<class T_Instance, class T_Capture, class T_Return, typename... T_Args>
class CaptureMethodDelegate : public Delegate<T_Return, T_Args...>
{
public:
	using Method = T_Return (T_Instance::*)(T_Capture state, T_Args... args);

	CaptureMethodDelegate(T_Instance* instance, T_Capture state, Method method) :
		m_instance{instance},
		m_method{method},
		m_state(state)
	{
	}

	CaptureMethodDelegate(T_Capture state,
		Delegate<T_Return, T_Capture, T_Args...>* function) :
		CaptureMethodDelegate(function, state,
			&Delegate<T_Return, T_Capture, T_Args...>::Call)
	{
	}

	virtual T_Return Call(T_Args... args) override
	{
		return (m_instance->*m_method)(m_state, args...);
	}

private:
	// Pointer to class instance
	T_Instance* m_instance;

	// Some captured state
	T_Capture m_state;

	// Pointer to class method
	Method m_method;
};


template<class T_Capture, class T_Return, typename... T_Args>
class CaptureFunctionDelegate : public Delegate<T_Return, T_Args...>
{
public:
	using Function = T_Return (*)(T_Capture state, T_Args... args);

	CaptureFunctionDelegate(T_Capture state, Function function) :
		m_function{function},
		m_state(state)
	{
	}

	virtual T_Return Call(T_Args... args) override
	{
		return (*m_function)(m_state, args...);
	}

private:
	// Some captured state
	T_Capture m_state;

	// Pointer to function
	Function m_function;
};