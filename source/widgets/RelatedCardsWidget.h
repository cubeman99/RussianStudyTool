#pragma once

#include "widgets/CardSearchWidget.h"

class RelatedCardsWidget : public AppWidget
{
public:
	RelatedCardsWidget(Card::sptr card);

	EventSignal<Card::sptr>& CardModified() { return m_eventCardModified; }

	virtual void OnInitialize() override;
	void SelectCard(Card::sptr card);
	void Refresh();

private:
	Widget* CreateRemoveButton(Card::sptr card);
	void OnClickAdd(Card::sptr card);
	void OnClickRemove(Card::sptr card);
	void AutoAdd();
	bool SearchFilter(Card::sptr card);
	void ApplyChanges();

	Card::sptr m_card;
	EventSignal<Card::sptr> m_eventCardModified;

	VBoxLayout m_layoutLeft;
	HBoxLayout m_mainLayout;
	Button m_buttonDone;
	Label m_labelRussian;
	Label m_labelEnglish;
	Label m_labelType;
	AbstractScrollArea m_scrollArea;
	GenericTableWidget<Card::sptr> m_table;
	CardSearchWidget m_searchWidget;
};
