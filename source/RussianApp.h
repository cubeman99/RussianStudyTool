#pragma once

#include <cmgApplication/cmg_application.h>
#include <cmgMath/cmg_math.h>
#include "Russian.h"
#include "CardDatabase.h"
#include "Resources.h"
#include "states/StudyState.h"
#include "states/TestState.h"
#include "study/StudyDatabase.h"

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

LARGE TASKS:
cmgGameState
yaml loading
GUI widgets
event-based and unicode input

FIXME:
accent in english vs russian causing problems with card keys

ROADMAP:
Wheel Input
GameState
StudyState
MainMenuState
StudyData
Word
Noun
Verb
Adjective
Word JSON load/save
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

	CardDatabase& GetCardDatabase() { return m_cardDatabase; }
	StudyDatabase& GetStudyDatabase() { return m_studyDatabase; }

	static RussianStudyToolApp* GetInstance() { return s_instance; }

private:
	static RussianStudyToolApp* s_instance;

	CardDatabase m_cardDatabase;
	StudyDatabase m_studyDatabase;
	RenderParams m_renderParams;
	Font::sptr m_font;
	TestState* m_state;
	Joystick* m_joystick;
	bool m_joystickButtonState[3] = { false, false, false };
	bool m_joystickButtonStatePrev[3] = { false, false, false };

public:
	bool m_joystickButtonPressed[3] = { false, false, false };
};

