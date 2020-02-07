#pragma once

#include "widgets/AppWidget.h"


class MenuItemWidget : public AppWidget
{
public:
	friend class MenuWidget;

	MenuItemWidget(const AccentedText& name);

	EventSignal<>& Clicked() { return m_clicked; }

	virtual void OnPress() override { m_clicked.Emit(); }

private:
	Label m_labelName;
	HBoxLayout m_layout;
	EventSignal<> m_clicked;
	Delegate<void>* m_function = nullptr;
	bool m_closeMenu;
};


class MenuWidget : public AppWidget
{
public:
	MenuWidget(const AccentedText& title);

	void Clear();
	MenuItemWidget* AddMenuOption(const AccentedText& name);
	MenuItemWidget* AddCancelOption(const AccentedText& name = "Cancel");

	MenuItemWidget* AddMenuOption(const AccentedText& name,
		bool closeMenu, Delegate<void>* function)
	{
		MenuItemWidget* option = AddMenuOption(name);
		option->m_function = function;
		option->m_closeMenu = closeMenu;
		option->Clicked().Connect(this, option, &MenuWidget::OnSelectOption);
		return option;
	}

	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	void OnSelectOption(MenuItemWidget* option);
	void Cancel();

	Array<MenuItemWidget*> m_options;
	Widget m_titleWidget;
	Widget m_widgetWindow;
	Label m_labelTitle;
	VBoxLayout m_mainLayout;
	AnchorLayout m_anchorLayout;
	VBoxLayout m_optionLayout;
	HBoxLayout m_titleLayout;
};
