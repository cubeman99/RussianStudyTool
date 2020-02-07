#include "CardSearchWidget.h"
#include "RussianApp.h"

CardSearchWidget::CardSearchWidget()
{
	SetBackgroundColor(GUIConfig::color_background);

	m_layoutSearchBox.Add({&m_labelSearch, &m_inputSearch});
	m_layoutResultsLabel.Add({&m_labelResults, &m_labelResultCount});
	m_scrollArea.SetWidget(&m_table);
	m_mainLayout.Add({&m_layoutSearchBox, &m_layoutResultsLabel, &m_scrollArea});

	m_layoutSearchBox.SetMargins(0.0f);
	m_layoutResultsLabel.SetMargins(0.0f);
	m_labelSearch.SetText("Search:");
	m_labelResults.SetText("Results:");
	m_labelResultCount.SetText("");

	// Create table
	m_table.AddColumn("Type", [](Card::sptr card) {
		return (Widget*) new Label(EnumToString(card->GetWordType()));
	}, 1.0f);
	m_table.AddColumn("Russian", [](Card::sptr card) {
		return (Widget*) new Label(card->GetRussian());
	}, 2.0f);
	m_table.AddColumn("English", [](Card::sptr card) {
		return (Widget*) new Label(card->GetEnglish());
	}, 2.0f);
	
	SetLayout(&m_mainLayout);

	// Connect signals
	m_inputSearch.TextEdited().Connect(
		this, &CardSearchWidget::OnSearchTextChanged);

	OnSearchTextChanged();
}

void CardSearchWidget::SetSearchText(const unistr& text)
{
	m_inputSearch.SetText(text);
}

void CardSearchWidget::RefreshResults()
{
	unistr searchText = m_inputSearch.GetText();
	ru::ToLowerIP(searchText);
	m_table.Clear();
	auto& cards = GetApp()->GetCardDatabase().GetCards();

	uint32 resultCount = 0;
	if (!searchText.empty())
	{
		for (auto it : cards)
		{
			Card::sptr card = it.second;
			CardKey key = card->GetKey();
			if (key.english.find(searchText) != unistr::npos ||
				key.russian.find(searchText) != unistr::npos)
			{
				if (!m_filterFunction || m_filterFunction->Call(card))
				{
					m_table.AddItem(card);
					resultCount++;
				}
			}
		}
	}

	m_labelResultCount.SetText(std::to_string(resultCount));
}

TextEdit& CardSearchWidget::GetSearchInputBox()
{
	return m_inputSearch;
}

GenericTableWidget<Card::sptr>& CardSearchWidget::GetTable()
{
	return m_table;
}

void CardSearchWidget::AddButtonColumn(const AccentedText& name,
	Delegate<void, Card::sptr>* clickedCallback)
{
	ButtonInfo info;
	info.name = name;
	info.clickedCallback = clickedCallback;
	m_table.AddColumn(name, new CaptureMethodDelegate<CardSearchWidget, ButtonInfo, Widget*, Card::sptr>(
		this, info, &CardSearchWidget::CreateButton), 0.5f);
}


void CardSearchWidget::OnSearchTextChanged()
{
	RefreshResults();
}

Card::sptr CardSearchWidget::GetTopResult()
{
	if (m_table.GetCount() > 0)
		return m_table.GetItem(0);
	return nullptr;
}

Widget* CardSearchWidget::CreateButton(ButtonInfo info, Card::sptr card)
{
	Button* button = new Button(info.name);
	button->Clicked().Connect(
		new CaptureMethodDelegate<Delegate<void, Card::sptr>, Card::sptr, void>(
		card, info.clickedCallback));
	return (Widget*) button;
}
