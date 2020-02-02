#pragma once

#include "study/StudySetMetrics.h"
#include "widgets/AppWidget.h"


class TestWidget : public AppWidget
{
public:
	TestWidget();

private:
	HBoxLayout m_mainLayout;
	Label m_labelName;
	TextEdit m_inputName;
};
