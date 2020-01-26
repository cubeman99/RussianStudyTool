#include "RussianApp.h"
#include "Resources.h"
#include "states/GUIState.h"
#include "widgets/CardSearchWidget.h"

static const Path g_assetsPath = "../assets";
static const Path g_cardSetRootPath = "../assets/data/cards";

RussianStudyToolApp* RussianStudyToolApp::s_instance = nullptr;


RussianStudyToolApp::RussianStudyToolApp() :
	m_studyDatabase(m_cardDatabase)
{
	s_instance = this;

	m_pedals[0] = PedalInput("Left", 1.0f, -1.0f, 0.05f, 0.005f);
	m_pedals[1] = PedalInput("Middle", 1.0f, -1.0f, 0.05f, 0.005f);
	m_pedals[2] = PedalInput("Right", 1.0f, -1.0f, 0.05f, 0.005f);

	cmg::core::console::EnableUnicode();

	m_renderParams.SetClearColor(Color::BLACK);
	m_renderParams.EnablePolygonSmooth(false);
	m_renderParams.EnableLineSmooth(false);
	m_renderParams.SetPolygonMode(PolygonMode::k_fill);
	m_renderParams.SetClearBits(true, true, false);
	m_renderParams.EnableBlend(true);
	m_renderParams.EnableCullFace(false);
	m_renderParams.EnableDepthBufferWrite(false);
	m_renderParams.EnableDepthTest(false);
	m_renderParams.EnableNearFarPlaneClipping(false);

	auto resourceManager = GetResourceManager();
	resourceManager->AddPath(g_assetsPath);
}

RussianStudyToolApp::~RussianStudyToolApp()
{
}

void RussianStudyToolApp::OnInitialize()
{
	auto resourceManager = GetResourceManager();

	//resourceManager->LoadBuiltInFont(m_font, BuiltInFonts::FONT_CONSOLE);
	resourceManager->LoadFont(m_font, Res::FONT, 14, 0x20, 0x500);

	m_studyDatabase.LoadStudyData(g_assetsPath / "data/study_data.json");
	m_cardDatabase.LoadCardData(g_assetsPath / "data/cards.json");
	m_cardDatabase.LoadCardSets(g_cardSetRootPath);

	//StudySet::sptr studySet = m_cardDatabase.GetCardSet(CardSetKey(u"cars & automotive"));

	m_mainMenuWidget = new MainMenuWidget(m_cardDatabase.GetRootPackage());
	m_stateStack.Push(new GUIState(m_mainMenuWidget));
	//PushState(new CardSearchWidget());

	m_stateStack.Begin(this);

	m_joystick = GetInputManager()->AddDevice<Joystick>(
		"Thrustmaster T300RS Racing Wheel");
}

void RussianStudyToolApp::OnQuit()
{
	delete m_mainMenuWidget;
	m_mainMenuWidget = nullptr;
	m_stateStack.OnEnd();
}

void RussianStudyToolApp::OnResizeWindow(int width, int height)
{
}

void RussianStudyToolApp::OnDropFiles(const Array<Path>& paths)
{
}

void RussianStudyToolApp::OnUpdate(float timeDelta)
{
	Mouse* mouse = GetMouse();
	Keyboard* keyboard = GetKeyboard();
	bool ctrl = keyboard->IsKeyDown(Keys::left_control) ||
		keyboard->IsKeyDown(Keys::right_control);
	bool shift = keyboard->IsKeyDown(Keys::left_shift) ||
		keyboard->IsKeyDown(Keys::right_shift);

	// Escape: pop state or quit
	if (keyboard->IsKeyPressed(Keys::escape))
	{
		m_stateStack.Pop();
	}
	if (!m_stateStack.IsActive())
	{
		Quit();
		return;
	}

	// F4: Toggle Fullscreen Mode
	if (keyboard->IsKeyPressed(Keys::f4))
		GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());

	m_stateStack.Update(timeDelta);

	if (m_joystick)
	{
		m_pedals[0].Update(m_joystick->GetAxisPosition(3));
		m_pedals[1].Update(m_joystick->GetAxisPosition(1));
		m_pedals[2].Update(m_joystick->GetAxisPosition(2));

		for (uint32 i = 0; i < 3; i++)
		{
			m_joystickButtonStatePrev[i] = m_joystickButtonState[i];
			m_joystickButtonState[i] = m_joystick->GetAxisPosition(i + 1) < 0.6f;
			m_joystickButtonPressed[i] =
				(m_joystickButtonState[i] && !m_joystickButtonStatePrev[i]);
		}
	}
}

void RussianStudyToolApp::OnRender()
{
	Window* window = GetWindow();
	MouseState mouseState = GetMouse()->GetMouseState();
	Vector2f windowSize((float) window->GetWidth(), (float) window->GetHeight());

	GetRenderDevice()->SetRenderParams(m_renderParams);
	GetRenderDevice()->ApplyRenderSettings();

	AppGraphics g(window);
	g.Clear(Color::BLACK);
	g.SetTransformation(Matrix4f::IDENTITY);
	g.SetWindowOrthoProjection();
	
	m_stateStack.Render(g, 0.0f);
	
	/*
	if (m_joystick)
	{
		auto state = m_joystick->GetState();
		for (uint32 i = 0; i < m_joystick->GetNumAxes(); i++)
		{
			std::stringstream ss;
			auto axis = m_joystick->GetAxisState(i);
			ss << "Axis " << i << " = " << axis.position;
			g.DrawString(m_font.get(), ss.str(), Vector2f(windowSize.x - 200, 80 + i * 30.0f),
				Color::YELLOW);
		}
		for (uint32 i = 0; i < 3; i++)
		{
			std::stringstream ss;
			auto& pedal = m_pedals[i];
			ss << "Pedal " << pedal.GetName() << " = " << pedal.GetAmount();
			if (pedal.IsDown())
				ss << " [down]";
			g.DrawString(m_font.get(), ss.str(), Vector2f(windowSize.x - 200, 300 + i * 30.0f),
				Color::YELLOW);
		}
	}
	*/
}

void RussianStudyToolApp::PopState()
{
	m_stateStack.Pop();
}

void RussianStudyToolApp::PushState(ApplicationState* state)
{
	m_stateStack.Push(state);
}

void RussianStudyToolApp::PushState(Widget* widget)
{
	m_stateStack.Push(new GUIState(widget));
}

