#include "TextEdit.h"
#include "RussianApp.h"

TextEdit::TextEdit() :
	TextEdit(u"", nullptr)
{
}

TextEdit::TextEdit(const String& text) :
	TextEdit(ConvertFromUTF8(text))
{
}

TextEdit::TextEdit(const char* text) :
	TextEdit(ConvertFromUTF8(text))
{
}

TextEdit::TextEdit(const AccentedText& text) :
	TextEdit(text.ToMarkedString())
{
}

TextEdit::TextEdit(const unistr& text, Font::sptr font) :
	m_text(text),
	m_font(font),
	m_selectPosition(-1),
	m_cursorVisible(false),
	m_cursorTimer(0.0f)
{
	SetFocusable(true);
	m_cursorPosition = text.length();

	LostFocus().Connect(this, &TextEdit::OnLoseFocus);

	SetBackgroundColor(GUIConfig::color_text_box_background);
}

bool TextEdit::HasSelection() const
{
	return (m_selectPosition >= 0 && m_selectPosition != m_cursorPosition);
}

unistr TextEdit::GetSelectedText() const
{
	if (!HasSelection())
		return u"";
	uint32 start = Math::Min((uint32) m_selectPosition, m_cursorPosition);
	uint32 end = Math::Max((uint32) m_selectPosition, m_cursorPosition);
	return m_text.substr(start, end - start);
}

void TextEdit::SetText(const unistr& text)
{
	if (m_text == text)
		return;
	m_text = text;
	m_cursorPosition = m_text.length();
	m_cursorTimer = 0.0f;
}

void TextEdit::SetText(const AccentedText& text)
{
	SetText(text.ToMarkedString());
}

void TextEdit::SetCursorPosition(uint32 cursorPosition)
{
	m_cursorPosition = Math::Min(cursorPosition, m_text.length());
}

void TextEdit::SetBackgroundText(const unistr& text)
{
	m_backgroundText = text;
}

void TextEdit::Deselect()
{
	m_selectPosition = -1;
}

void TextEdit::SelectAll()
{
	m_selectPosition = 0;
	m_cursorPosition = m_text.length();
}

void TextEdit::DeleteSelection()
{
	if (HasSelection())
	{
		uint32 start = Math::Min((uint32) m_selectPosition, m_cursorPosition);
		uint32 end = Math::Max((uint32) m_selectPosition, m_cursorPosition);
		m_text.erase(start, end - start);
		m_selectPosition = -1;
		m_cursorPosition = start;
	}
}

void TextEdit::BeginSelection()
{
	if (m_selectPosition < 0)
		m_selectPosition = m_cursorPosition;
}

void TextEdit::Copy()
{
	if (HasSelection())
		cmg::os::SetClipboardText(GetSelectedText());
}

void TextEdit::Paste()
{
	unistr clipboardText = cmg::os::GetClipboardUnicodeText();
	if (HasSelection())
		DeleteSelection();
	// Convert to-and-from accented text to standardize accent chars
	clipboardText = AccentedText(clipboardText).ToMarkedString();
	m_text.insert(m_cursorPosition, clipboardText);
	m_cursorPosition += clipboardText.length();
}

void TextEdit::Cut()
{
	if (HasSelection())
	{
		cmg::os::SetClipboardText(GetSelectedText());
		DeleteSelection();
	}
}

void TextEdit::OnLoseFocus()
{
	Deselect();
}

void TextEdit::CalcSizes()
{
	if (!m_font)
		m_font = RussianStudyToolApp::GetInstance()->GetResourceManager()->Get<Font>(Res::FONT);
	m_minSize.y = m_font->GetSize() * 2.0f;
	m_maxSize.y = m_minSize.y;
	Rect2f box = Graphics2D::MeasureString(m_font.get(), m_text);
	m_minSize.x = box.GetRight();
}

bool TextEdit::OnKeyDown(Keys key, uint32 mods)
{
	bool hasSelection = HasSelection();
	bool shift = mods & KeyMods::k_shift;
	bool ctrl = mods & KeyMods::k_control;

	if (ctrl && !shift && key == Keys::a)
	{
		SelectAll();
		m_cursorTimer = 0.0f;
	}
	else if (ctrl && !shift && key == Keys::v)
	{
		Paste();
		m_cursorTimer = 0.0f;
	}
	else if (ctrl && !shift && key == Keys::x)
	{
		Cut();
		m_cursorTimer = 0.0f;
	}
	else if (ctrl && !shift && key == Keys::c)
	{
		Copy();
	}
	else if (!shift && ctrl && key == Keys::insert)
	{
		Copy();
	}
	else if (shift && !ctrl && key == Keys::insert)
	{
		Paste();
		m_cursorTimer = 0.0f;
	}
	else if (key == Keys::k_delete)
	{
		if (hasSelection)
			DeleteSelection();
		else if (m_cursorPosition < m_text.length())
		{
			uint32 nextPos = GetNextCursorPosition(true, ctrl);
			if (nextPos > m_cursorPosition)
				m_text.erase(m_cursorPosition, nextPos - m_cursorPosition);
		}
		m_cursorTimer = 0.0f;
	}
	else if (key == Keys::backspace)
	{
		if (hasSelection)
			DeleteSelection();
		else if (m_cursorPosition > 0)
		{
			uint32 nextPos = GetNextCursorPosition(false, ctrl);
			if (nextPos < m_cursorPosition)
			{
				uint32 eraseCount = m_cursorPosition - nextPos;
				m_cursorPosition = nextPos;
				m_text.erase(m_cursorPosition, eraseCount);
			}
		}
		m_cursorTimer = 0.0f;
	}
	else if (key == Keys::right)
	{
		if (shift)
			BeginSelection();
		else
			Deselect();
		m_cursorPosition = GetNextCursorPosition(true, ctrl);
		m_cursorTimer = 0.0f;
	}
	else if (key == Keys::left)
	{
		if (shift)
			BeginSelection();
		else
			Deselect();
		m_cursorPosition = GetNextCursorPosition(false, ctrl);
		m_cursorTimer = 0.0f;
	}
	else if (key == Keys::home)
	{
		if (shift)
			BeginSelection();
		else
			Deselect();
		m_cursorPosition = 0;
		m_cursorTimer = 0.0f;
	}
	else if (key == Keys::end)
	{
		if (shift)
			BeginSelection();
		else
			Deselect();
		m_cursorPosition = m_text.length();
		m_cursorTimer = 0.0f;
	}
	else if (key == Keys::enter)
	{
		m_returnPressed.Emit();
	}
	else
	{
		return false;
	}

	return true;
}

