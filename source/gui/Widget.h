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
	bool IsEnabled() const { return m_isEnabled; }

	void SetLayout(Layout* layout);
	void SetWindowTitle(const AccentedText& windowTitle) { m_windowTitle = windowTitle; }
	void SetEnabled(bool enabled) { m_isEnabled = enabled; }
	void SetVisible(bool visible) { m_isVisible = visible; }
	void SetFocusable(bool focusable) { m_isFocusable = focusable; }
	void SetBackgroundColor(const Color& backgroundColor) { m_backgroundColor = backgroundColor; }
	void Close();

	virtual void OnKeyDown(Keys key, uint32 mods) {}
	virtual void OnKeyTyped(unichar charCode, Keys key, uint32 mods) {}
	virtual void OnUpdate(float timeDelta) {}
	virtual void OnRender(AppGraphics& g, float timeDelta) {}

	virtual bool IsWidget() const override { return true; }
	virtual bool IsVisible() const override { return m_isVisible; }
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
};