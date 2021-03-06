﻿#include "RelatedCardsWidget.h"
#include "RussianApp.h"

RelatedCardsWidget::RelatedCardsWidget(Card::sptr card) :
	m_card(card)
{
	SetBackgroundColor(GUIConfig::color_background);

	m_buttonDone.SetText("Done");

	m_scrollArea.SetWidget(&m_table);
	m_layoutLeft.Add(&m_labelRussian);
	m_layoutLeft.Add(&m_labelEnglish);
	m_layoutLeft.Add(&m_labelType);
	m_layoutLeft.Add(&m_scrollArea, 1.0f);
	m_mainLayout.Add(&m_layoutLeft, 1.0f);
	m_mainLayout.Add(&m_searchWidget, 1.0f);
	SetLayout(&m_mainLayout);

	// Set up related cards table
	m_table.SetSpacing(1.0f);
	m_table.AddColumn("Type", 1.0f);
	m_table.AddColumn("Russian", 2.0f);
	m_table.AddColumn("English", 2.0f);
	m_table.AddColumn("Remove", 0.0f);

	// Set up card search widget
	m_searchWidget.SetFilter(new MethodDelegate(
		this, &RelatedCardsWidget::SearchFilter));

	// Connect signals
	m_searchWidget.ItemClicked().Connect(this, &RelatedCardsWidget::OnClickAdd);
	m_table.RowCreated().Connect(this, &RelatedCardsWidget::OnRowCreated);
	m_buttonDone.Clicked().Connect((Widget*) this, &Widget::Close);
	Closed().Connect(this, &RelatedCardsWidget::ApplyChanges);

	SelectCard(card);
}

void RelatedCardsWidget::OnInitialize()
{
	m_searchWidget.GetSearchInputBox().Focus();
	m_searchWidget.GetSearchInputBox().ReturnPressed().Connect(
		this, &RelatedCardsWidget::AutoAdd);
}

void RelatedCardsWidget::SelectCard(Card::sptr card)
{
	m_labelRussian.SetText(card->GetRussian());
	m_labelEnglish.SetText(card->GetEnglish());
	m_labelType.SetText(EnumToString(card->GetWordType()));
	Refresh();
}

void RelatedCardsWidget::Refresh()
{
	m_table.Clear();
	for (Card::sptr relatedCard : m_card->GetRelatedCards())
		m_table.AddItem(relatedCard);
	m_searchWidget.RefreshResults();
}

void RelatedCardsWidget::OnClickAdd(Card::sptr card)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	m_table.AddItem(card);
	m_searchWidget.RefreshResults();
	m_searchWidget.GetSearchInputBox().Focus();
	m_searchWidget.GetSearchInputBox().SelectAll();
}

void RelatedCardsWidget::OnClickRemove(Card::sptr card)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	m_table.RemoveItem(card);
	m_searchWidget.RefreshResults();
	m_searchWidget.GetSearchInputBox().Focus();
	m_searchWidget.GetSearchInputBox().SelectAll();
}

void RelatedCardsWidget::AutoAdd()
{
	Card::sptr topResult = m_searchWidget.GetTopResult();
	if (topResult)
		OnClickAdd(topResult);
}

bool RelatedCardsWidget::SearchFilter(Card::sptr card)
{
	return (card != m_card &&
		!cmg::container::Contains(m_table.GetItems(), card));
}

void RelatedCardsWidget::ApplyChanges()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();

	CardData changes = m_card->GetData();
	changes.relatedCards.clear();
	for (auto card : m_table.GetItems())
		changes.relatedCards.insert(card);
	Set<Card::sptr> oldRelatedCards = m_card->GetRelatedCards();
	if (changes.relatedCards != oldRelatedCards)
	{
		CMG_LOG_INFO() << "Applying new related cards!";
		cardDatabase.ModifyCard(m_card, changes);
		cardDatabase.SaveChanges();
	}

	m_eventCardModified.Emit(m_card);
}

void RelatedCardsWidget::OnRowCreated(GenericTableWidget<Card::sptr>::Row& row)
{
	Card::sptr card = row.item;
	row.widgets.push_back(AllocateObject<Label>(
		EnumToString(card->GetWordType())));
	row.widgets.push_back(AllocateObject<Label>(
		card->GetRussian()));
	row.widgets.push_back(AllocateObject<Label>(
		card->GetEnglish()));
	Button* button = AllocateObject<Button>("Remove");
	button->Clicked().Connect(
		this, card, &RelatedCardsWidget::OnClickRemove);
	row.widgets.push_back(button);
}
