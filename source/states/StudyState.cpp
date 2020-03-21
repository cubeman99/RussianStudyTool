#include "StudyState.h"
#include "Resources.h"
#include "RussianApp.h"
#include "widgets/MenuWidget.h"
#include "widgets/RelatedCardsWidget.h"
#include "widgets/AddCardToSetWidget.h"
#include "widgets/CardEditWidget.h"
#include "widgets/ExampleTextWidget.h"

StudyState::StudyState(IStudySet* studySet, CardSet::sptr cardSet, const StudyParams& studyParams) :
	m_studySet(studySet),
	m_cardSet(cardSet),
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

	m_listCounterparts.SetLabel("Counterparts:");
	m_listRelatedCards.SetLabel("Related Cards:");
	m_listRelatedTerms.SetLabel("Related Terms:");
	m_listDerivedTerms.SetLabel("Derived Terms:");
	m_listSynonyms.SetLabel("Synonyms:");
	m_listAntonyms.SetLabel("Antonyms:");
		
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

	m_anchorLayout.Add(&m_proficiencyBarTop)
		.Pin(0.0f, 0.0f, 1.0f, 0.0f)
		.Offset(0.0f, 0.0f, 0.0f, barHeight);
	m_anchorLayout.Add(&m_proficiencyBarBottom)
		.Pin(0.0f, 1.0f, 1.0f, 1.0f)
		.Offset(0.0f, -barHeight, 0.0f, 0.0f);
	m_anchorLayout.Add(&m_studyHistoryTimeline)
		.Pin(0.0f, 0.0f, 0.3f, 0.0f)
		.Offset(0.0f, 0.0f, 0.0f, barHeight);
	m_anchorLayout.Add(&m_labelWordType)
		.Pin(0.5f, 0.0f, TextAlign::CENTERED)
		.Offset(0.0f, 0.0f, 0.0f, barHeight);
	m_anchorLayout.Add(&m_labelEncounterTime)
		.Pin(1.0f, 0.0f, TextAlign::MIDDLE_RIGHT)
		.Offset(0.0f, 0.0f, 0.0f, barHeight);
	m_layoutUnrevealed.Add(&m_widgetTagsShown)
		.Pin(0.5f, 0.5f, TextAlign::BOTTOM_CENTER).Offset(0, -80);
	m_layoutRevealed.Add(&m_widgetTagsRevealed)
		.Pin(0.5f, 0.5f, TextAlign::BOTTOM_CENTER).Offset(0, -80);
	m_layoutRevealed.Add(&m_layoutDefinitions)
		.Pin(0.0f, 1.0f, TextAlign::BOTTOM_LEFT)
		.Offset(8.0f, -barHeight - 16);
	m_layoutRevealed.Add(&m_widgetNounInfo)
		.Pin(1.0f, 1.0f, TextAlign::BOTTOM_RIGHT)
		.Offset(-8.0f, -barHeight - 16);
	m_layoutRevealed.Add(&m_widgetAdjectiveInfo)
		.Pin(1.0f, 1.0f, TextAlign::BOTTOM_RIGHT)
		.Offset(-8.0f, -barHeight - 16);
	m_layoutRevealed.Add(&m_widgetVerbInfo)
		.Pin(1.0f, 1.0f, TextAlign::BOTTOM_RIGHT)
		.Offset(-8.0f, -barHeight - 16);
	m_layoutRevealed.Add(&m_layoutCardSets)
		.Pin(1.0f, 0.0f, TextAlign::TOP_RIGHT).Offset(-16.0f, topTopStart);
	m_layoutRevealed.Add(&m_layoutExamples)
		.Pin(0.0f, 0.0f, TextAlign::TOP_LEFT).Offset(16.0f, topTopStart);

	m_anchorLayout.Add(&m_labelCardTextShown)
		.Pin(0.5f, 0.5f, TextAlign::BOTTOM_CENTER).Offset(0, -20);
	m_layoutRevealed.Add(&m_labelCardTextRevealed)
		.Pin(0.5f, 0.5f, TextAlign::TOP_CENTER).Offset(0, 20);

	m_layoutNounInfo.SetMargins(0.0f);
	m_layoutNounInfo.Add(&m_tableNounDeclension);
	m_widgetNounInfo.SetLayout(&m_layoutNounInfo);

	m_layoutAdjectiveInfo.SetMargins(0.0f);
	m_layoutAdjectiveInfo.Add(&m_tableAdjectiveDeclension);
	m_widgetAdjectiveInfo.SetLayout(&m_layoutAdjectiveInfo);

	m_layoutVerbParticiples.SetMargins(0.0f);
	m_layoutVerbParticiples.SetSpacing(4.0f);
	m_layoutVerbParticiples.Add(&m_labelInfinitive);
	m_layoutVerbParticiples.Add(&m_listCounterparts);
	m_layoutVerbParticiples.AddStretch();
	m_layoutVerbParticiples.Add(&m_tableVerbParticiples);
	m_layoutVerbInfo.Add(&m_tableVerbNonPast);
	m_layoutVerbInfo.Add(&m_tableVerbPast);
	m_layoutVerbInfo.Add(&m_layoutVerbParticiples);
	m_layoutVerbInfo.SetMargins(0.0f);
	m_layoutVerbInfo.SetSpacing(8.0f);
	m_widgetVerbInfo.SetLayout(&m_layoutVerbInfo);

	m_layoutDefinitions.SetSpacing(0.0f);
	m_layoutDefinitions.SetMargins(0.0f);

	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.Add(&m_titleWidget, 0.1f);
	m_mainLayout.Add(&m_anchorLayout, 1.0f);
	SetLayout(&m_mainLayout);

	SetBackgroundColor(GUIConfig::color_background);

	m_labelWordType.SetAlign(TextAlign::CENTERED);
	m_labelEncounterTime.SetAlign(TextAlign::MIDDLE_RIGHT);
	m_labelCardTextShown.SetAlign(TextAlign::BOTTOM_CENTER);
	m_labelCardTextRevealed.SetAlign(TextAlign::TOP_CENTER);

	// Connect signals
	AddKeyShortcut("e", [this]() { OpenCardEditView(m_card); return true; });
	AddKeyShortcut("r", [this]() { OpenRelatedCardsView(m_card); return true; });
	AddKeyShortcut("s", [this]() { OpenAddCardToSetView(m_card); return true; });
	AddKeyShortcut("o", [this]() { ReshowCard(); return true; });
	AddKeyShortcut("enter", [this]() { MarkGoodAndNext(); return true; });
	AddKeyShortcut("backspace", [this]() { RevealOrMarkBadAndNext(); return true; });
	//AddKeyShortcut("escape", [this]() { ShowPauseMenu(); return true; });
	AddKeyShortcut("i", [this]() { OpenCardInWebBrowser(); return true; });
	AddKeyShortcut("Ctrl+C", [this]() { Copy(); return true; });
}

