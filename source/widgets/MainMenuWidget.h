#pragma once

#include "Graphics.h"
#include "cards/CardSetPackage.h"
#include "gui/gui.h"
#include "widgets/StudyProficiencyBar.h"
#include "widgets/AppWidget.h"
#include "widgets/MenuWidget.h"


class MainMenuItemWidget : public AppWidget
{
public:
	MainMenuItemWidget(const AccentedText& name, IStudySet* studySet);

	EventSignal<>& Clicked() { return m_clicked; }

	void SetMetrics(const StudySetMetrics& metrics);

	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;
	virtual void OnPress() override { m_clicked.Emit(); }

private:

	IStudySet* m_studySet;
	HBoxLayout m_layout;
	Label m_labelName;
	StudyProficiencyBar m_proficiencyBar;
	EventSignal<> m_clicked;
};


class MainMenuWidget : public AppWidget
{
public:
	MainMenuWidget(CardSetPackage::sptr package);

	void SetPackage(CardSetPackage::sptr package);
	MainMenuItemWidget* AddMenuOption(const AccentedText& name,
		IStudySet* studySet = nullptr);

	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	void PopulateMenuOptions(MenuWidget* menu, IStudySet* studySet);

	CardSetPackage::sptr m_package;
	Map<CardSet::sptr, MainMenuItemWidget*> m_cardSetItems;
	Map<CardSetPackage::sptr, MainMenuItemWidget*> m_packageItems;
	VBoxLayout m_mainLayout;
	VBoxLayout m_optionLayout;
	Widget m_titleWidget;
	HBoxLayout m_titleLayout;
	Label m_labelTitle;
	StudyProficiencyBar m_topProficiencyBar;
};
