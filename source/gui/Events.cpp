#include "Events.h"

int32 EventSignalBase::s_globalSignalConnectionCount = 0;

EventSignalListener::EventSignalListener()
{
}

EventSignalListener::~EventSignalListener()
{
	DisconnectFromAllSignals();
}

void EventSignalListener::AddSignalConnection(
	EventSignalBase* signal, DelegateBase* deleg)
{
	if (!cmg::container::Contains(m_connectedSignals, signal))
		m_connectedSignals.push_back(signal);

}

void EventSignalListener::DisconnectFromAllSignals()
{
	std::list<EventSignalBase*> connectedSignals = m_connectedSignals;
	for (EventSignalBase* signal : connectedSignals)
		signal->DisconnectAll(this);
	m_connectedSignals.clear();
}
