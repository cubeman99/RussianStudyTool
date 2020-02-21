#pragma once

#include "widgets/CardSearchWidget.h"

class CardEditWidget : public AppWidget
{
public:
	CardEditWidget(Card::sptr card);

	EventSignal<Card::sptr>& CardModified() { return m_eventCardModified; }

	virtual void OnInitialize() override;
	void SelectCard(Card::sptr card);
	void Refresh();

private:
	void UpdateState();
	void ApplyChanges();
	void OnClickCancel();
	void OnClickDone();
	void OnClickSave();
	void OnRussianEdited();
	void OnEnglishEdited();
	void OnTypeEdited();

	Card::sptr m_card;

	EventSignal<Card::sptr> m_eventCardModified;

	Label m_labelRussian;
	Label m_labelEnglish;
	Label m_labelType;
	Label m_labelCardTags;
	TextEdit m_inputRussian;
	TextEdit m_inputEnglish;
	TextEdit m_inputType;
	TextEdit m_inputCardTags;
	Button m_buttonDone;
	Button m_buttonSave;
	Button m_buttonCancel;

	HBoxLayout m_layoutType;
	HBoxLayout m_layoutRussian;
	HBoxLayout m_layoutEnglish;
	HBoxLayout m_layoutCardTags;
	HBoxLayout m_layoutButtons;
	VBoxLayout m_mainLayout;

	bool m_isNewCard = false;

	// Cached
	EnumFlags<CardTags> m_cardTags;
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
