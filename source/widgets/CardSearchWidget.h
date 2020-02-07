#pragma once

#include "widgets/AppWidget.h"
#include "widgets/GenericTableWidget.h"
#include "cards/Card.h"
#include "gui/Events.h"


class CardSearchWidget : public AppWidget
{
public:
	//using FilterFunc = std::function<bool(Card::sptr card)>;
	//using FilterFunc = bool(*)(Card::sptr card);
	using FilterFunc = Delegate<bool, Card::sptr>;
	using ButtonClickedDelegate = Delegate<void, Card::sptr>;

	CardSearchWidget();

	void SetSearchText(const unistr& text);
	void RefreshResults();
	TextEdit& GetSearchInputBox();
	GenericTableWidget<Card::sptr>& GetTable();

	void AddButtonColumn(const AccentedText& name,
		Delegate<void, Card::sptr>* clickedCallback);

	void SetFilter(FilterFunc* function)
	{
		m_filterFunction = function;
	}

	void OnSearchTextChanged();
	Card::sptr GetTopResult();

private:
	struct ButtonInfo
	{
		AccentedText name;
		Delegate<void, Card::sptr>* clickedCallback;
	};
	Widget* CreateButton(ButtonInfo info, Card::sptr card);

	VBoxLayout m_mainLayout;
	HBoxLayout m_layoutSearchBox;
	HBoxLayout m_layoutResultsLabel;
	TextEdit m_inputSearch;
	AbstractScrollArea m_scrollArea;
	GenericTableWidget<Card::sptr> m_table;
	Label m_labelResultCount;
	Label m_labelResults;
	Label m_labelSearch;
	FilterFunc* m_filterFunction;
};
