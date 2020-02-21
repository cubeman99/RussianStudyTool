#pragma once

#include "Graphics.h"
#include "cards/CardSetPackage.h"
#include "gui/gui.h"
#include "widgets/StudyProficiencyBar.h"
#include "widgets/AppWidget.h"
#include "widgets/MenuWidget.h"


class CardSetBrowserWidget : public AppWidget
{
public:
	CardSetBrowserWidget();
	CardSetBrowserWidget(CardSetPackage::sptr package);

	CardSetPackage::sptr GetPackage();
	EventSignal<CardSet::sptr>& CardSetClicked() { return m_cardSetClicked; }

	void SetPackage(CardSetPackage::sptr package,
		CardSetPackage::sptr selectPackage = nullptr);
	void NavigateIntoCardPackage(CardSetPackage::sptr package);
	void GoBack();

private:
	void OnClickCardSet(CardSet::sptr cardSet);

private:
	CardSetPackage::sptr m_package;

	VBoxLayout m_mainLayout;
	AbstractScrollArea m_scrollArea;
	Widget m_widgetOptions;
	VBoxLayout m_layoutOptions;
	HBoxLayout m_layoutPath;
	Widget m_titleWidget;
	EventSignal<CardSet::sptr> m_cardSetClicked;
};
