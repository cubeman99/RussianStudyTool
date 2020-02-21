#pragma once

#include "widgets/CardSetSearchWidget.h"
#include "widgets/CardSetBrowserWidget.h"


class AddCardToSetWidget : public AppWidget
{
public:
	AddCardToSetWidget(Card::sptr card);

	virtual void OnInitialize() override;
	void SelectCard(Card::sptr card);
	void Refresh();

private:
	Widget* CreateRemoveButton(CardSet::sptr card);
	void OnClickCardSet(CardSet::sptr cardSet);
	void OnClickAdd(CardSet::sptr cardSet);
	void OnClickRemove(CardSet::sptr cardSet);
	void OnClickCreateCardSet();
	void AutoAdd();
	bool SearchFilter(CardSet::sptr card);
	void ApplyChanges();

	Card::sptr m_card;

	VBoxLayout m_layoutLeft;
	VBoxLayout m_layoutRight;
	HBoxLayout m_mainLayout;
	Button m_buttonDone;
	Label m_labelRussian;
	Label m_labelEnglish;
	Label m_labelType;
	AbstractScrollArea m_scrollArea;
	GenericTableWidget<CardSet::sptr> m_table;
	CardSetSearchWidget m_searchWidget;
	CardSetBrowserWidget m_cardSetBrowser;
	Button m_buttonCreateNewCardSet;
};
