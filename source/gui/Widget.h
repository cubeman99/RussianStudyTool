#pragma once

#include "GUIObject.h"

class Layout;

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
	//using Callback = bool (*)();
	using Callback = std::function<bool()>;

	static bool s_mapsInitialized;
	static Map<String, Keys> s_keyNames;
	static Map<String, KeyMods::value_type> s_modNames;

	static void InitializeMaps();

	KeyShortcut(const String& pattern, Callback callback) :
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

	bool Matches(Keys key,  KeyMods::value_type mods) const
	{
		return (key == m_key && mods == m_mods);
	}

	Callback GetCallback() const
	{
		return m_callback;
	}

private:
	KeyMods::value_type m_mods = 0;
	Keys m_key = Keys::none;
	String m_pattern;
	Callback m_callback = nullptr;
};


class Widget : public GUIObject
{
public:
	friend class GUIManager;

	Widget();
	virtual ~Widget();

	const AccentedText& GetWindowTitle() const { return m_windowTitle; }
	Layout* GetLayout() const { return m_layout; }
	bool IsFocused() const { return m_isFocused; }
	bool IsFocusable() const { return m_isFocusable; }

	void SetLayout(Layout* layout);
	void SetWindowTitle(const AccentedText& windowTitle) { m_windowTitle = windowTitle; }
	void SetEnabled(bool enabled) { m_isEnabled = enabled; }
	void SetVisible(bool visible) { m_isVisible = visible; }
	void SetFocusable(bool focusable) { m_isFocusable = focusable; }
	void SetBackgroundColor(const Color& backgroundColor) { m_backgroundColor = backgroundColor; }
	void Close();
	void Focus();
	void AddKeyShortcut(const String& shortcut, KeyShortcut::Callback callback);

	virtual bool OnMouseDown(MouseButtons::value_type buttons, const Vector2f& location) { return false; }
	virtual void OnMouseUp(MouseButtons::value_type buttons, const Vector2f& location) {}
	virtual void OnKeyDown(Keys key, uint32 mods) {}
	virtual void OnKeyTyped(unichar charCode, Keys key, uint32 mods) {}
	virtual void OnUpdate(float timeDelta) {}
	virtual void OnRender(AppGraphics& g, float timeDelta) {}

	virtual bool IsWidget() const override { return true; }
	virtual bool IsVisible() const override { return m_isVisible; }
	virtual bool IsEnabled() const override { return m_isEnabled; }
	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes() override;
	virtual void Update(float timeDelta) override;
	virtual void Render(AppGraphics& g, float timeDelta) override;

private:
	AccentedText m_windowTitle = "";
	Layout* m_layout = nullptr;
	bool m_isFocused = false;
	bool m_isFocusable = false;
	bool m_isEnabled = true;
	bool m_isVisible = true;
	Color m_backgroundColor = Color(0, 0, 0, 0);
	Array<KeyShortcut> m_keyShortcuts;
};