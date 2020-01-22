#include "TestState.h"
#include "Resources.h"
#include "gui/gui.h"

void TestState::OnBegin()
{
	m_gui.SetApplication(GetApp());

	auto font = GetApp()->GetResourceManager()->Get<Font>(Res::FONT);
	auto layout = new VBoxLayout();
	layout->Add(new Label("Hello, World!", font));
	layout->Add(new Label("Second widget!", font));
	layout->Add(new Label("THIRD WIDGET!!!!", font, Color::YELLOW));
	Widget* root = new Widget();
	root->SetLayout(layout);
	m_gui.SetRootWidget(root);
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

