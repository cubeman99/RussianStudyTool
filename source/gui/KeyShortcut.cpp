#include "Widget.h"
#include "Layout.h"
#include "GUIManager.h"

bool KeyShortcut::s_mapsInitialized = false;
Map<String, Keys> KeyShortcut::s_keyNames;
Map<String, KeyMods::value_type> KeyShortcut::s_modNames;


KeyShortcut::KeyShortcut(const String& pattern, Callback callback) :
	m_pattern(pattern),
	m_callback(callback)
{
	InitializeMaps();
	for (String token : cmg::string::Split(m_pattern, "+"))
	{
		cmg::string::ToLowerIP(token);
		auto itKey = s_keyNames.find(token);
		if (itKey != s_keyNames.end())
			m_key = itKey->second;
		auto itMod = s_modNames.find(token);
		if (itMod != s_modNames.end())
			m_mods |= itMod->second;
	}
}

bool KeyShortcut::Matches(Keys key,  KeyMods::value_type mods) const
{
	return (key == m_key && mods == m_mods);
}

KeyShortcut::Callback KeyShortcut::GetCallback() const
{
	return m_callback;
}

const String& KeyShortcut::GetString() const
{
	return m_pattern;
}

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