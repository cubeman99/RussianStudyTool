#pragma once

#include "widgets/AppWidget.h"


class CardSearchWidget : public AppWidget
{
public:
	CardSearchWidget();

private:
	VBoxLayout m_mainLayout;
	TextEdit m_textEdit;
};
