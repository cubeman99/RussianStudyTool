#include "StudyState.h"
#include "Resources.h"
#include "RussianApp.h"
#include "widgets/MenuWidget.h"
#include "widgets/RelatedCardsWidget.h"
#include "widgets/AddCardToSetWidget.h"

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

	m_widgetUnrevealed.SetLayout(&m_layoutUnrevealed);
	m_widgetRevealed.SetLayout(&m_layoutRevealed);
	m_anchorLayout.Add(&m_widgetUnrevealed);
	m_anchorLayout.Add(&m_widgetRevealed);

	m_anchorLayout.Add(&m_labelHistoryScore)
		.Pin(0.5f, 0.0f, TextAlign::TOP_CENTER).Offset(0.0f, topTopStart);
	m_anchorLayout.Add(&m_labelEncounterTime)
		.Pin(1.0f, 0.0f, TextAlign::TOP_RIGHT).Offset(-16.0f, topTopStart);
	m_anchorLayout.Add(&m_labelWordType)
		.Pin(0.5f, 0.2f, TextAlign::CENTERED);
	m_anchorLayout.Add(&m_labelCardTextShown)
		.Pin(0.5f, 0.5f, TextAlign::BOTTOM_CENTER).Offset(0, -20);
	m_layoutRevealed.Add(&m_labelCardTextRevealed)
		.Pin(0.5f, 0.5f, TextAlign::TOP_CENTER).Offset(0, 20);
	m_layoutUnrevealed.Add(&m_widgetTagsShown)
		.Pin(0.5f, 0.5f, TextAlign::BOTTOM_CENTER).Offset(0, -80);
	m_layoutRevealed.Add(&m_widgetTagsRevealed)
		.Pin(0.5f, 0.5f, TextAlign::BOTTOM_CENTER).Offset(0, -80);
	m_anchorLayout.Add(&m_proficiencyBarTop)
		.Pin(0.0f, 0.0f, 1.0f, 0.0f)
		.Offset(0.0f, 0.0f, 0.0f, barHeight);
	m_anchorLayout.Add(&m_proficiencyBarBottom)
		.Pin(0.0f, 1.0f, 1.0f, 1.0f)
		.Offset(0.0f, -barHeight, 0.0f, 0.0f);
	m_layoutRevealed.Add(&m_layoutDefinitions)
		.Pin(0.0f, 1.0f, TextAlign::BOTTOM_LEFT)
		.Offset(8.0f, -barHeight - 16);
	m_layoutRevealed.Add(&m_widgetDeclensionTable)
		.Pin(1.0f, 1.0f, TextAlign::BOTTOM_RIGHT)
		.Offset(-8.0f, -barHeight - 16);

	m_layoutDefinitions.SetSpacing(0.0f);
	m_layoutDefinitions.SetMargins(0.0f);
	m_tableDeclension.SetSpacing(1.0f);
	m_tableDeclension.SetMargins(1.0f);
	m_widgetDeclensionTable.SetLayout(&m_tableDeclension);
	m_widgetDeclensionTable.SetBackgroundColor(
		GUIConfig::color_background_light);

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
	AddKeyShortcut("i", [this]() { OpenInWebBrowser(); return true; });
}

void StudyState::OnInitialize()
{
	Font::sptr fontLarge = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_LARGE);
	Font::sptr fontSmall = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);

	m_labelCardTextRevealed.SetFont(fontLarge);
	m_labelCardTextShown.SetFont(fontLarge);


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
	m_widgetRevealed.SetVisible(true);
	m_widgetUnrevealed.SetVisible(false);
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
	
	Font::sptr fontSmall = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);

	// Wiktionary word
	m_term = app->GetWiktionary().GetTerm(m_card->GetRuKey().russian);
	m_wikiWord = nullptr;
	m_layoutDefinitions.Clear();
	m_widgetDeclensionTable.SetVisible(false);
	m_tableDeclension.Clear();
	if (m_term)
	{
		m_wikiWord = m_term->GetWord(m_card->GetWordType());
		if (m_wikiWord)
		{
			m_widgetDeclensionTable.SetVisible(true);
			m_layoutDefinitions.Add(new Label(
				m_wikiWord->GetText() + AccentedText(u":"), fontSmall));
			uint32 number = 1;
			for (auto definition : m_wikiWord->GetDefinitions())
			{
				//using unistringstream = std::basic_stringstream<char16_t, std::char_traits<char16_t>, std::allocator<char16_t>>;
				std::stringstream ss;
				ss << number << ". ";
				AccentedText text = ss.str() + definition.GetDefinition();
				m_layoutDefinitions.Add(new Label(text, fontSmall));
				number++;
			}

			if (m_wikiWord->GetWordType() == WordType::k_noun)
			{
				wiki::Noun::sptr noun = std::dynamic_pointer_cast<wiki::Noun>(m_wikiWord);
				Label* label;
				// Create table header
				for (Plurality plurality : EnumValues<Plurality>())
				{
					label = new Label(EnumToShortString(plurality) + ".", fontSmall),
					label->SetBackgroundColor(GUIConfig::color_background_alternate);
					m_tableDeclension.Add(label, 0, 1 + (uint32) plurality);
				}
				for (Case nounCase : EnumValues<Case>())
				{
					label = new Label(EnumToShortString(nounCase) + ".", fontSmall),
					label->SetBackgroundColor(GUIConfig::color_background_alternate);
					m_tableDeclension.Add(label, 1 + (uint32) nounCase, 0);
				}

				// Fill table body
				for (Plurality plurality : EnumValues<Plurality>())
				{
					String pluralityName = EnumToShortString(plurality);
					for (Case nounCase : EnumValues<Case>())
					{
						String caseName = EnumToShortString(nounCase);
						AccentedText text = noun->GetDeclension()
							.GetDeclension(nounCase, plurality);
						label = new Label(text, fontSmall);
						label->SetBackgroundColor(GUIConfig::color_background);
						m_tableDeclension.Add(label,
							1 + (uint32) nounCase, 1 + (uint32) plurality);
					}
				}
			}
		}
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
	m_widgetRevealed.SetVisible(false);
	m_widgetUnrevealed.SetVisible(true);
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
	profColor.a = 128;
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
	GetApp()->PushState(new AddCardToSetWidget(m_card));
}

void StudyState::OpenInWebBrowser()
{
	if (m_term)
	{
		std::stringstream ss;
		unistr url = u"https://en.wiktionary.org/wiki/" +
			m_term->GetText().GetString();
		ShellExecuteW(0, 0, (wchar_t*) url.c_str(), 0, 0, SW_SHOW);
		//cmg::os::OpenWebBrowser()
		//cmg::os::OpenDirectoryExplorer()
		//cmg::os::SetClipboardText()
		//cmg::os::GetClipboardText()
	}
}
