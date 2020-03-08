#include "MainMenuWidget.h"
#include "RussianApp.h"
#include "widgets/CardSetEditWidget.h"
#include "states/StudyState.h"
#include "widgets/editors/CreateCardSetWidget.h"
#include "widgets/CardListView.h"
#include "widgets/CardEditWidget.h"


MainMenuItemWidget::MainMenuItemWidget(const AccentedText& name, IStudySet* studySet) :
	m_studySet(studySet)
{
	SetFocusable(true);

	m_labelName.SetText(name);
	m_labelName.SetAlign(TextAlign::MIDDLE_LEFT);
	m_labelName.SetColor(GUIConfig::color_text);

	m_layout.Add(&m_labelName, 1.0f);
	if (m_studySet)
		m_layout.Add(&m_proficiencyBar, 1.0f);
	SetLayout(&m_layout);
}

void MainMenuItemWidget::SetMetrics(const StudySetMetrics & metrics)
{
	m_proficiencyBar.SetMetrics(metrics);
}

MainMenuWidget::MainMenuWidget(CardSetPackage::sptr package) :
	m_package(package),
	m_studySetOrphans("Orphan Cards"),
	m_studySetUncategorized("Uncategorized Cards")
{
	SetPackage(package);

	m_labelTitle.SetAlign(TextAlign::MIDDLE_LEFT);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleLayout.Add(&m_labelTitle, 1.0f);
	m_titleLayout.Add(&m_labelScore, 1.0f);
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

MainMenuWidget::~MainMenuWidget()
{
	Clear();
}

void MainMenuWidget::Clear()
{
	m_package = nullptr;
	m_optionLayout.Clear();
	m_cardSetItems.clear();
	m_packageItems.clear();
	m_studySetItems.clear();
	m_menuItems.clear();
}

void MainMenuWidget::SetPackage(CardSetPackage::sptr package,
	CardSetPackage::sptr selectPackage)
{
	Clear();

	MainMenuItemWidget::sptr button;
	m_package = package;

	m_labelTitle.SetText(package->GetName());

	MainMenuItemWidget::sptr toSelect = nullptr;

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

	if (!m_package->GetParent())
	{
		// Orphans
		button = AddMenuOption("** Orphans", &m_studySetOrphans);
		button->Clicked().Connect(
			this, (IStudySet*) &m_studySetOrphans, &MainMenuWidget::OpenStudySet);
		m_studySetItems[&m_studySetOrphans] = button;

		// Uncategorized
		button = AddMenuOption("** Uncategorized", &m_studySetUncategorized);
		button->Clicked().Connect(
			this, (IStudySet*) &m_studySetUncategorized, &MainMenuWidget::OpenStudySet);
		m_studySetItems[&m_studySetUncategorized] = button;

		// Card editor
		button = AddMenuOption("** Card Editor");
		button->Clicked().Connect(this, &MainMenuWidget::OpenCardEditor);
	}

	// Option to select this package
	button = AddMenuOption("[" + m_package->GetName() + "]", m_package.get());
	button->Clicked().Connect(
		this, m_package, &MainMenuWidget::OpenCardPackage);
	m_packageItems[m_package] = button;
	if (selectPackage == m_package)
		toSelect = button;

	if (toSelect)
		toSelect->Focus();

	// Calculate metrics for study sets
	if (GetApp())
	{
		auto& studyDatabase = GetApp()->GetStudyDatabase();
		for (auto it : m_studySetItems)
			it.second->SetMetrics(studyDatabase.GetStudySetMetrics(it.first));
	}
}

void MainMenuWidget::OpenCardSet(CardSet::sptr cardSet)
{
	MenuWidget* menu = new MenuWidget(cardSet->GetName());
	PopulateMenuOptions(menu, cardSet.get(), cardSet);
	menu->AddMenuOption("Edit", true, 
		new CaptureMethodDelegate(this, cardSet,
			&MainMenuWidget::OpenCardSetEditor));
	menu->AddMenuOption("Delete and merge", true, 
		new CaptureMethodDelegate(this, cardSet,
			&MainMenuWidget::OpenCardSetMerge));
	menu->AddCancelOption();
	GetApp()->PushState(menu);
}

void MainMenuWidget::OpenStudySet(IStudySet* studySet)
{
	MenuWidget* menu = new MenuWidget(studySet->GetName());
	PopulateMenuOptions(menu, studySet, nullptr);
	menu->AddCancelOption();
	GetApp()->PushState(menu);
}

void MainMenuWidget::OpenCardPackage(CardSetPackage::sptr package)
{
	MenuWidget* menu = new MenuWidget(package->GetName());
	PopulateMenuOptions(menu, package.get(), nullptr);
	menu->AddMenuOption("Create New Card Set", true, 
		new CaptureMethodDelegate(this, package,
			&MainMenuWidget::OpenCreateCardSetWidget));
	menu->AddCancelOption();
	GetApp()->PushState(menu);
}

void MainMenuWidget::OpenCardEditor()
{
	GetApp()->PushState(new CardEditWidget());
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

MainMenuItemWidget::sptr MainMenuWidget::AddMenuOption(
	const AccentedText& name, IStudySet* studySet)
{
	MainMenuItemWidget::sptr option = cmg::make_shared<MainMenuItemWidget>(
		name, studySet);
	m_menuItems.push_back(option);
	m_optionLayout.Add(option.get());
	return option;
}

void MainMenuWidget::OnInitialize()
{
	if (m_package && m_package->GetParent())
		return;

	auto& cardDatabase = GetApp()->GetCardDatabase();

	// Create study sets for orphan and uncategorized cards
	m_studySetOrphans.ClearCards();
	m_studySetUncategorized.ClearCards();
	for (auto it : cardDatabase.GetCards())
		RefreshStudySetsForCard(it.second);

	// Calculate metrics for study sets
	auto& studyDatabase = GetApp()->GetStudyDatabase();
	for (auto it : m_studySetItems)
		it.second->SetMetrics(studyDatabase.GetStudySetMetrics(it.first));

	cardDatabase.CardAddedToSet().Connect(
		this, &MainMenuWidget::OnCardAddedToSet);
	cardDatabase.CardAddedToSet().Connect(
		this, &MainMenuWidget::OnCardRemovedFromSet);
	cardDatabase.CardCreated().Connect(
		this, &MainMenuWidget::OnCardCreated);
	cardDatabase.CardDeleted().Connect(
		this, &MainMenuWidget::OnCardDeleted);
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

	// Get study metrics
	StudySetMetrics metrics = studyDatabase.GetStudyMetrics(m_package);
	int realCount = (int) (metrics.GetHistoryScore() * metrics.GetTotalCount());
	m_labelScore.SetText(std::to_string(realCount));
	m_topProficiencyBar.SetMetrics(metrics);
}

void MainMenuWidget::OnRender(AppGraphics& g, float timeDelta)
{
}

void MainMenuWidget::OpenStudyState(StudyStateOptions options)
{
	GetApp()->PushState(new StudyState(
		options.studySet, options.cardSet, options.studyParams));
}

void MainMenuWidget::OpenCardSetEditor(CardSet::sptr cardSet)
{
	GetApp()->PushState(new CardSetEditWidget(cardSet));
}

void MainMenuWidget::OpenCardListView(IStudySet* studySet)
{
	GetApp()->PushState(new CardListView(studySet));
}

void MainMenuWidget::OpenCreateCardSetWidget(CardSetPackage::sptr package)
{
	CreateCardSetWidget* widget = new CreateCardSetWidget("", package);
	widget->CardSetCreated().Connect(this, &MainMenuWidget::OnCardSetCreated);
	GetApp()->PushState(widget);
}

void MainMenuWidget::PopulateMenuOptions(MenuWidget* menu,
	IStudySet* studySet, CardSet::sptr cardSet)
{
	StudyStateOptions options;
	options.studySet = studySet;
	options.cardSet = cardSet;
	menu->AddMenuOption("Quiz Random Sides", true, 
		new CaptureMethodDelegate(this, options,
			&MainMenuWidget::OpenStudyState));
	// menu->AddMenuOption("Quiz Random Forms", true, 
	// 	new CaptureMethodDelegate(this, studySet,
	// 		&MainMenuWidget::OpenStudyState));
	// menu->AddMenuOption("Quiz English", true, 
	// 	new CaptureMethodDelegate(this, studySet,
	// 		&MainMenuWidget::OpenStudyState));
	// menu->AddMenuOption("Quiz Russian", true, 
	// 	new CaptureMethodDelegate(this, studySet,
	// 		&MainMenuWidget::OpenStudyState));
	// menu->AddMenuOption("Quiz New Cards", true, 
	// 	new CaptureMethodDelegate(this, studySet,
	// 		&MainMenuWidget::OpenStudyState));
	// menu->AddMenuOption("Quiz Problem Cards", true, 
	// 	new CaptureMethodDelegate(this, studySet,
	// 		&MainMenuWidget::OpenStudyState));
	// menu->AddMenuOption("Query", true, nullptr);
	menu->AddMenuOption("List", true, 
		new CaptureMethodDelegate(this, studySet,
			&MainMenuWidget::OpenCardListView));
}

void MainMenuWidget::OnCardSetCreated(CardSet::sptr cardSet)
{
	if (cardSet->GetParent() == m_package)
	{
		SetPackage(m_package, m_package);
	}
}

void MainMenuWidget::OpenCardSetMerge(CardSet::sptr cardSet)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	CardSetBrowserWidget* widget = new CardSetBrowserWidget(
		cardSet->GetParent());
	widget->SetCloseOnSelectCardSet(true);
	widget->CardSetClicked().Connect(
		this, cardSet, &MainMenuWidget::MergeCardSetInto);
	GetApp()->PushState(widget);
}

void MainMenuWidget::MergeCardSetInto(CardSet::sptr from, CardSet::sptr to)
{
	if (from != to)
	{
		auto& cardDatabase = GetApp()->GetCardDatabase();
		cardDatabase.MergeCardSets(from, to);
		cardDatabase.SaveChanges();
		SetPackage(m_package);
	}
	else
	{
		CMG_LOG_WARN() << "Cannot merge a card set into itself!";
	}
}

void MainMenuWidget::OnCardCreated(Card::sptr card)
{
	RefreshStudySetsForCard(card);
}

void MainMenuWidget::OnCardDeleted(Card::sptr card)
{
	RefreshStudySetsForCard(card);
}

void MainMenuWidget::OnCardAddedToSet(Card::sptr card, CardSet::sptr cardSet)
{
	RefreshStudySetsForCard(card);
}

void MainMenuWidget::OnCardRemovedFromSet(Card::sptr card, CardSet::sptr cardSet)
{
	RefreshStudySetsForCard(card);
}

void MainMenuWidget::RefreshStudySetsForCard(Card::sptr card)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();

	auto& cardSets = cardDatabase.GetCardSetsWithCard(card);
	bool isOrphan = cardSets.empty();
	bool isCategorized = false;
	for (auto cardSet : cardSets)
	{
		if (cardSet->GetCardSetType() == CardSetType::k_categorical)
		{
			isCategorized = true;
			break;
		}
	}

	if (isOrphan)
		m_studySetOrphans.AddCard(card);
	else
		m_studySetOrphans.RemoveCard(card);

	if (!isCategorized)
		m_studySetUncategorized.AddCard(card);
	else
		m_studySetUncategorized.RemoveCard(card);
}
