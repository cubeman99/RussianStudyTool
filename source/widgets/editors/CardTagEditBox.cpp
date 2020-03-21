#include "CardTagEditBox.h"

CardTagEditBox::CardTagEditBox()
{
	m_mainLayout.SetSpacing(4.0f);
	m_mainLayout.SetMargins(2.0f);
	m_mainLayout.Add(&m_textEdit, 1.0f);
	SetLayout(&m_mainLayout);

	m_textEdit.ReturnPressed().Connect(this, &CardTagEditBox::OnReturnPressed);
	m_textEdit.TextEdited().Connect(this, &CardTagEditBox::OnTextEdited);
	m_textEdit.AddKeyShortcut("Ctrl+Space", [&]() { OnReturnPressed(); return true; });
}

bool CardTagEditBox::IsModified() const
{
	return (m_modifiedTags != m_tags);
}

const EnumFlags<CardTags>& CardTagEditBox::GetModifiedTags() const
{
	return m_modifiedTags;
}

const EnumFlags<CardTags>& CardTagEditBox::GetOriginalTags() const
{
	return m_tags;
}

void CardTagEditBox::SetTags(const EnumFlags<CardTags>& tags)
{
	m_tags = tags;
	bool changed = (tags != m_modifiedTags);

	if (changed)
	{
		m_mainLayout.Clear();
		if (m_readOnly)
			m_mainLayout.Add(&m_stretchWidget, 1.0f);
		else
			m_mainLayout.Add(&m_textEdit, 1.0f);
		m_tagWidgets.clear();
		m_textEdit.SetText("");
		m_modifiedTags.Clear();

		for (auto it : m_tags)
		{
			if (it.second)
				AddTagWidget(it.first);
		}
	}

	Refresh();
	if (changed)
		m_eventTagsEdited.Emit();
}

void CardTagEditBox::SetTag(CardTags tag, bool enabled)
{
	if (enabled)
	{
		if (!m_modifiedTags[tag])
		{
			AddTagWidget(tag);
			m_eventTagsEdited.Emit();
		}
	}
	else
	{
		RemoveTag(tag);
	}
}

void CardTagEditBox::ToggleTag(CardTags tag)
{
	SetTag(tag, !m_modifiedTags[tag]);
}

void CardTagEditBox::SetReadOnly(bool readOnly)
{
	if (m_readOnly == readOnly)
		return;
	m_readOnly = readOnly;
	if (readOnly)
	{
		m_textEdit.SetText("");
		m_mainLayout.Remove(&m_textEdit);
		m_mainLayout.Add(&m_stretchWidget, 1.0f);
	}
	else
	{
		m_mainLayout.Remove(&m_stretchWidget);
		m_mainLayout.Add(&m_textEdit, 1.0f);
	}
	Refresh();
}

void CardTagEditBox::OnInitialize()
{
	Refresh();
}

void CardTagEditBox::OnClickRemoveTag(CardTags tag)
{
	if (!m_readOnly)
	{
		m_textEdit.Focus();
		RemoveTag(tag);
	}
}

void CardTagEditBox::RemoveTag(CardTags tag)
{
	Widget* widget = m_tagWidgets[tag];
	m_mainLayout.Remove(widget);
	m_tagWidgets.erase(tag);
	m_modifiedTags.Set(tag, false);
	Refresh();
	m_eventTagsEdited.Emit();
}

void CardTagEditBox::OnTextEdited()
{
	Refresh();
}

void CardTagEditBox::OnReturnPressed()
{
	String text = ConvertToUTF8(m_textEdit.GetText());
	cmg::string::ToLowerIP(text);
	cmg::string::TrimIP(text);
	CardTags tag;
	bool good = (TryStringToEnum<CardTags>(text, tag, true) ||
		TryStringToEnum<CardTags>(text, tag, false));
	if (good)
	{
		AddTagWidget(tag);
		m_textEdit.SetText("");
		m_eventTagsEdited.Emit();
	}
}

Widget* CardTagEditBox::AddTagWidget(CardTags tag)
{
	if (m_tagWidgets.find(tag) == m_tagWidgets.end())
	{
		Widget* tagLabel;
		AccentedText displayName = Config::GetCardTagShortDisplayName(tag);

		if (m_readOnly)
		{
			tagLabel = AllocateObject<Label>(displayName);
		}
		else
		{
			Button* button = AllocateObject<Button>(displayName);
			button->Clicked().Connect(this, tag, &CardTagEditBox::OnClickRemoveTag);
			tagLabel = button;
		}

		Color tagColor = Config::GetCardTagColor(tag);
		tagColor = Color::Lerp(tagColor, GUIConfig::color_background, 0.4f);
		tagLabel->SetBackgroundColor(tagColor);
		m_tagWidgets[tag] = tagLabel;
		m_mainLayout.Insert(m_mainLayout.GetNumChildren() - 1, tagLabel, 0.0f);
		m_modifiedTags.Set(tag, true);
	}
	Refresh();
	return m_tagWidgets[tag];
}

void CardTagEditBox::Refresh()
{
	bool isModified = (m_modifiedTags != m_tags);

	String text = ConvertToUTF8(m_textEdit.GetText());
	cmg::string::ToLowerIP(text);
	cmg::string::TrimIP(text);
	CardTags tag;
	bool good = (TryStringToEnum<CardTags>(text, tag, true) ||
		TryStringToEnum<CardTags>(text, tag, false));

	Color color = GUIConfig::color_text_box_background;
	if (m_readOnly)
		color = Color(0, 0, 0, 0);
	else if (good)
		color = Config::k_colorEditedMatched;
	else if (isModified)
		color = Config::k_colorEditedModified;
	m_textEdit.SetBackgroundColor(Color(0, 0, 0, 0));
	SetBackgroundColor(color);
}