void StudyState::OnInitialize()
{
	Font::sptr fontLarge = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_LARGE);
	Font::sptr fontSmall = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);

	m_labelCardTextRevealed.SetFont(fontLarge);
	m_labelCardTextShown.SetFont(fontLarge);
	m_labelInfinitive.SetFont(fontSmall);

	// Connect signals
	auto& cardDatabase = GetApp()->GetCardDatabase();
	cardDatabase.CardDataChanged().Connect(this, &StudyState::OnCardDataChanged);
	cardDatabase.CardAddedToSet().Connect(this, &StudyState::OnCardAddedOrRemovedFromSet);
	cardDatabase.CardRemovedFromSet().Connect(this, &StudyState::OnCardAddedOrRemovedFromSet);

	m_scheduler = new Scheduler(GetApp()->GetStudyDatabase());
	m_scheduler->Begin(m_studySet, m_studyParams);
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
	m_scheduler->MarkCard(m_card, knewIt, m_shownSide);
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
	auto& cardDatabase = app->GetCardDatabase();

	// Get the next card
	Card::sptr card;
	if (m_studySet)
		card = m_scheduler->NextItem();
	if (!card)
	{
		Close();
		return;
	}
	Language shownSide = Language::k_russian;
	if (Random::NextBool())
		shownSide = Language::k_russian;
	else
		shownSide = Language::k_english;

	ShowCard(card, shownSide);
}

void StudyState::ReshowCard()
{
	ShowCard(m_card, m_shownSide);
}

