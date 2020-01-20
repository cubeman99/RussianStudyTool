#include "StudyState.h"
#include "Resources.h"
#include "RussianApp.h"


void StudyState::OnBegin()
{
	NextCard();
}

void StudyState::OnEnd()
{
}

void StudyState::OnUpdate(float timeDelta)
{
	Keyboard* keyboard = GetKeyboard();
	bool ctrl = keyboard->IsKeyDown(Keys::left_control) ||
		keyboard->IsKeyDown(Keys::right_control);
	bool shift = keyboard->IsKeyDown(Keys::left_shift) ||
		keyboard->IsKeyDown(Keys::right_shift);

	// Mark as did know
	if (keyboard->IsKeyPressed(Keys::enter) ||
		((RussianStudyToolApp*) GetApp())->m_joystickButtonPressed[1])
	{
		NextCard();
	}

	// Mark as didn't know
	if (keyboard->IsKeyPressed(Keys::backspace) ||
		((RussianStudyToolApp*) GetApp())->m_joystickButtonPressed[0])
	{
		if (m_isRevealed)
		{
			NextCard();
		}
		else
		{
			m_isRevealed = true;
		}
	}
	
}

void StudyState::OnRender(AppGraphics& g, float timeDelta)
{
	Vector2f windowSize(
		(float) GetWindow()->GetWidth(),
		(float) GetWindow()->GetHeight());

	if (m_card)
	{
		Vector2f pos = windowSize * 0.5f;
		auto font = GetApp()->GetResourceManager()->Get<Font>(Res::FONT);
		g.DrawAccentedText(font.get(), m_card->GetText(m_shownSide), pos + Vector2f(24, 30), Color::WHITE, TextAlign::CENTERED);
		g.DrawString(font.get(), EnumToString(m_card->GetWordType()), pos + Vector2f(24, -50), Color::GRAY, TextAlign::CENTERED);

		auto hiddenSide = (m_shownSide == Language::k_russian ?
			Language::k_english : Language::k_russian);
		if (m_isRevealed)
			g.DrawAccentedText(font.get(), m_card->GetText(hiddenSide), pos + Vector2f(24, 60), Color::WHITE, TextAlign::CENTERED);
	}
}

void StudyState::NextCard()
{
	m_isRevealed = false;
	m_card = nullptr;
	if (m_studySet)
		m_card = m_scheduler.NextCard();
	m_shownSide = Language::k_russian;
}
