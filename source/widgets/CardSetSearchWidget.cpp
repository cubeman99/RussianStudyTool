#include "CardSetSearchWidget.h"
#include "RussianApp.h"

CardSetSearchWidget::CardSetSearchWidget()
{
	// Create table
	auto& table = GetTable();
	table.AddColumn("Name", [](CardSet::sptr cardSet) {
		return (Widget*) new Label(cardSet->GetName());
	}, 1.0f);
	table.AddColumn("Size", [](CardSet::sptr cardSet) {
		return (Widget*) new Label(std::to_string(cardSet->GetCards().size()));
	}, 0.1f);
}

uint32 CardSetSearchWidget::FindResults(unistr searchText,
	Array<CardSet::sptr>& results)
{
	ru::ToLowerIP(searchText);
	CardSetPackage::sptr package = GetApp()->GetCardDatabase().GetRootPackage();

	uint32 resultCount = 0;
	package->IterAllCardSets([&](CardSet::sptr cardSet) {
		unistr name = cardSet->GetName().GetString();
		ru::ToLowerIP(name);
		if (name.find(searchText) != unistr::npos &&
			MatchesFilter(cardSet))
		{
			results.push_back(cardSet);
			resultCount++;
		}
	});

	return resultCount;
}
