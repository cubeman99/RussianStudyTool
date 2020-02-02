#include "TestWidget.h"

TestWidget::TestWidget() :
	m_mainLayout({&m_labelName, &m_inputName})
{
	m_labelName.SetText("This is a label");
	SetLayout(&m_mainLayout);
}

