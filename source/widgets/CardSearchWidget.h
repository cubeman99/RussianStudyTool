﻿#pragma once

#include "widgets/SearchWidget.h"
#include "cards/Card.h"


class CardSearchWidget : public SearchWidget<Card::sptr>
{
public:
	CardSearchWidget();

	virtual uint32 FindResults(unistr searchText,
		Array<Card::sptr>& results) override;
};

