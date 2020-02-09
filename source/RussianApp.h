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
#include "external/wiktionary/Wiktionary.h"
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
More GUI Widgets and text editing
Card Set Edit Widget

FIXME:
accent in english vs russian causing problems with card keys
study metrics are not correct, they are counting duplicate cards

ROADMAP:
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
	virtual ~RussianStudyToolApp();

	void OnInitialize() override;
	void OnQuit() override;
	void OnResizeWindow(int width, int height) override;
	void OnDropFiles(const Array<Path>& paths) override;
	void OnUpdate(float timeDelta) override;
	void OnRender() override;

	void PopState();
	void PushState(ApplicationState* state);
	void PushState(Widget* widget);

	CardDatabase& GetCardDatabase() { return m_cardDatabase; }
	StudyDatabase& GetStudyDatabase() { return m_studyDatabase; }
	wiki::Wiktionary& GetWiktionary() { return m_wiktionary; }

	ApplicationStateStack& GetStateStack() { return m_stateStack; };
	Joystick* GetJoystick() { return m_joystick; }
	const PedalInput& GetLeftPedalInput() const { return m_pedals[0]; }
	const PedalInput& GetMiddlePedalInput() const { return m_pedals[1]; }
	const PedalInput& GetRightPedalInput() const { return m_pedals[2]; }
	const PedalInput& GetPedalInput(uint32 index) const { return m_pedals[index]; }

	static RussianStudyToolApp* GetInstance() { return s_instance; }

private:
	void OnMouseDown(Window::MouseDownEvent* e);
	void OnMouseUp(Window::MouseUpEvent* e);
	void OnKeyDown(Window::KeyDownEvent* e);
	void OnKeyTyped(Window::KeyTypedEvent* e);

	static RussianStudyToolApp* s_instance;

	// Databases
	CardDatabase m_cardDatabase;
	StudyDatabase m_studyDatabase;
	wiki::Wiktionary m_wiktionary;

	RenderParams m_renderParams;
	Font::sptr m_font;
	Font::sptr m_fontSmall;
	Font::sptr m_fontLarge;

	ApplicationStateStack m_stateStack;
	MainMenuWidget* m_mainMenuWidget;

	// Input
	bool m_isJoystickReady = false;
	Joystick* m_joystick = nullptr;
	bool m_joystickButtonState[3] = { false, false, false };
	bool m_joystickButtonStatePrev[3] = { false, false, false };
	PedalInput m_pedals[3];

public:
	bool m_joystickButtonPressed[3] = { false, false, false };
};

