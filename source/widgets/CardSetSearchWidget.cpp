#include "CardSetSearchWidget.h"
#include "RussianApp.h"

CardSetSearchWidget::CardSetSearchWidget()
{
	// Create table
	auto& table = GetTable();
	table.AddColumn("Name", 1.0f);
	table.AddColumn("Size", 0.1f);
	table.AddColumn("Add", 0.0f);

	table.RowCreated().Connect(this, &CardSetSearchWidget::OnRowCreated);
}

uint32 CardSetSearchWidget::FindResults(unistr searchText,
	Array<CardSet::sptr>& results)
{
	unistr enSearchText = ru::ToEnglishKeyboard(searchText);
	unistr ruSearchText = ru::ToRussianKeyboard(searchText);
	ru::ToLowerIP(searchText);
	CardSetPackage::sptr package = GetApp()->GetCardDatabase().GetRootPackage();
	Array<std::pair<uint32, CardSet::sptr>> matches;

	uint32 resultCount = 0;
	package->IterAllCardSets([&](CardSet::sptr cardSet) {
		unistr name = cardSet->GetName().GetString();
		ru::ToLowerIP(name);
		if ((name.find(searchText) != unistr::npos ||
			name.find(enSearchText) != unistr::npos ||
			name.find(ruSearchText) != unistr::npos) &&
			MatchesFilter(cardSet))
		{
			matches.push_back({name.length(), cardSet});
		}
	});

	std::sort(matches.begin(), matches.end());
	for (auto it : matches)
	{
		if (results.size() > 20)
			break;
		results.push_back(it.second);
	}

	return matches.size();
}

void CardSetSearchWidget::OnClickCardSet(CardSet::sptr cardSet)
{
	ItemClicked().Emit(cardSet);
}

void CardSetSearchWidget::OnRowCreated(GenericTableWidget<CardSet::sptr>::Row& row)
{
	CardSet::sptr cardSet = row.item;
	row.widgets.push_back(AllocateObject<Label>(
		cardSet->GetName()));
	row.widgets.push_back(AllocateObject<Label>(
		std::to_string(cardSet->GetCards().size())));
	Button* button = AllocateObject<Button>("Add");
	button->Clicked().Connect(
		this, cardSet, &CardSetSearchWidget::OnClickCardSet);
	row.widgets.push_back(button);
}
