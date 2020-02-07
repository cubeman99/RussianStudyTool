#include "StudyState.h"
#include "Resources.h"
#include "RussianApp.h"
#include "widgets/MenuWidget.h"
#include "widgets/RelatedCardsWidget.h"

StudyState::StudyState(IStudySet* studySet, const StudyParams& studyParams) :
	m_studySet(studySet),
	m_studyParams(studyParams)
{
	float barHeight = 32.0f;
	float topTopStart = barHeight + 16.0f;

	m_labelTitle.SetAlign(TextAlign::MIDDLE_LEFT);
	m_labelTitle.SetText(studySet->GetName());
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleWidget.SetLayout(&m_titleLayout);
	m_titleLayout.Add(&m_labelTitle);
	m_titleLayout.Add(&m_topProficiencyBar);

	m_layoutTagsRevealed.SetMargins(0.0f);
	m_layoutTagsRevealed.SetSpacing(4.0f);
	m_layoutTagsShown.SetMargins(0.0f);
	m_layoutTagsShown.SetSpacing(4.0f);
	m_widgetTagsShown.SetLayout(&m_layoutTagsShown);
	m_widgetTagsRevealed.SetLayout(&m_layoutTagsRevealed);

	m_anchorLayout.Add(&m_labelHistoryScore, Vector2f(0.5f, 0.0f), Vector2f(0.5f, 0.0f), Vector2f(0.0f, topTopStart), TextAlign::TOP_CENTER);
	m_anchorLayout.Add(&m_labelEncounterTime, Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector2f(-16.0f, topTopStart), TextAlign::TOP_RIGHT);
	m_anchorLayout.Add(&m_labelWordType, Vector2f(0.5f, 0.2f), Vector2f(0.5f, 0.2f), Vector2f::ZERO, TextAlign::CENTERED);
	m_anchorLayout.Add(&m_labelCardTextShown, Vector2f(0.5f), Vector2f(0.5f), Vector2f(0, -40), TextAlign::BOTTOM_CENTER);
	m_anchorLayout.Add(&m_labelCardTextRevealed, Vector2f(0.5f), Vector2f(0.5f), Vector2f(0, 40), TextAlign::TOP_CENTER);
	m_anchorLayout.Add(&m_widgetTagsShown, Vector2f(0.5f), Vector2f(0.5f), Vector2f(0, -80), TextAlign::BOTTOM_CENTER);
	m_anchorLayout.Add(&m_widgetTagsRevealed, Vector2f(0.5f), Vector2f(0.5f), Vector2f(0, -80), TextAlign::BOTTOM_CENTER);
	m_anchorLayout.Add(&m_proficiencyBarTop,
		Vector2f(0.0f, 0.0f), Vector2f(1.0f, 0.0f),
		Vector2f(0.0f, 0.0f), Vector2f(0.0f, barHeight));
	m_anchorLayout.Add(&m_proficiencyBarBottom,
		Vector2f(0.0f, 1.0f), Vector2f(1.0f, 1.0f),
		Vector2f(0.0f, -barHeight), Vector2f(0.0f, 0.0f));

	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.Add(&m_titleWidget, 0.1f);
	m_mainLayout.Add(&m_anchorLayout, 1.0f);
	SetLayout(&m_mainLayout);

	SetBackgroundColor(GUIConfig::color_background);

	m_labelCardTextShown.SetAlign(TextAlign::BOTTOM_CENTER);
	m_labelWordType.SetAlign(TextAlign::TOP_CENTER);
	m_labelHistoryScore.SetAlign(TextAlign::TOP_CENTER);
	m_labelEncounterTime.SetAlign(TextAlign::TOP_CENTER);
	m_labelCardTextRevealed.SetAlign(TextAlign::TOP_CENTER);
	m_labelCardTextShown.SetColor(Color::WHITE);
	m_labelCardTextRevealed.SetColor(Color::WHITE);
	m_labelWordType.SetColor(Color::WHITE);
	m_labelEncounterTime.SetColor(Color::WHITE);
	m_labelHistoryScore.SetColor(Color::WHITE);

	// Connect signals
	AddKeyShortcut("e", [this]() { OpenCardEditView(); return true; });
	AddKeyShortcut("r", [this]() { OpenRelatedCardsView(); return true; });
	AddKeyShortcut("a", [this]() { OpenAddCardToSetView(); return true; });
	AddKeyShortcut("enter", [this]() { MarkGoodAndNext(); return true; });
	AddKeyShortcut("backspace", [this]() { RevealOrMarkBadAndNext(); return true; });
	//AddKeyShortcut("escape", [this]() { ShowPauseMenu(); return true; });
}

void StudyState::OnInitialize()
{
	m_scheduler = new Scheduler(
		GetApp()->GetStudyDatabase(), m_studySet, m_studyParams);
	NextCard();
}

void StudyState::OnUninitialize()
{
	delete m_scheduler;
	m_scheduler = nullptr;
}

void StudyState::OnUpdate(float timeDelta)
{
	auto app = GetApp();
	auto& studyDatabase = app->GetStudyDatabase();

	m_topProficiencyBar.SetMetrics(
		studyDatabase.GetStudySetMetrics(m_studySet));

	// Pedal callbacks
	if (app->GetMiddlePedalInput().IsPressed())
		ShowPauseMenu();
	else if (app->GetRightPedalInput().IsPressed())
		MarkGoodAndNext();
	else if (app->GetLeftPedalInput().IsPressed())
		RevealOrMarkBadAndNext();
}