void StudyState::ShowCard(Card::sptr card, Language shownSide)
{
	auto app = (RussianStudyToolApp*) GetApp();
	auto& cardDatabase = app->GetCardDatabase();
	auto& exampleDatabase = app->GetExampleDatabase();
	auto& wordDatabase = app->GetWordDatabase();
	m_card = card;
	m_cardTags = card->GetTags();
	m_term = nullptr;
	m_wikiWord = nullptr;
	m_cardStudyData = app->GetStudyDatabase().GetCardStudyData(m_card);
	m_shownSide = shownSide;
	m_revealedSide = (shownSide == Language::k_english ?
		Language::k_russian : Language::k_english);
	m_isRevealed = false;
	m_widgetRevealed.SetVisible(false);
	m_widgetUnrevealed.SetVisible(true);
	CMG_LOG_INFO() << "Showing card: " << m_card->GetText(m_shownSide);

	// Get wiktionary word
	m_term = nullptr;
	m_wikiWord = nullptr;
	CardWordMatch wordMatch = wordDatabase.GetWordFromCard(m_card, true, false);
	m_term = wordMatch.GetTerm();
	m_wikiWord = wordMatch.GetWord();

	Font::sptr fontSmall = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);

	m_layoutDefinitions.Clear();
	m_widgetNounInfo.SetVisible(false);
	m_widgetAdjectiveInfo.SetVisible(false);
	m_widgetVerbInfo.SetVisible(false);

	// Related cards
	m_listRelatedCards.Clear();
	if (!m_card->GetRelatedCards().empty())
	{
		for (Card::sptr relatedCard : m_card->GetRelatedCards())
		{
			RelatedWordWidget* widget =
				m_listRelatedCards.AddWord(relatedCard->GetRussian());
			widget->Clicked().Connect(this, widget, &StudyState::OnClickWordBox);
		}
		m_layoutDefinitions.Add(&m_listRelatedCards);
	}

	// Examples
	m_layoutExamples.Clear();
	Array<SentenceMatch> exampleMatches =
		exampleDatabase.GetExampleSentences(m_card);
	uint32 exampleCount = Math::Min(exampleMatches.size(),
		Config::k_maxExamplesToDisplay);
	for (uint32 i = 0; i < exampleCount; i++)
	{
		SentenceMatch& match = exampleMatches[i];
		ExampleTextWidget* exampleLabel = AllocateObject<ExampleTextWidget>(match);
		exampleLabel->SetFont(fontSmall);
		m_layoutExamples.Add(exampleLabel);
	}
	Label* exampleCountLabel = AllocateObject<Label>(
		std::to_string(exampleMatches.size()) + " total examples");
	exampleCountLabel->SetFont(fontSmall);
	exampleCountLabel->SetColor(GUIConfig::color_text_box_background_text);
	m_layoutExamples.Add(exampleCountLabel);

	if (m_wikiWord)
	{
		// Get card tags from the wiki word, if relevant
		if (m_wikiWord->GetWordType() == m_card->GetWordType())
			m_cardTags = (m_cardTags | m_wikiWord->GetTags());

		// Word-specific info and declensions
		if (m_wikiWord->GetWordType() == WordType::k_noun)
		{
			wiki::Noun::sptr noun =
				std::dynamic_pointer_cast<wiki::Noun>(m_wikiWord);
			m_widgetNounInfo.SetVisible(true);
			m_tableNounDeclension.InitNoun(noun->GetDeclension());
		}
		else if (m_wikiWord->GetWordType() == WordType::k_adjective)
		{
			wiki::Adjective::sptr adjective =
				std::dynamic_pointer_cast<wiki::Adjective>(m_wikiWord);
			m_widgetAdjectiveInfo.SetVisible(true);
			m_tableAdjectiveDeclension.InitAdjective(adjective->GetDeclension());
		}
		else if (m_wikiWord->GetWordType() == WordType::k_verb)
		{
			wiki::Verb::sptr verb =
				std::dynamic_pointer_cast<wiki::Verb>(m_wikiWord);
			m_widgetVerbInfo.SetVisible(true);

			// Conjugation tables
			m_tableVerbNonPast.InitVerbNonPast(verb->GetConjugation());
			m_tableVerbPast.InitVerbPastAndImperative(verb->GetConjugation());
			m_tableVerbParticiples.InitVerbParticiples(verb->GetConjugation());

			// Infinitive
			AccentedText infinitiveText = u"Infinitive: ";
			infinitiveText += verb->GetConjugation().GetInfinitive();
			infinitiveText += " (";
			infinitiveText += EnumToShortString(verb->GetConjugation().GetAspect ());
			infinitiveText += ")";
			m_labelInfinitive.SetText(infinitiveText);

			// Counterparts
			m_listCounterparts.Clear();
			for (const AccentedText& counterpart : verb->GetCounterparts())
			{
				RelatedWordWidget* widget = m_listCounterparts.AddWord(counterpart);
				widget->Clicked().Connect(this, widget, &StudyState::OnClickWordBox);
			}
		}

		// Related terms
		if (PopulateTermList(m_listRelatedTerms, m_wikiWord->GetRelatedTerms()))
			m_layoutDefinitions.Add(&m_listRelatedTerms);
		if (PopulateTermList(m_listDerivedTerms, m_wikiWord->GetDerivedTerms()))
			m_layoutDefinitions.Add(&m_listDerivedTerms);
		if (PopulateTermList(m_listSynonyms, m_wikiWord->GetSynonyms()))
			m_layoutDefinitions.Add(&m_listSynonyms);
		if (PopulateTermList(m_listAntonyms, m_wikiWord->GetAntonyms()))
			m_layoutDefinitions.Add(&m_listAntonyms);

		// Add definitions
		m_wordDefinitionWidget.SetWord(m_wikiWord);
		m_layoutDefinitions.Add(&m_wordDefinitionWidget);
	}

	// Card tag boxes
	m_layoutTagsShown.Clear();
	m_layoutTagsRevealed.Clear();
	Label* tagLabel = nullptr;
	for (auto it : m_cardTags)
	{
		if (it.second)
		{
			CardTags tag = it.first;
			if (IsKeyCardTag(tag))
			{
				tagLabel = AllocateObject<Label>(EnumToString(tag));
				tagLabel->SetColor(Color::WHITE);
				tagLabel->SetBackgroundColor(Config::GetCardTagColor(tag));
				m_layoutTagsShown.Add(tagLabel);
			}
			tagLabel = AllocateObject<Label>(EnumToString(tag));
			tagLabel->SetColor(Color::WHITE);
			tagLabel->SetBackgroundColor(Config::GetCardTagColor(tag));
			m_layoutTagsRevealed.Add(tagLabel);
		}
	}
	
	// Card set list
	m_layoutCardSets.Clear();
	for (CardSet::sptr cardSet : cardDatabase.GetCardSetsWithCard(m_card))
		m_layoutCardSets.Add(AllocateObject<Label>(cardSet->GetName(), fontSmall));

	m_labelCardTextShown.SetText(m_card->GetText(m_shownSide));
	m_labelCardTextRevealed.SetText(m_card->GetText(m_revealedSide));
	m_labelWordType.SetText(EnumToString(m_card->GetWordType()));
	m_studyHistoryTimeline.SetCardStudyData(m_cardStudyData);
	m_studyHistoryTimeline.SetVisible(
		m_cardStudyData.IsEncountered());

	std::stringstream ss;

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

	Color profColor = Config::k_colorGray;
	if (m_cardStudyData.IsEncountered())
		profColor = Config::GetHistoryScoreColor(
			m_cardStudyData.GetHistoryScore());
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
		new CaptureMethodDelegate(this, m_card, &StudyState::OpenCardEditView));
	menu->AddMenuOption("Edit Related Card", true,
		new CaptureMethodDelegate(this, m_card, &StudyState::OpenRelatedCardsView));
	menu->AddMenuOption("Add to Card Sets", true,
		new CaptureMethodDelegate(this, m_card, &StudyState::OpenAddCardToSetView));
	auto menuOption = menu->AddMenuOption("View Wiktionary Term", true,
		new CaptureMethodDelegate(
			GetApp(), m_term, &RussianStudyToolApp::OpenWikiTermView));
	menuOption->SetEnabled(m_term != nullptr);
	menu->AddMenuOption("Main Menu", true,
		new MethodDelegate((Widget*) this, &Widget::Close));
	GetApp()->PushState(menu);
}

