#include "MainMenuWidget.h"
#include "RussianApp.h"
#include "states/StudyState.h"

MainMenuItemWidget::MainMenuItemWidget(const AccentedText& name, IStudySet* studySet) :
	m_studySet(studySet)
{
	SetFocusable(true);

	m_labelName.SetText(name);
	m_labelName.SetAlign(TextAlign::MIDDLE_LEFT);
	m_labelName.SetColor(GUIConfig::color_text);

	m_layout.Add(&m_labelName);
	m_layout.Add(&m_proficiencyBar);
	SetLayout(&m_layout);
}

void MainMenuItemWidget::SetMetrics(const StudySetMetrics & metrics)
{
	m_proficiencyBar.SetMetrics(metrics);
}

void MainMenuItemWidget::OnUpdate(float timeDelta)
{
}

void MainMenuItemWidget::OnRender(AppGraphics & g, float timeDelta)
{
}


MainMenuWidget::MainMenuWidget(CardSetPackage::sptr package) :
	m_package(package)
{
	SetPackage(package);

	m_labelTitle.SetAlign(TextAlign::MIDDLE_LEFT);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleLayout.Add(&m_labelTitle);
	m_titleLayout.Add(&m_topProficiencyBar);
	m_titleWidget.SetLayout(&m_titleLayout);

	m_optionLayout.SetItemBackgroundColors(true);

	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.Add(&m_titleWidget, 0.1f);
	m_mainLayout.Add(&m_optionLayout, 1.0f);
	SetLayout(&m_mainLayout);

	SetBackgroundColor(GUIConfig::color_background);
}

void MainMenuWidget::SetPackage(CardSetPackage::sptr package)
{
	MainMenuItemWidget* button;
	m_package = package;
	m_optionLayout.Clear();
	m_cardSetItems.clear();
	m_packageItems.clear();

	m_labelTitle.SetText(package->GetName());

	// Option to go back or quit
	if (m_package->GetParent())
	{
		button = AddMenuOption("Back");
		button->Clicked().Connect([this]() {
			SetPackage(m_package->GetParent());
		});
	}
	else
	{
		button = AddMenuOption("Quit");
		button->Clicked().Connect([this]() {
			Close();
		});
	}

	// Sub Card Packages
	for (auto subPackage : m_package->GetPackages())
	{
		button = AddMenuOption("[...] " + subPackage->GetName(), subPackage.get());
		button->Clicked().Connect([this, subPackage]() {
			SetPackage(subPackage);
		});
		m_packageItems[subPackage] = button;
	}

	// Card Sets
	for (auto cardSet : m_package->GetCardSets())
	{
		button = AddMenuOption(cardSet->GetName(), cardSet.get());
		button->Clicked().Connect([this, cardSet]() {
			MenuWidget* menu = new MenuWidget(cardSet->GetName());
			PopulateMenuOptions(menu, cardSet.get());
			menu->AddMenuOption("List", true, [this, cardSet]() {
				// TODO:
			});
			menu->AddMenuOption("Edit", true, [this, cardSet]() {
				// TODO:
			});
			menu->AddCancelOption();
			GetApp()->PushState(menu);
		});
		m_cardSetItems[cardSet] = button;
	}

	// Option to select this package
	button = AddMenuOption("[" + m_package->GetName() + "]", m_package.get());
	button->Clicked().Connect([this]() {
		MenuWidget* menu = new MenuWidget(m_package->GetName());
		PopulateMenuOptions(menu, m_package.get());
		menu->AddCancelOption();
		GetApp()->PushState(menu);
	});
	m_packageItems[m_package] = button;
}

MainMenuItemWidget* MainMenuWidget::AddMenuOption(
	const AccentedText& name, IStudySet* studySet)
{
	MainMenuItemWidget* button = new MainMenuItemWidget(name, studySet);
	m_optionLayout.Add(button);
	return button;
}

void MainMenuWidget::OnUpdate(float timeDelta)
{
	auto keyboard = GetApp()->GetKeyboard();
	if (keyboard->IsKeyPressed(Keys::backspace) && m_package->GetParent())
		SetPackage(m_package->GetParent());

	// Update study metrics
	auto& studyDatabase = GetApp()->GetStudyDatabase();
	for (auto it : m_cardSetItems)
		it.second->SetMetrics(studyDatabase.GetStudyMetrics(it.first));
	for (auto it : m_packageItems)
		it.second->SetMetrics(studyDatabase.GetStudyMetrics(it.first));
	m_topProficiencyBar.SetMetrics(studyDatabase.GetStudyMetrics(m_package));
}

void MainMenuWidget::OnRender(AppGraphics& g, float timeDelta)
{
}

void MainMenuWidget::PopulateMenuOptions(MenuWidget* menu, IStudySet* studySet)
{
	menu->AddMenuOption("Quiz Random Sides", true, [this, studySet]() {
		GetApp()->PushState(new StudyState(studySet));
	});
	menu->AddMenuOption("Quiz Random Forms", true, [this, studySet]() {
		// TODO:
	});
	menu->AddMenuOption("Quiz English", true, [this, studySet]() {
		// TODO:
	});
	menu->AddMenuOption("Quiz Russian", true, [this, studySet]() {
		// TODO:
	});
	menu->AddMenuOption("Quiz New Cards", true, [this, studySet]() {
		// TODO:
	});
	menu->AddMenuOption("Quiz Problem Cards", true, [this, studySet]() {
		// TODO:
	});
	menu->AddMenuOption("Query", true, [this, studySet]() {
		// TODO:
	});
}
