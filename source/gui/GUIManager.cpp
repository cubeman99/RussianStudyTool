#include "GUIManager.h"
#include "RussianApp.h"

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
	if (rootWidget)
		InitializeObjects(rootWidget);
}

Widget* GUIManager::GetRootWidget()
{
	return m_rootWidget;
}

void GUIManager::GetFocusableWidgets(GUIObject* object,
	Array<Widget*>& outWidgets)
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

Widget* GUIManager::GetWidgetAtPoint(const Vector2f& point)
{
	return GetWidgetAtPoint(m_rootWidget, point);
}

Widget* GUIManager::GetWidgetAtPoint(GUIObject* object, const Vector2f& point)
{
	uint32 numChildren = object->GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		auto child = object->GetChild(i);
		if (child && child->IsEnabled() && child->IsVisible())
		{
			Widget* widget = GetWidgetAtPoint(child, point);
			if (widget)
				return widget;
		}
	}

	if (object->IsWidget() && object->IsEnabled() && object->IsVisible())
		return (Widget*) object;
	return nullptr;
}

Widget* GUIManager::CycleFocus(bool reverse)
{
	Widget* widgetToFocus = nullptr;

	if (m_focusableWidgets.empty())
		widgetToFocus = nullptr;
	else if (!m_focusedWidget)
	{
		if (reverse)
			widgetToFocus = m_focusableWidgets.back();
		else
			widgetToFocus = m_focusableWidgets.front();
	}
	else
	{
		auto it = std::find(m_focusableWidgets.begin(),
			m_focusableWidgets.end(), m_focusedWidget);
		if (it == m_focusableWidgets.end() || 
			(!reverse && it + 1 == m_focusableWidgets.end()))
		{
			widgetToFocus = m_focusableWidgets.front();
		}
		else if (reverse && it == m_focusableWidgets.begin())
		{
			widgetToFocus = m_focusableWidgets.back();
		}
		else if (reverse)
			widgetToFocus = *(it - 1);
		else
			widgetToFocus = *(it + 1);
	}

	SetFocus(widgetToFocus);
	return m_focusedWidget;
}

void GUIManager::SetFocus(Widget* widget)
{
	if (m_focusedWidget)
		m_focusedWidget->m_isFocused = false;
	m_focusedWidget = widget;
	if (m_focusedWidget)
		m_focusedWidget->m_isFocused = true;
}

void GUIManager::OnMouseDown(Window::MouseDownEvent* e)
{
	m_mouseDownWidgets.clear();
	TriggerMouseDown(m_rootWidget, e);
}

void GUIManager::OnMouseUp(Window::MouseUpEvent * e)
{
	for (Widget* widget : m_mouseDownWidgets)
		widget->OnMouseUp(e->button, Vector2f(e->location));
	m_mouseDownWidgets.clear();
}

bool GUIManager::TriggerMouseDown(GUIObject* object, Window::MouseDownEvent* e)
{
	uint32 numChildren = object->GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		auto child = object->GetChild(i);
		if (child && child->IsEnabled() && child->IsVisible())
		{
			if (TriggerMouseDown(child, e))
				return true;
		}
	}

	if (object->IsWidget() && object->IsEnabled() && object->IsVisible() &&
		object->GetBounds().Contains(Vector2f(e->location)))
	{
		Widget* widget = (Widget*) object;
		bool captured = widget->OnMouseDown(e->button, Vector2f(e->location));
		if (widget->IsFocusable())
		{
			SetFocus(widget);
			captured = true;
		}
		m_mouseDownWidgets.push_back(widget);
		return captured;
	}

	return false;
}

void GUIManager::OnKeyDown(Window::KeyDownEvent* e)
{
	if (m_focusedWidget)
	{
		uint32 mods = 0;
		auto keyboard = m_app->GetKeyboard();
		if (keyboard->IsKeyDown(Keys::left_shift) || keyboard->IsKeyDown(Keys::right_shift))
			mods |= KeyMods::k_shift;
		if (keyboard->IsKeyDown(Keys::left_control) || keyboard->IsKeyDown(Keys::right_control))
			mods |= KeyMods::k_control;
		if (keyboard->IsKeyDown(Keys::left_alt) || keyboard->IsKeyDown(Keys::right_alt))
			mods |= KeyMods::k_alt;
		m_focusedWidget->OnKeyDown(e->key, mods);
	}
}

void GUIManager::OnKeyTyped(Window::KeyTypedEvent* e)
{
	if (m_focusedWidget)
	{
		uint32 mods = 0;
		auto keyboard = m_app->GetKeyboard();
		if (keyboard->IsKeyDown(Keys::left_shift) || keyboard->IsKeyDown(Keys::right_shift))
			mods |= KeyMods::k_shift;
		if (keyboard->IsKeyDown(Keys::left_control) || keyboard->IsKeyDown(Keys::right_control))
			mods |= KeyMods::k_control;
		if (keyboard->IsKeyDown(Keys::left_alt) || keyboard->IsKeyDown(Keys::right_alt))
			mods |= KeyMods::k_alt;
		m_focusedWidget->OnKeyTyped(e->keyCharUTF32, e->key, mods);
	}
}

