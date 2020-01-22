#include "RussianApp.h"
#include "Resources.h"

static const Path g_assetsPath = "../assets";
static const Path g_cardSetRootPath = "C:/workspace/python/russian-study-tool/data/cards";

RussianStudyToolApp::RussianStudyToolApp()
{
	SetConsoleOutputCP(CP_UTF8);

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

	// m_studyDatabase.LoadStudyData(g_assetsPath / "study_data.json");
	// m_cardDatabase.LoadCardData(g_assetsPath / "cards.json");
	// m_cardDatabase.LoadCardSets(g_cardSetRootPath);

	StudySet::sptr studySet = m_cardDatabase.GetCardSet(CardSetKey(u"cars & automotive"));
	m_state = new TestState();
	//m_state = new StudyState(nullptr);
	m_state->Begin(this);

	m_joystick = GetInputManager()->AddDevice<Joystick>(
		"Thrustmaster T300RS Racing Wheel");
}

void RussianStudyToolApp::OnQuit()
{
	m_state->OnEnd();
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

	// Escape: Quit
	if (keyboard->IsKeyPressed(Keys::escape))
	{
		Quit();
		return;
	}

	// F4: Toggle Fullscreen Mode
	if (keyboard->IsKeyPressed(Keys::f4))
		GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());

	m_state->Update(timeDelta);

	if (m_joystick)
	{
		for (uint32 i = 0; i < 3; i++)
		{
			m_joystickButtonStatePrev[i] = m_joystickButtonState[i];
			m_joystickButtonState[i] = m_joystick->m_axes[i + 1]->x < 0.6f;
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
	
	m_state->Render(g, 0.0f);
	
	if (m_joystick)
	{
		auto state = m_joystick->GetState();
		for (uint32 i = 0; i < m_joystick->m_axes.size(); i++)
		{
			std::stringstream ss;
			ss << "Axis " << i << ": " << m_joystick->m_axes[i]->x;
			g.DrawString(m_font.get(), ss.str(), Vector2f(24, 180 + i * 30.0f),
				Color::YELLOW);
		}
	}

	//AccentedText text(u"Приве'т, ми'р! Don't go?");
	//g.DrawString(m_font.get(), "Hello, world!", Vector2f(24, 24));
	//g.DrawString(m_font.get(), u"Привет, мир!", Vector2f(24, 24));
	//g.DrawAccentedText(m_font.get(), text, Vector2f(24, 24));
	//g.DrawTexture(m_font->GetGlyphTexture(), Vector2f(50, 100));
}

