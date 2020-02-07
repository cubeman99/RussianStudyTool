#include "RelatedCardsWidget.h"
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
	m_table.AddColumn("Type", [](Card::sptr card) {
		return (Widget*) new Label(EnumToString(card->GetWordType()));
	}, 1.0f);
	m_table.AddColumn("Russian", [](Card::sptr card) {
		return (Widget*) new Label(card->GetRussian());
	}, 2.0f);
	m_table.AddColumn("English", [](Card::sptr card) {
		return (Widget*) new Label(card->GetEnglish());
	}, 2.0f);
	m_table.AddColumn("Remove", new MethodDelegate(
		this, &RelatedCardsWidget::CreateRemoveButton), 0.5f);

	// Set up card search widget
	m_searchWidget.SetFilter(new MethodDelegate(
		this, &RelatedCardsWidget::SearchFilter));
	m_searchWidget.AddButtonColumn("Add", new MethodDelegate(
		this, &RelatedCardsWidget::OnClickAdd));

	SelectCard(card);

	// Connect signals
	m_buttonDone.Clicked().Connect((Widget*) this, &Widget::Close);
	Closed().Connect(this, &RelatedCardsWidget::ApplyChanges);
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
	m_searchWidget.GetSearchInputBox().SelectAll();
}

void RelatedCardsWidget::OnClickRemove(Card::sptr card)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	m_table.RemoveItem(card);
	m_searchWidget.RefreshResults();
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
	return !cmg::container::Contains(m_table.GetItems(), card);
}

void RelatedCardsWidget::ApplyChanges()
{
	CardData changes = m_card->GetData();
	changes.relatedCards.clear();
	for (auto card : m_table.GetItems())
		changes.relatedCards.insert(card);
	Set<Card::sptr> oldRelatedCards = m_card->GetRelatedCards();
	if (changes.relatedCards != oldRelatedCards)
	{
		CMG_LOG_INFO() << "Applying new related cards!";
		GetApp()->GetCardDatabase().ModifyCard(m_card, changes);
	}

	//Refresh();
}

Widget* RelatedCardsWidget::CreateRemoveButton(Card::sptr card)
{
	Button* button = new Button("Remove");
	button->Clicked().Connect(
		this, card, &RelatedCardsWidget::OnClickRemove);
	return (Widget*) button;
}