void StudyState::OpenCardEditView(Card::sptr card)
{
	GetApp()->PushState(new CardEditWidget(card));
}

void StudyState::OpenRelatedCardsView(Card::sptr card)
{
	GetApp()->PushState(new RelatedCardsWidget(card));
}

void StudyState::OpenAddCardToSetView(Card::sptr card)
{
	GetApp()->PushState(new AddCardToSetWidget(card));
}

void StudyState::OpenCardInWebBrowser()
{
	if (m_term)
		OpenInWebBrowser(m_term->GetText().GetString());
}

void StudyState::OpenInWebBrowser(const AccentedText& text)
{
	unistr url = wiki::Parser::GetTermURL(text.GetString(), true);
	CMG_LOG_DEBUG() << "Opening web page: " << url;
	cmg::os::OpenInWebBrowser(url);
}

void StudyState::Copy()
{
	cmg::os::SetClipboardText(m_card->GetRussian().GetString());
}

void StudyState::OnCardDataChanged(Card::sptr card)
{
	if (card == m_card || (m_card && m_card->HasRelatedCards(card)))
	{
		ReshowCard();
	}
}

void StudyState::OnCardAddedOrRemovedFromSet(
	Card::sptr card, CardSet::sptr cardSet)
{
	if (card == m_card)
		ReshowCard();
}