void StudyState::OnRender(AppGraphics& g, float timeDelta)
{
	auto app = (RussianStudyToolApp*) GetApp();
	Vector2f windowSize(
		(float) app->GetWindow()->GetWidth(),
		(float) app->GetWindow()->GetHeight());
}

void StudyState::MarkCard(bool knewIt)
{
	auto app = (RussianStudyToolApp*) GetApp();
	m_scheduler->MarkCard(m_card, knewIt);
	app->GetStudyDatabase().SaveStudyData();
}

void StudyState::Reveal()
{
	m_isRevealed = true;
	m_labelCardTextRevealed.SetVisible(true);
	m_widgetTagsRevealed.SetVisible(true);
	m_widgetTagsShown.SetVisible(false);
}

void StudyState::NextCard()
{
	auto app = (RussianStudyToolApp*) GetApp();

	// Get the next card
	m_card = nullptr;
	if (m_studySet)
		m_card = m_scheduler->NextCard();
	if (m_card == nullptr)
	{
		Close();
		return;
	}
	
	m_cardStudyData = app->GetStudyDatabase().GetCardStudyData(m_card);
	m_isRevealed = false;
	if (Random::NextBool())
	{
		m_shownSide = Language::k_russian;
		m_revealedSide = Language::k_english;
	}
	else
	{
		m_shownSide = Language::k_english;
		m_revealedSide = Language::k_russian;
	}
	CMG_LOG_INFO() << "Showing card: " << m_card->GetText(m_shownSide);

	// Card tag boxes
	m_layoutTagsShown.Clear();
	m_layoutTagsRevealed.Clear();
	Label* tagLabel = nullptr;
	for (auto it : m_card->GetTags())
	{
		if (it.second)
		{
			CardTags tag = it.first;
			if (IsKeyCardTag(tag))
			{
				tagLabel = new Label(EnumToString(tag));
				tagLabel->SetColor(Color::WHITE);
				tagLabel->SetBackgroundColor(Config::GetCardTagColor(tag));
				m_layoutTagsShown.Add(tagLabel);
			}
			tagLabel = new Label(EnumToString(tag));
			tagLabel->SetColor(Color::WHITE);
			tagLabel->SetBackgroundColor(Config::GetCardTagColor(tag));
			m_layoutTagsRevealed.Add(tagLabel);
		}
	}
	
	m_labelCardTextShown.SetText(m_card->GetText(m_shownSide));
	m_labelCardTextRevealed.SetText(m_card->GetText(m_revealedSide));
	m_labelCardTextRevealed.SetVisible(false);
	m_widgetTagsRevealed.SetVisible(false);
	m_widgetTagsShown.SetVisible(true);
	m_labelWordType.SetText(EnumToString(m_card->GetWordType()));

	std::stringstream ss;
	ss << m_cardStudyData.GetHistoryScore();
	m_labelHistoryScore.SetText(ss.str());
	ss.str("");

	if (m_cardStudyData.IsEncountered())
	{
		AppTimestamp elapsedTime = GetAppTimestamp() -
			m_cardStudyData.GetLastEncounterTime();
		String units = "second";
		if (elapsedTime > 60)
		{
			units = "minute";
			elapsedTime /= 60;
			if (elapsedTime > 60)
			{
				units = "hour";
				elapsedTime /= 60;
				if (elapsedTime > 24)
				{
					units = "day";
					elapsedTime /= 24;
				}
			}
		}
		int intElapsedTime = int(elapsedTime + 0.5);
		ss << intElapsedTime << " " << units;
		if (intElapsedTime != 1)
			ss << "s";
		ss << " ago";
		m_labelEncounterTime.SetText(ss.str());
	}
	else
	{
		m_labelEncounterTime.SetText("");
	}

	Color profColor = Config::GetProficiencyLevelColor(
		m_cardStudyData.GetProficiencyLevel());
	m_proficiencyBarTop.SetBackgroundColor(profColor);
	m_proficiencyBarBottom.SetBackgroundColor(profColor);
}

void StudyState::MarkGoodAndNext()
{
	MarkCard(true);
	NextCard();
}

void StudyState::RevealOrMarkBadAndNext()
{
	if (m_isRevealed)
	{
		MarkCard(false);
		NextCard();
	}
	else
	{
		Reveal();
	}
}

void StudyState::ShowPauseMenu()
{
	MenuWidget* menu = new MenuWidget("Options");
	menu->AddCancelOption("Resume");
	menu->AddMenuOption("Edit Card", true,
		new MethodDelegate(this, &StudyState::OpenCardEditView));
	menu->AddMenuOption("Edit Related Card", true,
		new MethodDelegate(this, &StudyState::OpenRelatedCardsView));
	menu->AddMenuOption("Add to Card Sets", true,
		new MethodDelegate(this, &StudyState::OpenAddCardToSetView));
	//menu->AddMenuOption("Edit Card Set", nullptr); 
	menu->AddMenuOption("Main Menu", true,
		new MethodDelegate((Widget*) this, &Widget::Close));
	GetApp()->PushState(menu);
}

void StudyState::OpenCardEditView()
{
}

void StudyState::OpenRelatedCardsView()
{
	GetApp()->PushState(new RelatedCardsWidget(m_card));
}

void StudyState::OpenAddCardToSetView()
{
}
