#include "CardSearchWidget.h"
#include "RussianApp.h"

CardSearchWidget::CardSearchWidget()
{
	// Create table
	auto& table = GetTable();
	table.SetSpacing(1);
	table.AddColumn("Type", 1.0f);
	table.AddColumn("Russian", 2.0f);
	table.AddColumn("English", 2.0f);
	table.AddColumn("Add", 0.0f);

	table.RowCreated().Connect(this, &CardSearchWidget::OnRowCreated);
}

uint32 CardSearchWidget::FindResults(unistr searchText,
	Array<Card::sptr>& results)
{
	unistr enSearchText = ru::ToEnglishKeyboard(searchText);
	unistr ruSearchText = ru::ToRussianKeyboard(searchText);
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
			else if (ruKey.russian.find(ruSearchText) != unistr::npos)
				matches.push_back({ruKey.russian.length(), card});
			else if (enKey.english.find(enSearchText) != unistr::npos)
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

void CardSearchWidget::OnClickCard(Card::sptr card)
{
	ItemClicked().Emit(card);
}

void CardSearchWidget::OnRowCreated(GenericTableWidget<Card::sptr>::Row& row)
{
	Card::sptr card = row.item;
	row.widgets.push_back(AllocateObject<Label>(
		EnumToString(card->GetWordType())));
	row.widgets.push_back(AllocateObject<Label>(
		card->GetRussian()));
	row.widgets.push_back(AllocateObject<Label>(
		card->GetEnglish()));
	Button* button = AllocateObject<Button>("Add");
	button->Clicked().Connect(
		this, card, &CardSearchWidget::OnClickCard);
	row.widgets.push_back(button);
}