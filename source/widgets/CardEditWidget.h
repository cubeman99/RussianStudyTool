#pragma once

#include "widgets/CardSearchWidget.h"
#include "widgets/LanguageTextEdit.h"
#include "widgets/WordDefinitionWidget.h"
#include "widgets/editors/CardTagEditBox.h"


class CardEditWidget : public AppWidget
{
public:
	CardEditWidget();
	CardEditWidget(const CardData& cardData);
	CardEditWidget(Card::sptr card);

	EventSignal<Card::sptr>& CardModified() { return m_eventCardModified; }

	virtual void OnInitialize() override;
	void SelectCard(Card::sptr card);
	void SetCardData(const CardData& cardData);
	void Refresh();

private:
	void UpdateState();
	void ApplyChanges();
	void OnClickCancel();
	void OnClickDone();
	void OnClickSave();
	void OnTagsEdited();
	void OnRussianEdited();
	void OnEnglishEdited();
	void OnTypeEdited();
	void OnRussianReturnPressed();

	// State
	Card::sptr m_card;
	wiki::Term::sptr m_term;
	wiki::Word::sptr m_wikiWord;

	// Events
	EventSignal<Card::sptr> m_eventCardModified;

	// Widgets
	Label m_labelTitle;
	Label m_labelRussian;
	Label m_labelEnglish;
	Label m_labelType;
	Label m_labelCardTags;
	RussianTextEdit m_inputRussian;
	EnglishTextEdit m_inputEnglish;
	EnglishTextEdit m_inputType;
	CardTagEditBox m_tagEditBox;
	Button m_buttonDone;
	Button m_buttonSave;
	Button m_buttonCancel;
	Widget m_widgetTitle;
	WordDefinitionWidget m_wordDefinitionWidget;

	// Layouts
	HBoxLayout m_layoutTitle;
	HBoxLayout m_layoutType;
	HBoxLayout m_layoutRussian;
	HBoxLayout m_layoutEnglish;
	HBoxLayout m_layoutCardTags;
	HBoxLayout m_layoutButtons;
	VBoxLayout m_mainLayout;

	// Cached
	WordType m_wordType;
	TranslationPair m_text;
	CardRuKey m_ruKey;
	CardEnKey m_enKey;
	bool m_isModified = false;
	bool m_validType = false;
	bool m_validRussian = false;
	bool m_validEnglish = false;
	bool m_isEnKeyUnique = true;
	bool m_isRuKeyUnique = true;
};
