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
	using FilterFunc = EventHandlerFunctionBase<bool, Card::sptr>;

	CardSearchWidget();

	void SetSearchText(const unistr& text);
	void RefreshResults();


	template <typename T_Function>
	void SetFilter(T_Function& function)
	{
		m_filterFunction = new StaticFunctionHandler<bool, Card::sptr>(function);
	}

	void OnSearchTextChanged();
	Card::sptr GetTopResult();

private:
	VBoxLayout m_mainLayout;
	HBoxLayout m_layoutSearchBox;
	HBoxLayout m_layoutResultsLabel;
	TextEdit m_inputSearch;
	GenericTableWidget<Card::sptr> m_table;
	Label m_labelResultCount;
	Label m_labelResults;
	Label m_labelSearch;
	FilterFunc* m_filterFunction;
};