void GUIManager::Begin()
{
	m_app->GetEventManager()->Subscribe(this, &GUIManager::OnKeyTyped);
	m_app->GetEventManager()->Subscribe(this, &GUIManager::OnKeyDown);
	m_app->GetEventManager()->Subscribe(this, &GUIManager::OnMouseDown);
	m_app->GetEventManager()->Subscribe(this, &GUIManager::OnMouseUp);
}

void GUIManager::End()
{
	//m_app->GetEventManager()->UnsubscribeAll(this);
}

void GUIManager::InitializeObjects(GUIObject* object)
{
	object->m_guiManager = this;
	IterateObjects(object, [this](GUIObject* childObject) {
		childObject->m_guiManager = this;
		return false;
	});
}

void GUIManager::UninitializeObjects(GUIObject* object)
{
	// TODO: GUIObject garbage collection?
	// Would need to know how it was allocated.

	// Move focus if focus object was removed
	if (m_focusedWidget == object)
	{
		m_focusedWidget->m_isFocused = false;
		GetFocusableWidgets(m_rootWidget, m_focusableWidgets);
		CycleFocus(false);
	}
}

void GUIManager::Update(float timeDelta)
{
	if (m_rootWidget)
	{
		m_focusableWidgets.clear();
		GetFocusableWidgets(m_rootWidget, m_focusableWidgets);
		if (m_focusedWidget && std::find(m_focusableWidgets.begin(),
			m_focusableWidgets.end(), m_focusedWidget) == m_focusableWidgets.end())
			m_focusedWidget = nullptr;
		if (m_cursorWidget && std::find(m_focusableWidgets.begin(),
			m_focusableWidgets.end(), m_cursorWidget) == m_focusableWidgets.end())
			m_cursorWidget = nullptr;

		if (m_app)
		{
			auto window = m_app->GetWindow();
			Vector2f windowSize(
				(float) window->GetWidth(),
				(float) window->GetHeight());
			m_bounds.position = Vector2f::ZERO;
			m_bounds.size = windowSize;

			auto app = RussianStudyToolApp::GetInstance();
			Keyboard* keyboard = app->GetKeyboard();
			Joystick* joystick = app->GetJoystick();
			const PedalInput& leftPedal = app->GetLeftPedalInput();
			const PedalInput& rightPedal = app->GetRightPedalInput();
			const PedalInput& middlePedal = app->GetMiddlePedalInput();
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
			if (keyboard->IsKeyPressed(Keys::enter) ||
				middlePedal.IsPressed())
			{
				if (m_focusedWidget)
				{
					m_focusedWidget->OnPress();
				}
			}

			float move = 0.0f;
			float speed = 10.0f;
			move += rightPedal.GetAmount();
			move -= leftPedal.GetAmount();
			if (keyboard->IsKeyDown(Keys::down))
				move += 1.0f;
			if (keyboard->IsKeyDown(Keys::up))
				move -= 1.0f;
			move = Math::Sign(move) * Math::Abs(move);
			if (Math::Abs(move) > 0.001f)
			{
				if (!m_cursorWidget)
					m_cursorPosition = 0.0f;
				m_cursorPosition += move * timeDelta * speed;
				if (!m_focusedWidget)
					CycleFocus(false);
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
		CalcSizes(m_rootWidget);
		m_rootWidget->m_bounds = m_bounds;
		m_rootWidget->Update(timeDelta);
	}
}

void GUIManager::Render(AppGraphics& g, float timeDelta)
{
	if (m_rootWidget)
	{
		m_rootWidget->Render(g, timeDelta);
	}

	if (m_cursorWidget)
	{
		float fract = m_cursorPosition - Math::Floor(m_cursorPosition);
		Rect2f bounds = m_cursorWidget->GetBounds();
		int axis = 1;
		Vector2f a, b;
		a[axis] = bounds.position[axis] + fract * bounds.size[axis];
		b[axis] = a[axis];
		a[1 - axis] = bounds.position[1 - axis];
		b[1 - axis] = bounds.GetBottomRight()[1 - axis];
		g.DrawLine(a, b, GUIConfig::color_outline_focused, 2.0f);
	}
}

void GUIManager::CalcSizes(GUIObject* object)
{
	uint32 numChildren = object->GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		GUIObject* child = object->GetChild(i);
		if (child)
			CalcSizes(child);
	}
	object->CalcSizes();
}
