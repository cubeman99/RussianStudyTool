#include "CardSearchWidget.h"
#include "RussianApp.h"

CardSearchWidget::CardSearchWidget() :
	m_layoutSearchBox({&m_labelSearch, &m_inputSearch}),
	m_layoutResultsLabel({&m_labelResults, &m_labelResultCount})
{
	SetBackgroundColor(GUIConfig::color_background);

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
	m_table.AddColumn("Add", [](Card::sptr card) {
		Button* button = new Button("Add");
		//button->Clicked().Connect()
		return (Widget*) button;
	}, 0.5f);
	
	// Create layouts
	m_mainLayout.Add(&m_layoutSearchBox);
	m_mainLayout.Add(&m_layoutResultsLabel);
	m_mainLayout.Add(&m_table);
	SetLayout(&m_mainLayout);

	// Connect signals
	m_inputSearch.TextEdited().Connect([this]() {
		OnSearchTextChanged();
	});

	OnSearchTextChanged();
}

void CardSearchWidget::OnSearchTextChanged()
{
	unistr searchText = m_inputSearch.GetText();
	ru::ToLowerIP(searchText);
	m_table.Clear();
	auto& cards = GetApp()->GetCardDatabase().GetCards();

	uint32 resultCount = 0;
	for (auto it : cards)
	{
		Card::sptr card = it.second;
		CardKey key = card->GetKey();
		if (key.english.find(searchText) != unistr::npos ||
			key.russian.find(searchText) != unistr::npos)
		{
			m_table.AddItem(card);
			resultCount++;
		}
	}

	m_labelResultCount.SetText(std::to_string(resultCount));
}
