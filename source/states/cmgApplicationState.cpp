#include "cmgApplicationState.h"

namespace cmg
{

void ApplicationStateBase::Begin(Application * app)
{
	m_application = app;
	OnBegin();
}

void ApplicationStateBase::Update(float timeDelta)
{
	OnUpdate(timeDelta);
}

}
