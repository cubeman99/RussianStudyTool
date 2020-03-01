#include "CardSearchWidget.h"
#include "RussianApp.h"

CardSearchWidget::CardSearchWidget()
{
	// Create table
	auto& table = GetTable();
	table.SetSpacing(1);
	table.AddColumn("Type", [](Card::sptr card) {
		Label* label = new Label(EnumToShortString(card->GetWordType()));
		label->SetBackgroundColor(GUIConfig::color_background_alternate);
		return (Widget*) label;
	}, 1.0f);
	table.AddColumn("Russian", [](Card::sptr card) {
		Label* label = new Label(card->GetRussian());
		label->SetBackgroundColor(GUIConfig::color_background_alternate);
		return (Widget*) label;
	}, 2.0f);
	table.AddColumn("English", [](Card::sptr card) {
		Label* label = new Label(card->GetEnglish());
		label->SetBackgroundColor(GUIConfig::color_background_alternate);
		return (Widget*) label;
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
			CardRuKey ruKey = card->GetRuKey();
			CardEnKey enKey = card->GetEnKey();
			int score = 0;
			if (!MatchesFilter(card))
				continue;
			else if (ruKey.russian.find(searchText) != unistr::npos)
				matches.push_back({ruKey.russian.length(), card});
			else if (enKey.english.find(searchText) != unistr::npos)
				matches.push_back({enKey.english.length(), card});
		}
	}

	std::sort(matches.begin(), matches.end());

	for (auto it : matches)
	{
		if (results.size() > 20)
			break;
		results.push_back(it.second);
	}
	return matches.size();
}
