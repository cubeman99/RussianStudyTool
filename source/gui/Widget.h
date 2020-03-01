#pragma once

#include "gui/GUIObject.h"
#include "gui/KeyShortcut.h"


class Layout;

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
	const Color& GetBackgroundColor() const { return m_backgroundColor; }

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
	virtual bool OnKeyDown(Keys key, uint32 mods) { return false; }
	virtual bool OnKeyTyped(unichar charCode, Keys key, uint32 mods) { return false; }
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

	EventSignal<>& Closed() { return m_eventClosed; }
	EventSignal<>& LostFocus() { return m_eventLoseFocus; }
	EventSignal<>& GainedFocus() { return m_eventGainFocus; }

private:
	AccentedText m_windowTitle = "";
	Layout* m_layout = nullptr;
	bool m_isFocused = false;
	bool m_isFocusable = false;
	bool m_isEnabled = true;
	bool m_isVisible = true;
	Color m_backgroundColor = Color(0, 0, 0, 0);
	Array<KeyShortcut> m_keyShortcuts;

	EventSignal<> m_eventClosed;
	EventSignal<> m_eventLoseFocus;
	EventSignal<> m_eventGainFocus;
};