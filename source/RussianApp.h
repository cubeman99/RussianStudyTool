#pragma once

#include <cmgApplication/cmg_application.h>
#include <cmgMath/cmg_math.h>
#include "Russian.h"
#include "cards/CardDatabase.h"
#include "Resources.h"
#include "states/StudyState.h"
#include "states/ApplicationStateStack.h"
#include "states/TestState.h"
#include "widgets/MainMenuWidget.h"
#include "study/StudyDatabase.h"
#include "PedalInput.h"

/*
Done:
AccentedText
Enums
AccentedText drawing
Card JSON load
CardSet JSON load
Card
StudySet
CardSet
CardKey/attributes
CardDatabase
Wheel Input
GameState
StudyData
StudyState
cmgGameState
StudyMetrics
MainMenuState
Titles in GUI
Pause Menu
Scheduler logic

FIXME:
accent in english vs russian causing problems with card keys
study metrics are not correct, they are counting duplicate cards

ROADMAP:
More GUI Widgets and text editing
Card Set Edit Widget
Card Edit Widget
Related Cards Edit Widget
Add to Card Set Edit Widget
WordData
WordData JSON load
Wiktionary Data
Cooljugator
Custom Word Data
External Word Downloading
ExampleDatabase
Memory allocation management for widgets
*/

class RussianStudyToolApp : public Application
{
public:
	RussianStudyToolApp();
	~RussianStudyToolApp();

	void OnInitialize() override;
	void OnQuit() override;
	void OnResizeWindow(int width, int height) override;
	void OnDropFiles(const Array<Path>& paths) override;
	void OnUpdate(float timeDelta) override;
	void OnRender() override;

	void PopState();
	void PushState(ApplicationState* state);
	void PushState(Widget* widget);
	ApplicationStateStack& GetStateStack() { return m_stateStack; };

	CardDatabase& GetCardDatabase() { return m_cardDatabase; }
	StudyDatabase& GetStudyDatabase() { return m_studyDatabase; }

	static RussianStudyToolApp* GetInstance() { return s_instance; }
	Joystick* GetJoystick() { return m_joystick; }

	const PedalInput& GetLeftPedalInput() const { return m_pedals[0]; }
	const PedalInput& GetMiddlePedalInput() const { return m_pedals[1]; }
	const PedalInput& GetRightPedalInput() const { return m_pedals[2]; }
	const PedalInput& GetPedalInput(uint32 index) const { return m_pedals[index]; }

private:
	static RussianStudyToolApp* s_instance;

	CardDatabase m_cardDatabase;
	StudyDatabase m_studyDatabase;
	RenderParams m_renderParams;
	Font::sptr m_font;
	ApplicationStateStack m_stateStack;
	MainMenuWidget* m_mainMenuWidget;

	Joystick* m_joystick = nullptr;
	bool m_joystickButtonState[3] = { false, false, false };
	bool m_joystickButtonStatePrev[3] = { false, false, false };
	PedalInput m_pedals[3];

public:
	bool m_joystickButtonPressed[3] = { false, false, false };
};

