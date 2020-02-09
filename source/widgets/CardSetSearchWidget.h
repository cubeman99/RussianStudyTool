#pragma once

#include "widgets/SearchWidget.h"
#include "cards/CardSet.h"


class CardSetSearchWidget : public SearchWidget<CardSet::sptr>
{
public:
	CardSetSearchWidget();

	virtual uint32 FindResults(unistr searchText,
		Array<CardSet::sptr>& results) override;
};

