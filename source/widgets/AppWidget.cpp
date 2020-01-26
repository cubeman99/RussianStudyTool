#include "AppWidget.h"
#include "RussianApp.h"

AppWidget::AppWidget() :
	Widget()
{
}

RussianStudyToolApp* AppWidget::GetApp() const
{
	return RussianStudyToolApp::GetInstance();
}
