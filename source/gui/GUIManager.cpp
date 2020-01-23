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

void GUIManager::GetFocusableWidgets(GUIObject* object, Array<Widget*>& outWidgets)
{
	if (object->IsWidget())
	{
		Widget* widget = (Widget*) object;
		if (widget->IsEnabled() && widget->IsFocusable())
			outWidgets.push_back(widget);
	}

	uint32 numChildren = object->GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		auto child = object->GetChild(i);
		if (child)
			GetFocusableWidgets(child, outWidgets);
	}
}

Widget* GUIManager::CycleFocus(bool reverse)
{
	if (m_focusedWidget)
		m_focusedWidget->m_isFocused = false;

	if (m_focusableWidgets.empty())
		m_focusedWidget = nullptr;
	else if (!m_focusedWidget)
	{
		if (reverse)
			m_focusedWidget = m_focusableWidgets.back();
		else
			m_focusedWidget = m_focusableWidgets.front();
	}
	else
	{
		auto it = std::find(m_focusableWidgets.begin(),
			m_focusableWidgets.end(), m_focusedWidget);
		if (it == m_focusableWidgets.end() || 
			(!reverse && it + 1 == m_focusableWidgets.end()))
		{
			m_focusedWidget = m_focusableWidgets.front();
		}
		else if (reverse && it == m_focusableWidgets.begin())
		{
			m_focusedWidget = m_focusableWidgets.back();
		}
		else if (reverse)
			m_focusedWidget = *(it - 1);
		else
			m_focusedWidget = *(it + 1);
	}

	if (m_focusedWidget)
		m_focusedWidget->m_isFocused = true;

	return m_focusedWidget;
}

void GUIManager::Update(float timeDelta)
{

	if (m_rootWidget)
	{
		m_focusableWidgets.clear();
		GetFocusableWidgets(m_rootWidget, m_focusableWidgets);

		if (m_app)
		{
			auto window = m_app->GetWindow();
			Vector2f windowSize(
				(float) window->GetWidth(),
				(float) window->GetHeight());
			m_bounds.position = Vector2f::ZERO;
			m_bounds.size = windowSize;

			Keyboard* keyboard = m_app->GetKeyboard();
			bool ctrl = keyboard->IsKeyDown(Keys::left_control) ||
				keyboard->IsKeyDown(Keys::right_control);
			bool shift = keyboard->IsKeyDown(Keys::left_shift) ||
				keyboard->IsKeyDown(Keys::right_shift);
			bool tab = keyboard->IsKeyPressed(Keys::tab);

			if (ctrl && tab)
			{
				CycleFocus(false);
			}
			else if (ctrl && shift && tab)
			{
				CycleFocus(true);
			}

			float move = 0.0f;
			float speed = 20.0f;
			if (keyboard->IsKeyDown(Keys::down))
				move += 1.0f;
			if (keyboard->IsKeyDown(Keys::up))
				move -= 1.0f;
			if (Math::Abs(move) > 0.001f)
			{
				if (!m_cursorWidget)
					m_cursorPosition = 0.5f;
				m_cursorPosition += move * timeDelta * speed;
				if (!m_focusedWidget)
					CycleFocus(true);
				m_cursorWidget = m_focusedWidget;

				if (m_cursorPosition > 1.0f)
				{
					m_cursorPosition -= 1.0f;
					m_cursorWidget = CycleFocus(false);
				}
				else if (m_cursorPosition < 0.0f)
				{
					m_cursorPosition += 1.0f;
					m_cursorWidget = CycleFocus(true);
				}
			}

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
