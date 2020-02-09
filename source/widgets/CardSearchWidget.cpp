#include "CardSearchWidget.h"
#include "RussianApp.h"

CardSearchWidget::CardSearchWidget()
{
	// Create table
	auto& table = GetTable();
	table.AddColumn("Type", [](Card::sptr card) {
		return (Widget*) new Label(EnumToString(card->GetWordType()));
	}, 1.0f);
	table.AddColumn("Russian", [](Card::sptr card) {
		return (Widget*) new Label(card->GetRussian());
	}, 2.0f);
	table.AddColumn("English", [](Card::sptr card) {
		return (Widget*) new Label(card->GetEnglish());
	}, 2.0f);
}

uint32 CardSearchWidget::FindResults(unistr searchText,
	Array<Card::sptr>& results)
{
	ru::ToLowerIP(searchText);
	std::replace(searchText.begin(), searchText.end(), u'ё', u'е');
	auto& cards = GetApp()->GetCardDatabase().GetCards();
	
	Array<std::pair<uint32, Card::sptr>> matches;
	if (!searchText.empty())
	{
		for (auto it : cards)
		{
			Card::sptr card = it.second;
			CardKey key = card->GetKey();
			int score = 0;
			if (!MatchesFilter(card))
				continue;
			else if (key.russian.find(searchText) != unistr::npos)
				matches.push_back({key.russian.length(), card});
			else if (key.english.find(searchText) != unistr::npos)
				matches.push_back({key.english.length(), card});
		}
	}

	std::sort(matches.begin(), matches.end());

	for (auto it : matches)
		results.push_back(it.second);
	return matches.size();
}
