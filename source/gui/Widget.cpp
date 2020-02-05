#include "Widget.h"
#include "Layout.h"
#include "GUIManager.h"

bool KeyShortcut::s_mapsInitialized = false;
Map<String, Keys> KeyShortcut::s_keyNames;
Map<String, KeyMods::value_type> KeyShortcut::s_modNames;

void KeyShortcut::InitializeMaps()
{
	if (s_mapsInitialized)
		return;

	s_modNames["ctrl"] = KeyMods::k_control;
	s_modNames["control"] = KeyMods::k_control;
	s_modNames["shift"] = KeyMods::k_shift;
	s_modNames["alt"] = KeyMods::k_alt;

	s_keyNames["home"] = Keys::home;
	s_keyNames["end"] = Keys::end;
	s_keyNames["insert"] = Keys::insert;
	s_keyNames["ins"] = Keys::insert;
	s_keyNames["delete"] = Keys::k_delete;
	s_keyNames["del"] = Keys::k_delete;
	s_keyNames["up"] = Keys::up;
	s_keyNames["down"] = Keys::down;
	s_keyNames["left"] = Keys::left;
	s_keyNames["right"] = Keys::right;
	s_keyNames["space"] = Keys::space;
	s_keyNames["backspace"] = Keys::backspace;
	s_keyNames["back"] = Keys::backspace;
	s_keyNames["enter"] = Keys::enter;
	s_keyNames["return"] = Keys::enter;
	s_keyNames["escape"] = Keys::escape;
	s_keyNames["esc"] = Keys::escape;
	s_keyNames["tab"] = Keys::tab;
	s_keyNames["pgup"] = Keys::page_up;
	s_keyNames["pgdown"] = Keys::page_down;
	s_keyNames["pageup"] = Keys::page_up;
	s_keyNames["pagedown"] = Keys::page_down;

	// Letters
	for (uint8 index = 0; index < 32; index++)
	{
		char c = 'a' + index;
		char name[2] = {c, '\0'};
		s_keyNames[name] = (Keys) ((uint32) Keys::a + index);
	}
	// Digits
	for (uint8 index = 0; index < 10; index++)
	{
		char c = '0' + index;
		char name[2] = {c, '\0'};
		s_keyNames[name] = (Keys) ((uint32) Keys::n0 + index);
	}

	s_mapsInitialized = true;
}


Widget::Widget()
{
}

Widget::~Widget()
{
}

void Widget::SetLayout(Layout* layout)
{
	if (m_layout)
		m_layout->SetParent(nullptr);
	m_layout = layout;
	if (m_layout)
		m_layout->SetParent(this);
}

void Widget::Close()
{
	m_isVisible = false;
}

void Widget::Focus()
{
	if (m_guiManager && m_isFocusable && !m_isFocused
		&& m_isEnabled && m_isVisible)
	{
		m_guiManager->SetFocus(this);
	}
}

void Widget::AddKeyShortcut(const String& pattern, KeyShortcut::Callback callback)
{
	m_keyShortcuts.push_back(KeyShortcut(pattern, callback));
}

uint32 Widget::GetNumChildren() const
{
	return (m_layout ? 1 : 0);
}

GUIObject* Widget::GetChild(uint32 index)
{
	CMG_ASSERT(index < 1);
	return m_layout;
}

void Widget::CalcSizes()
{
	if (m_layout)
	{
		m_minSize = m_layout->GetMinSize();
		m_maxSize = m_layout->GetMaxSize();
	}
}

void Widget::Update(float timeDelta)
{
	OnUpdate(timeDelta);

	if (m_layout)
	{
		m_layout->SetBounds(m_bounds);
		m_layout->Update(timeDelta);
	}
}

void Widget::Render(AppGraphics& g, float timeDelta)
{
	if (!m_isVisible)
		return;
	if (m_backgroundColor.a > 0)
		g.FillRect(m_bounds, m_backgroundColor);

	if (m_layout)
		m_layout->Render(g, timeDelta);
	
	OnRender(g, timeDelta);

	if (m_isFocused)
		g.DrawRect(m_bounds, GUIConfig::color_outline_focused);
	else
		g.DrawRect(m_bounds, GUIConfig::color_outline);

	if (!m_isEnabled)
	{
		g.FillRect(m_bounds, GUIConfig::color_background * 0.5f);
	}
}
