#pragma once

#include "gui/Widget.h"

class TextEdit : public Widget
{
public:
	TextEdit();
	TextEdit(const unistr& text, Font::sptr font);

	bool HasSelection() const;
	const AccentedText& GetText() const { return m_text; }

	void SetText(const unistr& text);
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
	virtual void OnKeyDown(Keys key, uint32 mods) override;
	virtual void OnKeyTyped(unichar charCode, Keys key, uint32 mods) override;
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
