#pragma once

#include "widgets/AppWidget.h"
#include "widgets/GenericTableWidget.h"
#include "cards/Card.h"


class CardSearchWidget : public AppWidget
{
public:
	CardSearchWidget();

	void OnSearchTextChanged();

private:
	VBoxLayout m_mainLayout;
	HBoxLayout m_layoutSearchBox;
	HBoxLayout m_layoutResultsLabel;
	TextEdit m_inputSearch;
	GenericTableWidget<Card::sptr> m_table;
	Label m_labelResultCount;
	Label m_labelResults;
	Label m_labelSearch;
};
