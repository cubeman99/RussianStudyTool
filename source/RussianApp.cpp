#include "RussianApp.h"
#include "Resources.h"
#include "states/GUIState.h"
#include "widgets/CardSearchWidget.h"
#include "widgets/TestWidget.h"
#include "widgets/CardSetEditWidget.h"
#include "widgets/CardSetBrowserWidget.h"
#include "widgets/CardListView.h"
#include "examples/ExampleDatabase.h"

// Comment this out to use the REAL card dataset
#ifdef _DEBUG
#define USE_TEST_DATA
#endif

static const Path g_assetsPath = "../assets";
static const PathU16 g_assetsPathU16 = u"../assets";

#ifdef USE_TEST_DATA
static const Path g_dataPath = g_assetsPath / "data";
static const PathU16 g_dataPathU16 = g_assetsPathU16 / PathU16(u"data");
#else
static const Path g_dataPath = "C:/workspace/python/russian-study-tool/data";
static const PathU16 g_dataPathU16 = u"C:/workspace/python/russian-study-tool/data";
#endif

RussianStudyToolApp* RussianStudyToolApp::s_instance = nullptr;


RussianStudyToolApp::RussianStudyToolApp() :
	m_studyDatabase(m_cardDatabase),
	m_exampleDatabase(m_wiktionary),
	m_wordDatabase(m_wiktionary),
	m_wiktionary(m_requests)
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

	// Connect signals
	GetEventManager()->Subscribe(this, &RussianStudyToolApp::OnKeyTyped);
	GetEventManager()->Subscribe(this, &RussianStudyToolApp::OnKeyDown);
	GetEventManager()->Subscribe(this, &RussianStudyToolApp::OnMouseDown);
	GetEventManager()->Subscribe(this, &RussianStudyToolApp::OnMouseUp);
}

RussianStudyToolApp::~RussianStudyToolApp()
{
}

void RussianStudyToolApp::OnInitialize()
{
	auto resourceManager = GetResourceManager();

	Path path = g_assetsPath / Path(Res::FONT);

	//resourceManager->LoadBuiltInFont(m_font, BuiltInFonts::FONT_CONSOLE);
	resourceManager->LoadFont(m_font, Res::FONT, 16, 0x20, 0x500);
	resourceManager->LoadFont(m_fontSmall, Res::FONT_SMALL, 14, 0x20, 0x500);
	resourceManager->LoadFont(m_fontLarge, Res::FONT_LARGE, 30, 0x20, 0x500);
	
	m_studyDatabase.SetStudyDataPath(g_dataPath / "study_data.json");
	m_wiktionary.SetDataPath(g_dataPath / "wiktionary.json");

	// Load all datasets
	m_exampleDatabase.LoadExamples(g_dataPathU16 / PathU16(u"examples"));
	if (m_studyDatabase.GetStudyDataPath().FileExists())
		m_studyDatabase.LoadStudyData();
	m_cardDatabase.LoadCardData(g_dataPath / "card_data.json");
	m_cardDatabase.LoadCardSets(g_dataPath / "cards");
	if (m_wiktionary.GetDataPath().FileExists())
		m_wiktionary.Load();

#ifdef USE_TEST_DATA
	CardSet::sptr cardSet = m_cardDatabase.GetCardSet(CardSetKey(u"conversation"));
#else
	//CardSet::sptr cardSet = m_cardDatabase.GetCardSet(CardSetKey(u"common words"));
	CardSet::sptr cardSet = m_cardDatabase.GetCardSet(CardSetKey(u"important phrases"));
#endif

	m_mainMenuWidget = new MainMenuWidget(m_cardDatabase.GetRootPackage());
	PushState(m_mainMenuWidget);
	//PushState(new Widget());
	//PushState(new CardSearchWidget());
	//PushState(new CardSetEditWidget(cardSet));
	//PushState(new StudyState(cardSet.get(), cardSet));
	//PushState(new CardListView(cardSet.get()));

	//m_cardDatabase.SaveAllCardSets();
	//m_cardDatabase.SaveCardData();

	m_stateStack.Begin(this);

	m_joystick = GetInputManager()->AddDevice<Joystick>(
		"Thrustmaster T300RS Racing Wheel");
}

void RussianStudyToolApp::OnQuit()
{
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
		if (!m_isJoystickReady)
		{
			// All axes seem to be zero until first input is detected
			for (uint32 axis = 0; axis < 3; axis++)
				m_isJoystickReady = (m_joystick->GetAxisPosition(axis) != 0);
		}
		else
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

//#ifdef _DEBUG
	String str;
	str = "Allocs: " + std::to_string(GUIObject::s_globalAllocationCount);
	str += ",  Connections: " + std::to_string(EventSignalBase::s_globalSignalConnectionCount);
	g.DrawString(m_font.get(), str, Vector2f(10, 10), Color::RED);
//#endif

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
	}*/
}

void RussianStudyToolApp::OnMouseDown(Window::MouseDownEvent* e)
{
	m_stateStack.OnMouseDown(e);
}

void RussianStudyToolApp::OnMouseUp(Window::MouseUpEvent * e)
{
	m_stateStack.OnMouseUp(e);
}

void RussianStudyToolApp::OnKeyDown(Window::KeyDownEvent* e)
{
	m_stateStack.OnKeyDown(e);
}

void RussianStudyToolApp::OnKeyTyped(Window::KeyTypedEvent* e)
{
	m_stateStack.OnKeyTyped(e);
}

void RussianStudyToolApp::PushState(Widget* widget)
{
	m_stateStack.Push(new GUIState(widget));
}

