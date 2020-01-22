#include "GUIManager.h"

GUIManager::GUIManager()
{
}

GUIManager::~GUIManager()
{
}

void GUIManager::SetApplication(Application * app)
{
	m_app = app;
}

void GUIManager::SetRootWidget(Widget* rootWidget)
{
	m_rootWidget = rootWidget;
}

void GUIManager::Update(float timeDelta)
{
	if (m_rootWidget)
	{
		if (m_app)
		{
			auto window = m_app->GetWindow();
			Vector2f windowSize(
				(float) window->GetWidth(),
				(float) window->GetHeight());
			m_bounds.position = Vector2f::ZERO;
			m_bounds.size = windowSize;
		}
		m_rootWidget->m_bounds = m_bounds;
		m_rootWidget->CalcSizes();
		m_rootWidget->Update(timeDelta);
	}
}

void GUIManager::Render(AppGraphics& g, float timeDelta)
{
	if (m_rootWidget)
	{
		m_rootWidget->Render(g, timeDelta);
	}
}
