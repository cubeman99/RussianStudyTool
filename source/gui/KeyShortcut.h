#pragma once

#include <cmgCore/cmg_core.h>
#include <cmgInput/cmg_input.h>
#include <functional>


struct KeyMods
{
	enum
	{
		k_shift = 1,
		k_control = 2,
		k_alt = 4,
	};
	using value_type = uint32;
};


struct KeyShortcut
{
public:
	using Callback = std::function<bool()>;

	KeyShortcut(const String& pattern, Callback callback);

	bool Matches(Keys key,  KeyMods::value_type mods) const;
	Callback GetCallback() const;
	const String& GetString() const;

private:
	static bool s_mapsInitialized;
	static Map<String, Keys> s_keyNames;
	static Map<String, KeyMods::value_type> s_modNames;

	static void InitializeMaps();

	KeyMods::value_type m_mods = 0;
	Keys m_key = Keys::none;
	String m_pattern;
	Callback m_callback = nullptr;
};
