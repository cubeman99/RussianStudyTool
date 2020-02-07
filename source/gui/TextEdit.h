#pragma once

#include "gui/Widget.h"

class TextEdit : public Widget
{
public:
	TextEdit();
	TextEdit(const String& text);
	TextEdit(const char* text);
	TextEdit(const AccentedText& text);
	TextEdit(const unistr& text, Font::sptr font = nullptr);

	bool HasSelection() const;
	const unistr& GetText() const { return m_text; }

	void SetText(const unistr& text);
	void SetText(const AccentedText& text);
	void SetCursorPosition(uint32 cursorPosition);
	void SetBackgroundText(const unistr& text);
	void SetFont(Font::sptr font) { m_font = font; }
	void Deselect();
	void SelectAll();
	void DeleteSelection();
	void BeginSelection();
	void Copy();
	void Paste();
	void Cut();

	virtual void CalcSizes() override;
	virtual bool OnKeyDown(Keys key, uint32 mods) override;
	virtual bool OnKeyTyped(unichar charCode, Keys key, uint32 mods) override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

	EventSignal<>& TextEdited() { return m_textEdited; }
	EventSignal<>& ReturnPressed() { return m_returnPressed; }

private:
	unistr m_text;
	unistr m_backgroundText;
	Font::sptr m_font;
	uint32 m_cursorPosition;
	int32 m_selectPosition;
	bool m_cursorVisible;
	float m_cursorTimer;
	unistr m_textPrev;
	EventSignal<> m_textEdited;
	EventSignal<> m_returnPressed;
};
