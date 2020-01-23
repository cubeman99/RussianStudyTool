#include "MainMenuWidget.h"
#include "RussianApp.h"

MainMenuWidget::MainMenuWidget(CardSetPackage::sptr package) :
	m_package(package)
{
	AddMenuOption("Back");

	for (auto subPackage : m_package->GetPackages())
	{
		AddMenuOption(subPackage->GetName());
	}

	for (auto cardSet : m_package->GetCardSets())
	{
		AddMenuOption(cardSet->GetName());
	}

	AddMenuOption(m_package->GetName());

	SetLayout(&m_optionLayout);
}

void MainMenuWidget::AddMenuOption(const AccentedText & name)
{
	auto font = RussianStudyToolApp::GetInstance()->GetResourceManager()->Get<Font>(Res::FONT);
	m_optionLayout.Add(new Label(name, font, Color::WHITE, TextAlign::MIDDLE_LEFT));
}

void MainMenuWidget::OnUpdate(float timeDelta)
{
}

void MainMenuWidget::OnRender(AppGraphics& g, float timeDelta)
{
}
