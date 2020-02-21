#include "MainMenuWidget.h"
#include "RussianApp.h"
#include "widgets/CardSetEditWidget.h"
#include "states/StudyState.h"

MainMenuItemWidget::MainMenuItemWidget(const AccentedText& name, IStudySet* studySet) :
	m_studySet(studySet)
{
	SetFocusable(true);

	m_labelName.SetText(name);
	m_labelName.SetAlign(TextAlign::MIDDLE_LEFT);
	m_labelName.SetColor(GUIConfig::color_text);

	m_layout.Add(&m_labelName, 1.0f);
	m_layout.Add(&m_proficiencyBar, 1.0f);
	SetLayout(&m_layout);
}

void MainMenuItemWidget::SetMetrics(const StudySetMetrics & metrics)
{
	m_proficiencyBar.SetMetrics(metrics);
}

MainMenuWidget::MainMenuWidget(CardSetPackage::sptr package) :
	m_package(package)
{
	SetPackage(package);

	m_labelTitle.SetAlign(TextAlign::MIDDLE_LEFT);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleLayout.Add(&m_labelTitle, 1.0f);
	m_titleLayout.Add(&m_topProficiencyBar, 1.0f);
	m_titleWidget.SetLayout(&m_titleLayout);

	m_scrollArea.SetWidget(&m_widgetOptions);
	m_widgetOptions.SetLayout(&m_optionLayout);
	m_optionLayout.SetItemBackgroundColors(true);

	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.Add(&m_titleWidget, 0.1f);
	m_mainLayout.Add(&m_scrollArea, 1.0f);
	SetLayout(&m_mainLayout);

	SetBackgroundColor(GUIConfig::color_background);

	AddKeyShortcut("escape", [this]() {
		GoBack();
		return true;
	});
}

void MainMenuWidget::SetPackage(CardSetPackage::sptr package,
	CardSetPackage::sptr selectPackage)
{
	MainMenuItemWidget* button;
	m_package = package;
	m_optionLayout.Clear();
	m_cardSetItems.clear();
	m_packageItems.clear();

	m_labelTitle.SetText(package->GetName());

	Widget* toSelect = nullptr;

	// Option to go back or quit
	if (m_package->GetParent())
	{
		button = AddMenuOption("Back");
		button->Clicked().Connect(this, &MainMenuWidget::GoBack);
		toSelect = button;
	}
	else
	{
		button = AddMenuOption("Quit");
		button->Clicked().Connect(this, &MainMenuWidget::GoBack);
	}

	// Sub Card Packages
	for (auto subPackage : m_package->GetPackages())
	{
		button = AddMenuOption(
			"[...] " + subPackage->GetName(), subPackage.get());
		button->Clicked().Connect(
			this, subPackage, &MainMenuWidget::NavigateIntoCardPackage);
		m_packageItems[subPackage] = button;
		if (toSelect == nullptr)
			toSelect = button;
		if (selectPackage == subPackage)
			toSelect = button;
	}

	// Card Sets
	for (auto cardSet : m_package->GetCardSets())
	{
		button = AddMenuOption(cardSet->GetName(), cardSet.get());
		button->Clicked().Connect(
			this, cardSet, &MainMenuWidget::OpenCardSet);
		m_cardSetItems[cardSet] = button;
	}

	// Option to select this package
	button = AddMenuOption("[" + m_package->GetName() + "]", m_package.get());
	button->Clicked().Connect(
		this, m_package, &MainMenuWidget::OpenCardPackage);
	m_packageItems[m_package] = button;

	if (toSelect)
		toSelect->Focus();
}

void MainMenuWidget::OpenCardSet(CardSet::sptr cardSet)
{
	MenuWidget* menu = new MenuWidget(cardSet->GetName());
	PopulateMenuOptions(menu, cardSet.get());
	// TODO: card list
	menu->AddMenuOption("List", true, nullptr);
	menu->AddMenuOption("Edit", true, 
		new CaptureMethodDelegate(this, cardSet,
			&MainMenuWidget::OpenCardSetEditor));
	menu->AddCancelOption();
	GetApp()->PushState(menu);
}

void MainMenuWidget::OpenCardPackage(CardSetPackage::sptr package)
{
	MenuWidget* menu = new MenuWidget(package->GetName());
	PopulateMenuOptions(menu, package.get());
	menu->AddCancelOption();
	GetApp()->PushState(menu);
}

void MainMenuWidget::NavigateIntoCardPackage(CardSetPackage::sptr package)
{
	SetPackage(package);
}

void MainMenuWidget::GoBack()
{
	if (m_package->GetParent())
	{
		SetPackage(m_package->GetParent(), m_package);
	}
	else
		Close();
}

MainMenuItemWidget* MainMenuWidget::AddMenuOption(
	const AccentedText& name, IStudySet* studySet)
{
	MainMenuItemWidget* option = new MainMenuItemWidget(name, studySet);
	m_optionLayout.Add(option);
	return option;
}

void MainMenuWidget::OnUpdate(float timeDelta)
{
	auto keyboard = GetApp()->GetKeyboard();
	if (keyboard->IsKeyPressed(Keys::backspace) && m_package->GetParent())
		GoBack();

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

void MainMenuWidget::OpenStudyState(IStudySet * studySet)
{
	GetApp()->PushState(new StudyState(studySet));
}

void MainMenuWidget::OpenCardSetEditor(CardSet::sptr cardSet)
{
	GetApp()->PushState(new CardSetEditWidget(cardSet));
}

void MainMenuWidget::PopulateMenuOptions(MenuWidget* menu, IStudySet* studySet)
{
	menu->AddMenuOption("Quiz Random Sides", true, 
		new CaptureMethodDelegate(this, studySet,
			&MainMenuWidget::OpenStudyState));
	menu->AddMenuOption("Quiz Random Forms", true, 
		new CaptureMethodDelegate(this, studySet,
			&MainMenuWidget::OpenStudyState));
	menu->AddMenuOption("Quiz English", true, 
		new CaptureMethodDelegate(this, studySet,
			&MainMenuWidget::OpenStudyState));
	menu->AddMenuOption("Quiz Russian", true, 
		new CaptureMethodDelegate(this, studySet,
			&MainMenuWidget::OpenStudyState));
	menu->AddMenuOption("Quiz New Cards", true, 
		new CaptureMethodDelegate(this, studySet,
			&MainMenuWidget::OpenStudyState));
	menu->AddMenuOption("Quiz Problem Cards", true, 
		new CaptureMethodDelegate(this, studySet,
			&MainMenuWidget::OpenStudyState));
	menu->AddMenuOption("Query", true, nullptr);
}