bool TextEdit::OnKeyTyped(unichar charCode, Keys key, uint32 mods)
{
	bool ctrl = mods & KeyMods::k_control;
	bool alt = mods & KeyMods::k_alt;
	if (!ctrl && !alt && key == Keys::none)
	{
		if (HasSelection())
			DeleteSelection();
		unistr str({ charCode });
		m_text.insert(m_cursorPosition, str);
		m_cursorPosition++;
		m_cursorTimer = 0.0f;
		return true;
	}

	return false;
}

void TextEdit::OnUpdate(float timeDelta)
{
	if (m_text != m_textPrev)
		m_textEdited.Emit();
	m_textPrev = m_text;

	if (IsFocused())
	{
		m_cursorTimer += timeDelta;
		m_cursorVisible = std::fmod(m_cursorTimer, 1.0f) < 0.5f;
	}
	else
	{
		m_cursorPosition = m_text.length();
		m_cursorVisible = false;
	}
}

void TextEdit::OnRender(AppGraphics& g, float timeDelta)
{
	if (!m_font)
		m_font = RussianStudyToolApp::GetInstance()->GetResourceManager()->Get<Font>(Res::FONT);

	Vector2f anchor(0.0f, 0.5f);
	TextAlign align = TextAlign::MIDDLE_LEFT;
	Color textColor = GUIConfig::color_text;
	auto rect = GetBounds();
	Vector2f point = rect.position + anchor * rect.size;
	float startY = point.y - (GetMinSize().y * 0.5f);
	float endY = point.y + (GetMinSize().y * 0.5f);

	// Draw the selection box
	if (HasSelection())
	{
		uint32 start = Math::Min((uint32) m_selectPosition, m_cursorPosition);
		uint32 end = Math::Max((uint32) m_selectPosition, m_cursorPosition);
		unistr textBeforeStart = m_text.substr(0, start);
		unistr textBeforeEnd = m_text.substr(0, end);
		Rect2f rectBeforeStart = Graphics2D::MeasureString(m_font.get(), textBeforeStart);
		Rect2f rectBeforeEnd = Graphics2D::MeasureString(m_font.get(), textBeforeEnd);
		Rect2f selectionBox;
		selectionBox.position.x = rect.GetLeft() + rectBeforeStart.GetRight();
		selectionBox.size.x = rectBeforeEnd.GetRight() - rectBeforeStart.GetRight();
		selectionBox.position.y = startY;
		selectionBox.size.y = endY - startY;
		g.FillRect(selectionBox, GUIConfig::color_text_box_selection_background);
	}

	// Draw the text
	if (m_backgroundText.length() > 0)
		g.DrawString(m_font.get(), m_backgroundText, point,
			GUIConfig::color_text_box_background_text, align);
	if (m_text.length() > 0)
		g.DrawString(m_font.get(), m_text, point, textColor, align);

	// Draw the cursor
	if (m_cursorVisible)
	{
		unistr textBeforeCursor = m_text.substr(0, m_cursorPosition);
		Rect2f rectBeforeCursor = Graphics2D::MeasureString(m_font.get(),
			textBeforeCursor);
		float cursorX = point.x + rectBeforeCursor.GetRight();
		g.DrawLine(Vector2f(cursorX, startY),
			Vector2f(cursorX, endY), GUIConfig::color_text, 2.0f);
	}
}

uint32 TextEdit::GetNextCursorPosition(bool right, bool wordBoundary) const
{
	uint32 index;
	if (right)
		index = Math::Min(m_cursorPosition + 1, m_text.length());
	else
		index = (m_cursorPosition > 0 ? m_cursorPosition - 1 : 0);
	if (wordBoundary)
	{
		if (right)
		{
			while (index < m_text.length())
			{
				if (index > 0 && m_text[index - 1] == u' ' && m_text[index] != u' ')
					break;
				index++;
			}
		}
		else
		{
			while (index > 0)
			{
				if (m_text[index - 1] == u' ' && m_text[index] != u' ')
					break;
				index--;
			}
		}
	}
	return index;
}
