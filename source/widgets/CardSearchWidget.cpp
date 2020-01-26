#include "CardSearchWidget.h"

CardSearchWidget::CardSearchWidget()
{
	m_mainLayout.Add(&m_textEdit);
	SetLayout(&m_mainLayout);
}
