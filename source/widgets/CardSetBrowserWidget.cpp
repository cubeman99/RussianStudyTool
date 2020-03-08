#include "CardSetBrowserWidget.h"
#include "RussianApp.h"
#include "widgets/CardSetEditWidget.h"
#include "states/StudyState.h"


CardSetBrowserWidget::CardSetBrowserWidget()
{
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleWidget.SetLayout(&m_layoutPath);

	m_scrollArea.SetWidget(&m_widgetOptions);
	m_widgetOptions.SetLayout(&m_layoutOptions);
	m_layoutOptions.SetItemBackgroundColors(true);

	// Create layouts
	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.Add(&m_titleWidget, 0.1f);
	m_mainLayout.Add(&m_scrollArea, 1.0f);
	SetLayout(&m_mainLayout);

	SetBackgroundColor(GUIConfig::color_background);

	// Connect signals
	AddKeyShortcut("escape", [this]() {
		if (!m_package->GetParent())
			return false;
		GoBack();
		return true;
	});
	AddKeyShortcut("backspace", [this]() {
		if (!m_package->GetParent())
			return false;
		GoBack();
		return true;
	});
}

CardSetBrowserWidget::CardSetBrowserWidget(CardSetPackage::sptr package) :
	CardSetBrowserWidget()
{
	SetPackage(package);
}

CardSetPackage::sptr CardSetBrowserWidget::GetPackage()
{
	return m_package;
}

void CardSetBrowserWidget::SetPackage(CardSetPackage::sptr package,
	CardSetPackage::sptr selectPackage)
{
	Button* button = nullptr;
	Button* toSelect = nullptr;
	m_package = package;
	m_layoutOptions.Clear();
	m_layoutPath.Clear();

	// Create path buttons
	m_layoutPath.Clear();
	for (CardSetPackage::sptr parent = m_package;
		parent; parent = parent->GetParent())
	{
		button = AllocateObject<Button>(parent->GetName());
		button->SetAlign(TextAlign::MIDDLE_LEFT);
		button->Clicked().Connect(this, parent,
			&CardSetBrowserWidget::NavigateIntoCardPackage);
		m_layoutPath.Insert(0, button);
	}
	m_layoutPath.AddStretch();

	// Option to go back
	if (m_package->GetParent())
	{
		button = AllocateObject<Button>("Back");
		button->SetAlign(TextAlign::MIDDLE_LEFT);
		button->Clicked().Connect(this, &CardSetBrowserWidget::GoBack);
		m_layoutOptions.Add(button);
		toSelect = button;
	}

	// Sub Card Packages
	for (auto subPackage : m_package->GetPackages())
	{
		button = AllocateObject<Button>("[...] " + subPackage->GetName());
		button->SetAlign(TextAlign::MIDDLE_LEFT);
		button->Clicked().Connect(
			this, subPackage, &CardSetBrowserWidget::NavigateIntoCardPackage);
		m_layoutOptions.Add(button);
		if (toSelect == nullptr)
			toSelect = button;
		if (selectPackage == subPackage)
			toSelect = button;
	}

	// Card Sets
	for (auto cardSet : m_package->GetCardSets())
	{
		button = AllocateObject<Button>(cardSet->GetName());
		button->SetAlign(TextAlign::MIDDLE_LEFT);
		button->Clicked().Connect(
			this, cardSet, &CardSetBrowserWidget::OnClickCardSet);
		m_layoutOptions.Add(button);
	}

	if (toSelect)
		toSelect->Focus();
}

void CardSetBrowserWidget::SetCloseOnSelectCardSet(bool closeOnSelectCardSet)
{
	m_closeOnSelectCardSet = closeOnSelectCardSet;
}

void CardSetBrowserWidget::NavigateIntoCardPackage(CardSetPackage::sptr package)
{
	SetPackage(package);
}

void CardSetBrowserWidget::GoBack()
{
	if (m_package->GetParent())
		SetPackage(m_package->GetParent(), m_package);
}

void CardSetBrowserWidget::OnClickCardSet(CardSet::sptr cardSet)
{
	m_cardSetClicked.Emit(cardSet);
	if (m_closeOnSelectCardSet)
		Close();
}
