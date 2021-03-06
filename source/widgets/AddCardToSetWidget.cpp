﻿#include "AddCardToSetWidget.h"
#include "RussianApp.h"
#include "widgets/editors/CreateCardSetWidget.h"


AddCardToSetWidget::AddCardToSetWidget(Card::sptr card) :
	m_card(card),
	m_buttonCreateNewCardSet("Create New Card Set")
{
	SetBackgroundColor(GUIConfig::color_background);

	m_buttonDone.SetText("Done");

	m_scrollArea.SetWidget(&m_table);
	m_layoutLeft.Add(&m_labelRussian);
	m_layoutLeft.Add(&m_labelEnglish);
	m_layoutLeft.Add(&m_labelType);
	m_layoutLeft.Add(&m_scrollArea, 1.0f);
	m_layoutRight.Add(&m_searchWidget, 1.0f);
	m_layoutRight.Add(&m_cardSetBrowser, 1.0f);
	m_layoutRight.Add(&m_buttonCreateNewCardSet);
	m_mainLayout.Add(&m_layoutLeft, 1.0f);
	m_mainLayout.Add(&m_layoutRight, 1.0f);
	SetLayout(&m_mainLayout);

	// Set up related cards table
	m_table.SetSpacing(1.0f);
	m_table.AddColumn("Name", 1.0f);
	m_table.AddColumn("Size", 0.0f);
	m_table.AddColumn("Remove", 0.0f);

	// Set up card search widget
	m_searchWidget.SetFilter(new MethodDelegate(
		this, &AddCardToSetWidget::SearchFilter));

	// Connect signals
	m_table.RowCreated().Connect(this, &AddCardToSetWidget::OnRowCreated);
	m_searchWidget.ItemClicked().Connect(this, &AddCardToSetWidget::OnClickAdd);
	m_buttonDone.Clicked().Connect((Widget*) this, &Widget::Close);
	Closed().Connect(this, &AddCardToSetWidget::ApplyChanges);
	m_cardSetBrowser.CardSetClicked().Connect(
		this, &AddCardToSetWidget::OnClickCardSet);
	m_buttonCreateNewCardSet.Clicked().Connect(
		this, &AddCardToSetWidget::OnClickCreateCardSet);

	SelectCard(card);
}

void AddCardToSetWidget::OnInitialize()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	m_cardSetBrowser.SetPackage(cardDatabase.GetRootPackage());
	m_searchWidget.GetSearchInputBox().Focus();
	m_searchWidget.GetSearchInputBox().ReturnPressed().Connect(
		this, &AddCardToSetWidget::AutoAdd);
}

void AddCardToSetWidget::SelectCard(Card::sptr card)
{
	m_labelRussian.SetText(card->GetRussian());
	m_labelEnglish.SetText(card->GetEnglish());
	m_labelType.SetText(EnumToString(card->GetWordType()));
	Refresh();
}

void AddCardToSetWidget::Refresh()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	m_table.Clear();
	for (CardSet::sptr cardSet : cardDatabase.GetCardSetsWithCard(m_card))
		m_table.AddItem(cardSet);
	m_searchWidget.RefreshResults();
}

void AddCardToSetWidget::OnClickCardSet(CardSet::sptr cardSet)
{
	if (cmg::container::Contains(m_table.GetItems(), cardSet))
		m_table.RemoveItem(cardSet);
	else
		m_table.AddItem(cardSet);
	m_searchWidget.RefreshResults();
}


void AddCardToSetWidget::OnClickAdd(CardSet::sptr cardSet)
{
	m_table.AddItem(cardSet);
	m_searchWidget.RefreshResults();
	m_searchWidget.GetSearchInputBox().Focus();
	m_searchWidget.GetSearchInputBox().SelectAll();
}

void AddCardToSetWidget::OnClickRemove(CardSet::sptr cardSet)
{
	m_table.RemoveItem(cardSet);
	m_searchWidget.RefreshResults();
	m_searchWidget.GetSearchInputBox().Focus();
	m_searchWidget.GetSearchInputBox().SelectAll();
}

void AddCardToSetWidget::OnClickCreateCardSet()
{
	AccentedText name(m_searchWidget.GetSearchInputBox().GetText());
	CardSetPackage::sptr package = m_cardSetBrowser.GetPackage();

	CreateCardSetWidget* cardSetCreateWidget =
		new CreateCardSetWidget(name, package);
	cardSetCreateWidget->CardSetCreated().Connect(
		this, &AddCardToSetWidget::OnClickAdd);
	GetApp()->PushState(cardSetCreateWidget);
}

void AddCardToSetWidget::AutoAdd()
{
	CardSet::sptr topResult = m_searchWidget.GetTopResult();
	if (topResult)
		OnClickAdd(topResult);
}

bool AddCardToSetWidget::SearchFilter(CardSet::sptr cardSet)
{
	return !cmg::container::Contains(m_table.GetItems(), cardSet);
}

void AddCardToSetWidget::ApplyChanges()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	Set<CardSet::sptr> oldCardSets = cardDatabase.GetCardSetsWithCard(m_card);
	Set<CardSet::sptr> newCardSets;
	for (CardSet::sptr cardSet : m_table.GetItems())
		newCardSets.insert(cardSet);
	if (oldCardSets != newCardSets)
	{
		CMG_LOG_INFO() << "Applying new card sets!";
		for (CardSet::sptr cardSet : oldCardSets)
		{
			if (!cmg::container::Contains(newCardSets, cardSet))
				cardDatabase.RemoveCardFromSet(m_card, cardSet);
		}
		for (CardSet::sptr cardSet : newCardSets)
		{
			if (!cmg::container::Contains(oldCardSets, cardSet))
				cardDatabase.AddCardToSet(m_card, cardSet);
		}
		cardDatabase.SaveChanges();
	}
}

void AddCardToSetWidget::OnRowCreated(GenericTableWidget<CardSet::sptr>::Row& row)
{
	CardSet::sptr cardSet = row.item;
	row.widgets.push_back(AllocateObject<Label>(
		cardSet->GetName()));
	row.widgets.push_back(AllocateObject<Label>(
		std::to_string(cardSet->GetCards().size())));
	Button* button = AllocateObject<Button>("Remove");
	button->Clicked().Connect(
		this, cardSet, &AddCardToSetWidget::OnClickRemove);
	row.widgets.push_back(button);
}
