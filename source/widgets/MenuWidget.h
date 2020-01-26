#pragma once

#include "widgets/AppWidget.h"


class MenuItemWidget : public AppWidget
{
public:
	MenuItemWidget(const AccentedText& name);

	EventSignal<>& Clicked() { return m_clicked; }

	virtual void OnPress() override { m_clicked.Emit(); }

private:
	Label m_labelName;
	HBoxLayout m_layout;
	EventSignal<> m_clicked;
};


class MenuWidget : public AppWidget
{
public:
	MenuWidget(const AccentedText& title);

	void Clear();
	MenuItemWidget* AddMenuOption(const AccentedText& name);
	MenuItemWidget* AddCancelOption(const AccentedText& name = "Cancel");

	template <typename T_Function>
	MenuItemWidget* AddMenuOption(const AccentedText& name, bool closeMenu, T_Function& function)
	{
		MenuItemWidget* option = AddMenuOption(name);
		option->Clicked().Connect([this, closeMenu, function]() {
			if (closeMenu)
			{
				Close();
				GetApp()->PopState();
			}
			function();
		});
		return option;
	}

	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	Array<MenuItemWidget*> m_options;
	Widget m_titleWidget;
	Widget m_widgetWindow;
	Label m_labelTitle;
	VBoxLayout m_mainLayout;
	AnchorLayout m_anchorLayout;
	VBoxLayout m_optionLayout;
	HBoxLayout m_titleLayout;
};
