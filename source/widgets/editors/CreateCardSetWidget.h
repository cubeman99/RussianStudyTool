#pragma once

#include "widgets/CardSetBrowserWidget.h"
#include "cards/CardSetPackage.h"
#include "widgets/LanguageTextEdit.h"


class CreateCardSetWidget : public AppWidget
{
public:
	CreateCardSetWidget(const AccentedText& name,
		CardSetPackage::sptr package);

	EventSignal<CardSet::sptr>& CardSetCreated() { return m_cardSetCreated; }

	virtual void OnInitialize() override;

private:
	void UpdateState();
	void Refresh();
	void OnClickCreate();
	void OnClickCancel();
	void OnEdited();

	EventSignal<CardSet::sptr> m_cardSetCreated;

	Label m_labelName;
	Label m_labelType;
	Label m_labelFileName;
	TextEdit m_inputName;
	TextEdit m_inputFileName;
	EnglishTextEdit m_inputType;
	Button m_buttonCreate;
	Button m_buttonCancel;
	CardSetBrowserWidget m_cardSetBrowser;

	HBoxLayout m_layoutType;
	HBoxLayout m_layoutName;
	HBoxLayout m_layoutFileName;
	HBoxLayout m_layoutButtons;
	VBoxLayout m_mainLayout;

	bool m_isNewCard = false;

	// Cached
	CardSetPackage::sptr m_package;
	CardSetType m_cardSetType;
	CardSetKey m_key;
	AccentedText m_name;
	PathU16 m_path;
	bool m_validName = false;
	bool m_validType = false;
	bool m_isKeyUnique = true;
};
