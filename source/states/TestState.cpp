#include "TestState.h"
#include "Resources.h"
#include "gui/gui.h"
#include "MainMenuWidget.h"

void TestState::OnBegin()
{
	m_gui.SetApplication(GetApp());

	auto font = GetApp()->GetResourceManager()->Get<Font>(Res::FONT);
	auto layout = new VBoxLayout();

	auto mainLayout = new AnchorLayout();

	mainLayout->Add(new Label(u"Приве'т, ми'р!", font), Vector2f(0.5f, 0.0f), Vector2f(0.8f, 0.5f), Vector2f(0, 100));
	mainLayout->Add(new Label(u"Bottom Right", font), Vector2f(0.5f, 0.5f), Vector2f(0.5f, 0.5f), Vector2f(0, 50), TextAlign::BOTTOM_RIGHT);
	mainLayout->Add(new Label(u"Top Left", font), Vector2f(0.5f, 0.5f), Vector2f(0.5f, 0.5f), Vector2f(0, 0), TextAlign::TOP_LEFT);
	mainLayout->Add(new Label(u"Centered", font), Vector2f(0.5f, 0.5f), Vector2f(0.5f, 0.5f), Vector2f(0, 0), TextAlign::CENTERED);

	layout->Add(new Label("Hello, World!", font));
	layout->Add(new Label("Second widget!", font));
	layout->Add(new Label("THIRD WIDGET!!!!", font, Color::YELLOW));
	layout->Add(new Label("THIRD WIDGET!!!!", font, Color::YELLOW));
	layout->Add(new Label("THIRD WIDGET!!!!", font, Color::YELLOW));
	layout->Add(new Label("THIRD WIDGET!!!!", font, Color::YELLOW));
	layout->Add(new Label("THIRD WIDGET!!!!", font, Color::YELLOW));

	Widget* root = new Widget();
	mainLayout->Add(layout);
	root->SetLayout(layout);
	m_gui.SetRootWidget(root);
	CardSetPackage::sptr package = cmg::make_shared<CardSetPackage>("Card Package!!");
	package->AddCardSet(cmg::make_shared<CardSet>("Card Set 1"));
	package->AddCardSet(cmg::make_shared<CardSet>("Card Set 2"));
	package->AddPackage(cmg::make_shared<CardSetPackage>("Sub Package 1"));
	m_gui.SetRootWidget(new MainMenuWidget(package));
}

void TestState::OnEnd()
{
}

void TestState::OnUpdate(float timeDelta)
{
	m_gui.Update(timeDelta);
}

void TestState::OnRender(AppGraphics& g, float timeDelta)
{
	m_gui.Render(g, timeDelta);
}

