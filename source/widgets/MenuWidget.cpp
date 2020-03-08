#include "MenuWidget.h"
#include "RussianApp.h"


MenuItemWidget::MenuItemWidget(const AccentedText& name)
{
	SetFocusable(true);

	m_labelName.SetText(name);
	m_labelName.SetAlign(TextAlign::MIDDLE_LEFT);
	m_labelName.SetColor(GUIConfig::color_text);

	m_layout.Add(&m_labelName);

	/*
	m_layout.Add(&m_labelName,
		Vector2f(0.0f, 0.5f), Vector2f(0.0f, 0.5f),
		Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), TextAlign::MIDDLE_LEFT);
	m_layout.Add(&m_proficiencyBar,
		Vector2f(0.0f, 1.0f), Vector2f(0.0f, 1.0f),
		Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), TextAlign::MIDDLE_RIGHT);*/
	SetLayout(&m_layout);
}


MenuWidget::MenuWidget(const AccentedText& title)
{
	SetWindowTitle(title);

	m_labelTitle.SetAlign(TextAlign::CENTERED);
	m_labelTitle.SetText(title);
	m_titleLayout.Add(&m_labelTitle);
	m_titleWidget.SetLayout(&m_titleLayout);

	m_optionLayout.SetItemBackgroundColors(true);

	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.Add(&m_titleWidget, 0.1f);
	m_mainLayout.Add(&m_optionLayout, 1.0f);
	m_widgetWindow.SetLayout(&m_mainLayout);
	m_anchorLayout.Add(&m_widgetWindow,
		Vector2f(0.2f, 0.2f), Vector2f(0.8f, 0.8f),
		Vector2f::ZERO, Vector2f::ZERO, TextAlign::CENTERED);
	SetLayout(&m_anchorLayout);

	m_widgetWindow.SetBackgroundColor(GUIConfig::color_background);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	SetBackgroundColor(Color::BLACK * 0.5f);
}

void MenuWidget::Clear()
{
	m_optionLayout.Clear();
	for (auto option : m_options)
		DeallocateObject(option);
	m_options.clear();
}

MenuItemWidget* MenuWidget::AddMenuOption(const AccentedText& name)
{
	MenuItemWidget* option = AllocateObject<MenuItemWidget>(name);
	m_options.push_back(option);
	m_optionLayout.Add(option);
	return option;
}

MenuItemWidget* MenuWidget::AddCancelOption(const AccentedText & name)
{
	MenuItemWidget* option = AddMenuOption(name);
	option->Clicked().Connect(this, &MenuWidget::Cancel);
	return option;
}

void MenuWidget::OnInitialize()
{
	m_options[0]->Focus();
}

void MenuWidget::OnUpdate(float timeDelta)
{
}

void MenuWidget::OnRender(AppGraphics& g, float timeDelta)
{
}

void MenuWidget::OnSelectOption(MenuItemWidget* option)
{
	if (option->m_closeMenu)
		Close();
	if (option->m_function)
		option->m_function->Call();
}

void MenuWidget::Cancel()
{
	Close();
}
