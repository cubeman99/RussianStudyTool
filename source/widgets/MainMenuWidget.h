#pragma once

#include "Graphics.h"
#include "cards/CardSetPackage.h"
#include "gui/gui.h"
#include "widgets/StudyProficiencyBar.h"
#include "widgets/AppWidget.h"
#include "widgets/MenuWidget.h"
#include "study/Scheduler.h"

#define DECLARE_GUI_OBJECT(_class) \
	using sptr = cmg::shared_ptr<_class>


class MainMenuItemWidget : public AppWidget
{
public:
	DECLARE_GUI_OBJECT(MainMenuItemWidget);

	MainMenuItemWidget(const AccentedText& name, IStudySet* studySet);

	EventSignal<>& Clicked() { return m_clicked; }

	void SetMetrics(const StudySetMetrics& metrics);

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
	virtual ~MainMenuWidget();

	void Clear();
	void SetPackage(CardSetPackage::sptr package,
		CardSetPackage::sptr selectPackage = nullptr);
	void OpenCardSet(CardSet::sptr cardSet);
	void OpenStudySet(IStudySet* studySet);
	void OpenCardPackage(CardSetPackage::sptr package);
	void OpenCardEditor();
	MainMenuItemWidget::sptr AddMenuOption(const AccentedText& name,
		IStudySet* studySet = nullptr);

	void NavigateIntoCardPackage(CardSetPackage::sptr package);
	void GoBack();

	virtual void OnInitialize() override;
	virtual void OnUpdate(float timeDelta) override;
	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	struct StudyStateOptions
	{
		IStudySet* studySet = nullptr;
		CardSet::sptr cardSet;
		StudyParams studyParams;
	};

	void OpenStudyState(StudyStateOptions options);
	void OpenCardSetEditor(CardSet::sptr cardSet);
	void OpenCardListView(IStudySet* studySet);
	void OpenCreateCardSetWidget(CardSetPackage::sptr package);
	void PopulateMenuOptions(MenuWidget* menu, IStudySet* studySet, CardSet::sptr cardSet);
	void OnCardSetCreated(CardSet::sptr cardSet);
	void OpenCardSetMerge(CardSet::sptr cardSet);
	void MergeCardSetInto(CardSet::sptr from, CardSet::sptr to);
	void OnCardCreated(Card::sptr card);
	void OnCardDeleted(Card::sptr card);
	void OnCardAddedToSet(Card::sptr card, CardSet::sptr cardSet);
	void OnCardRemovedFromSet(Card::sptr card, CardSet::sptr cardSet);
	void RefreshStudySetsForCard(Card::sptr card);
	CardSetPackage::sptr m_package;

	Array<MainMenuItemWidget::sptr> m_menuItems;
	Map<IStudySet*, MainMenuItemWidget::sptr> m_studySetItems;
	Map<CardSet::sptr, MainMenuItemWidget::sptr> m_cardSetItems;
	Map<CardSetPackage::sptr, MainMenuItemWidget::sptr> m_packageItems;

	VBoxLayout m_mainLayout;
	AbstractScrollArea m_scrollArea;
	Widget m_widgetOptions;
	VBoxLayout m_optionLayout;
	Widget m_titleWidget;
	HBoxLayout m_titleLayout;
	Label m_labelTitle;
	Label m_labelScore;
	StudyProficiencyBar m_topProficiencyBar;

	StudySet m_studySetOrphans;
	StudySet m_studySetUncategorized;
};
