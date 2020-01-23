#pragma once

#include "cmgApplicationState.h"
#include "Graphics.h"
#include "CardSetPackage.h"
#include "gui/gui.h"

class MainMenuWidget : public Widget
{
public:
	using sptr = cmg::shared_ptr<StudySet>;

	MainMenuWidget(CardSetPackage::sptr package);

	void AddMenuOption(const AccentedText& name);

	void OnUpdate(float timeDelta) override;
	void OnRender(AppGraphics& g, float timeDelta) override;

private:
	VBoxLayout m_optionLayout;
	CardSetPackage::sptr m_package;
};
