﻿#include "WordDefinitionWidget.h"
#include "RussianApp.h"

WordDefinitionWidget::WordDefinitionWidget()
{
	SetLayout(&m_layoutDefinitions);
}

const AccentedText & WordDefinitionWidget::GetWordText() const
{
	return m_wordText;
}

void WordDefinitionWidget::OnInitialize()
{
	SetWord(m_wikiWord);
}

void WordDefinitionWidget::SetWord(wiki::Word::sptr word)
{
	m_layoutDefinitions.Clear();
	m_wikiWord = word;
	if (!m_wikiWord)
		return;

	Font::sptr fontSmall = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);
		
	m_wordText = m_wikiWord->GetInfoString();
	m_layoutDefinitions.Add(AllocateObject<Label>(m_wordText, fontSmall));

	// Add definitions
	uint32 number = 1;
	const auto& wordDefinitions = m_wikiWord->GetDefinitions();
	for (const wiki::Definition& definition : wordDefinitions)
	{
		std::stringstream ss;
		ss << number << ". ";
		AccentedText text = ss.str() + definition.GetDefinition();
		m_layoutDefinitions.Add(AllocateObject<Label>(text, fontSmall));
		number++;
		if (number > 10)
		{
			Label* label = AllocateObject<Label>(
				"(+ " + std::to_string(wordDefinitions.size() - number + 1) +
				" others)", fontSmall);
			label->SetColor(GUIConfig::color_text_box_background_text);
			m_layoutDefinitions.Add(label);
			break;
		}
	}
}
