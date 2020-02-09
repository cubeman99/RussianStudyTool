﻿#pragma once

#include "widgets/CardSetSearchWidget.h"

class AddCardToSetWidget : public AppWidget
{
public:
	AddCardToSetWidget(Card::sptr card);

	virtual void OnInitialize() override;
	void SelectCard(Card::sptr card);
	void Refresh();

private:
	Widget* CreateRemoveButton(CardSet::sptr card);
	void OnClickAdd(CardSet::sptr card);
	void OnClickRemove(CardSet::sptr card);
	void AutoAdd();
	bool SearchFilter(CardSet::sptr card);
	void ApplyChanges();

	Card::sptr m_card;

	VBoxLayout m_layoutLeft;
	HBoxLayout m_mainLayout;
	Button m_buttonDone;
	Label m_labelRussian;
	Label m_labelEnglish;
	Label m_labelType;
	AbstractScrollArea m_scrollArea;
	GenericTableWidget<CardSet::sptr> m_table;
	CardSetSearchWidget m_searchWidget;
};