bool StudyState::PopulateTermList(RelatedWordList& termList,
	const Set<AccentedText>& terms)
{
	termList.Clear();
	if (terms.empty())
		return false;
	for (const AccentedText& term : terms)
	{
		RelatedWordWidget* widget = termList.AddWord(term);
		widget->Clicked().Connect(this, widget, &StudyState::OnClickWordBox);
	}
	return true;
}

void StudyState::OnClickWordBox(RelatedWordWidget* widget)
{
	auto& wiktionary = GetApp()->GetWiktionary();

	AccentedText title = widget->GetText();
	Card::sptr card = widget->GetCard();
	wiki::Term::sptr term = widget->GetWiktionaryTerm();
	if (!term)
		term = wiktionary.GetTerm(widget->GetText().GetString(), true);
	if (card)
	{
		title += " - ";
		title += card->GetEnglish();
	}
	MenuWidget* menu = new MenuWidget(title);
	MenuItemWidget* menuOption;
	menu->AddCancelOption("Cancel");

	if (card != nullptr)
		menuOption = menu->AddMenuOption("Edit Card", true,
			new CaptureMethodDelegate(this, card, &StudyState::OpenCardEditView));
	else if (term != nullptr)
		menuOption = menu->AddMenuOption("Create Card from Term", true,
			new CaptureMethodDelegate(
				GetApp(), term, &RussianStudyToolApp::OpenCreateCardViewFromTerm));
	else
		menuOption = menu->AddMenuOption("Create as Card", true,
			new CaptureMethodDelegate<RussianStudyToolApp, const AccentedText&, void>(
				GetApp(), widget->GetText(), &RussianStudyToolApp::OpenCreateCardViewFromText));

	menuOption = menu->AddMenuOption("View Wiktionary Term", true,
		new CaptureMethodDelegate(
			GetApp(), term, &RussianStudyToolApp::OpenWikiTermView));
	menuOption->SetEnabled(term != nullptr);

	menuOption = menu->AddMenuOption("Open in Wiktionary", true,
		new CaptureMethodDelegate<StudyState, const AccentedText&, void>(
			this, widget->GetText(), &StudyState::OpenInWebBrowser));


	if (!card || !m_cardSet || !m_cardSet->HasCard(card))
		menuOption = menu->AddMenuOption("Add to Card Set", true,
			new CaptureMethodDelegate(this, card, &StudyState::OnChooseAddToCardSet));
	else
		menuOption = menu->AddMenuOption("Remove from Card Set", true,
			new CaptureMethodDelegate(this, card, &StudyState::OnChooseRemoveFromCardSet));
	menuOption->SetEnabled(card != nullptr && m_cardSet != nullptr);

	if (!card || !cmg::container::Contains(m_card->GetRelatedCards(), card))
		menuOption = menu->AddMenuOption("Add to related cards", true,
			new CaptureMethodDelegate(this, card, &StudyState::OnChooseAddToRelatedCards));
	else
		menuOption = menu->AddMenuOption("Remove from related cards", true,
			new CaptureMethodDelegate(this, card, &StudyState::OnChooseRemoveFromRelatedCards));
	menuOption->SetEnabled(card != nullptr);

	GetApp()->PushState(menu);
}

void StudyState::OnChooseAddToRelatedCards(Card::sptr card)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	cardDatabase.LinkRelatedCards(m_card, card);
	cardDatabase.SaveChanges();
}

void StudyState::OnChooseRemoveFromRelatedCards(Card::sptr card)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	cardDatabase.UnlinkRelatedCards(m_card, card);
	cardDatabase.SaveChanges();
}

void StudyState::OnChooseAddToCardSet(Card::sptr card)
{
	if (m_cardSet)
	{
		auto& cardDatabase = GetApp()->GetCardDatabase();
		cardDatabase.AddCardToSet(card, m_cardSet);
		cardDatabase.SaveChanges();
	}
}

void StudyState::OnChooseRemoveFromCardSet(Card::sptr card)
{
	if (m_cardSet)
	{
		auto& cardDatabase = GetApp()->GetCardDatabase();
		cardDatabase.RemoveCardFromSet(card, m_cardSet);
		cardDatabase.SaveChanges();
	}
}
