#include "CardListView.h"
#include "RussianApp.h"
#include "widgets/CardSetEditWidget.h"
#include "widgets/MenuWidget.h"
#include "widgets/RelatedCardsWidget.h"
#include "widgets/AddCardToSetWidget.h"
#include "widgets/CardEditWidget.h"


CardListView::CardListView(IStudySet* studySet) :
	m_studySet(studySet),
	m_buttonRefreshList("Refresh")
{
	// Title
	m_layoutTitleBar.Add(&m_labelName);
	m_layoutTitleBar.Add(&m_buttonRefreshList);
	m_layoutTitleBar.Add(&m_labelCount);
	m_layoutTitleBar.Add(&m_topProficiencyBar);

	// Nvaigation
	m_layoutPageNavigation.Add(&m_buttonPrevPage);
	m_layoutPageNavigation.Add(&m_labelPage);
	m_layoutPageNavigation.Add(&m_buttonNextPage);
	m_labelPage.SetAlign(TextAlign::CENTERED);

	// Table header
	m_layoutTableHeader.Add(&m_labelHeaderNumber, 0.5f);
	m_layoutTableHeader.Add(&m_labelHeaderWordType, 0.8f);
	m_layoutTableHeader.Add(&m_labelHeaderRussian, 4.0f);
	m_layoutTableHeader.Add(&m_labelHeaderEnglish, 4.0f);
	m_layoutTableHeader.Add(&m_labelHeaderTags, 1.0f);
	m_layoutTableHeader.Add(&m_labelHeaderScore, 0.5f);
	m_headerLabelMap[SortKey::kNumber] = &m_labelHeaderNumber;
	m_headerLabelMap[SortKey::kWordType] = &m_labelHeaderWordType;
	m_headerLabelMap[SortKey::kRussian] = &m_labelHeaderRussian;
	m_headerLabelMap[SortKey::kEnglish] = &m_labelHeaderEnglish;
	m_headerLabelMap[SortKey::kTags] = &m_labelHeaderTags;
	m_headerLabelMap[SortKey::kScore] = &m_labelHeaderScore;
	m_headerNameMap[SortKey::kNumber] = "#";
	m_headerNameMap[SortKey::kWordType] = "Type";
	m_headerNameMap[SortKey::kRussian] = "Russian";
	m_headerNameMap[SortKey::kEnglish] = "English";
	m_headerNameMap[SortKey::kTags] = "Tags";
	m_headerNameMap[SortKey::kScore] = "Score";

	// Card list
	m_layoutCardList.SetItemBackgroundColors(true);
	m_widgetCardList.SetLayout(&m_layoutCardList);
	m_scrollArea.SetWidget(&m_widgetCardList);

	// Title panel
	m_layoutTitle.Add(&m_layoutTitleBar);
	m_layoutTitle.Add(&m_layoutPageNavigation);
	m_layoutTitle.Add(&m_layoutTableHeader);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleWidget.SetLayout(&m_layoutTitle);

	// Create layouts
	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.Add(&m_titleWidget, 0.0f);
	m_mainLayout.Add(&m_scrollArea, 1.0f);
	SetLayout(&m_mainLayout);
	SetBackgroundColor(GUIConfig::color_background);

	// Connect signals
	m_buttonPrevPage.Clicked().Connect(this, &CardListView::GoToPrevPage);
	m_buttonNextPage.Clicked().Connect(this, &CardListView::GoToNextPage);
	m_buttonRefreshList.Clicked().Connect(this, &CardListView::OnClickRefresh);
	AddKeyShortcut("1", [this]{ ApplySortKey(SortKey::kWordType); return true; });
	AddKeyShortcut("2", [this]{ ApplySortKey(SortKey::kRussian); return true; });
	AddKeyShortcut("3", [this]{ ApplySortKey(SortKey::kEnglish); return true; });
	AddKeyShortcut("4", [this]{ ApplySortKey(SortKey::kScore); return true; });
	AddKeyShortcut("f5", [this]{ OnClickRefresh(); return true; });
}

void CardListView::SetCards(IStudySet* studySet)
{
	m_studySet = studySet;
	m_labelName.SetText(m_studySet->GetName());
	m_pageIndex = 0;
	RepopulateCardList(true, true);
}

void CardListView::SetPage(uint32 pageIndex, bool changeFocus)
{
	m_pageIndex = pageIndex;
	Widget* itemToSelect = nullptr;
	m_layoutCardList.Clear();
	
	// Page navigation
	m_buttonPrevPage.SetEnabled(m_pageIndex > 0);
	m_buttonNextPage.SetEnabled(m_pageIndex + 1 < m_pageCount);
	m_buttonPrevPage.SetText("");
	m_buttonNextPage.SetText("");
	m_labelPage.SetText("Page " + std::to_string(m_pageIndex + 1) +
		"/" + std::to_string(m_pageCount));
	if (m_buttonPrevPage.IsEnabled())
		m_buttonPrevPage.SetText("< Page " + std::to_string(m_pageIndex));
	if (m_buttonNextPage.IsEnabled())
		m_buttonNextPage.SetText("Page " + std::to_string(m_pageIndex + 2) + " >");
	
	// Card rows
	m_rows.clear();
	m_cardToRowMap.clear();
	uint32 start = pageIndex * m_cardsPerPage;
	uint32 end = Math::Min(start + m_cardsPerPage, m_cards.size());
	for (uint32 i = start; i < end; i++)
	{
		Row::sptr row = AddRow(m_cards[i]);
		if (!itemToSelect)
			itemToSelect = row.get();
	}

	if (changeFocus && itemToSelect)
		itemToSelect->Focus();
}

void CardListView::OnInitialize()
{
	SetCards(m_studySet);

	// Connect signals
	auto& cardDatabase = GetApp()->GetCardDatabase();
	auto& wordDatabase = GetApp()->GetWordDatabase();
	cardDatabase.CardDataChanged().Connect(this, &CardListView::OnCardDataChanged);
	cardDatabase.CardAddedToSet().Connect(this, &CardListView::OnCardAddedOrRemovedFromSet);
	cardDatabase.CardRemovedFromSet().Connect(this, &CardListView::OnCardAddedOrRemovedFromSet);
	wordDatabase.TermDownloaded().Connect(this, &CardListView::OnTermDownloaded);
}

void CardListView::OnUpdate(float timeDelta)
{
	auto app = GetApp();
	auto& studyDatabase = app->GetStudyDatabase();

	m_topProficiencyBar.SetMetrics(
		studyDatabase.GetStudySetMetrics(m_studySet));
}

CardListView::Row::sptr CardListView::GetRow(Card::sptr card)
{
	auto it = m_cardToRowMap.find(card);
	if (it != m_cardToRowMap.end())
		return it->second;
	return nullptr;
}

CardListView::Row::sptr CardListView::AddRow(Card::sptr card)
{
	Row::sptr row = cmg::make_shared<Row>(card);
	m_cardToRowMap[card] = row;
	
	// Connect signals
	row->Clicked().Connect(this, card, &CardListView::OpenPauseMenu);
	row->AddKeyShortcut("e", [this, card]() { OpenCardEditView(card); return true; });
	row->AddKeyShortcut("r", [this, card]() { OpenRelatedCardsView(card); return true; });
	row->AddKeyShortcut("s", [this, card]() { OpenAddCardToSetView(card); return true; });
	row->AddKeyShortcut("i", [this, card]() { OpenCardInWebBrowser(card); return true; });

	m_layoutCardList.Add(row.get());
	m_rows.push_back(row);
	RefreshRow(row);
	return row;
}

void CardListView::OpenPauseMenu(Card::sptr card)
{
	Row::sptr row = GetRow(card);
	wiki::Term::sptr wikiTerm = (row ? row->m_wikiTerm : nullptr);
	unistr termName = card->GetRuKey().russian;
	if (wikiTerm)
		termName = wikiTerm->GetText().GetString();
	AccentedText title = card->GetRussian() + " - " + card->GetEnglish();

	MenuWidget* menu = new MenuWidget(title);
	menu->AddCancelOption("Cancel");
	menu->AddMenuOption("Edit Card", true,
		new CaptureMethodDelegate(this, card, &CardListView::OpenCardEditView));
	menu->AddMenuOption("Edit Related Card", true,
		new CaptureMethodDelegate(this, card, &CardListView::OpenRelatedCardsView));
	menu->AddMenuOption("Add to Card Sets", true,
		new CaptureMethodDelegate(this, card, &CardListView::OpenAddCardToSetView));
	auto option = menu->AddMenuOption(u"Open in Wiktionary: " + termName, true,
		new CaptureMethodDelegate(this, card, &CardListView::OpenCardInWebBrowser));
	option->SetEnabled(wikiTerm != nullptr);
	menu->AddMenuOption("Main Menu", true,
		new MethodDelegate((Widget*) this, &Widget::Close));

	GetApp()->PushState(menu);
}

void CardListView::OpenCardEditView(Card::sptr card)
{
	GetApp()->PushState(new CardEditWidget(card));
}

void CardListView::OpenRelatedCardsView(Card::sptr card)
{
	GetApp()->PushState(new RelatedCardsWidget(card));
}

void CardListView::OpenAddCardToSetView(Card::sptr card)
{
	GetApp()->PushState(new AddCardToSetWidget(card));
}

void CardListView::OpenCardInWebBrowser(Card::sptr card)
{
	Row::sptr row = GetRow(card);
	if (row && row->m_wikiTerm)
		OpenInWebBrowser(row->m_wikiTerm->GetText().GetString());
}

void CardListView::OpenInWebBrowser(const unistr& text)
{
	unistr url = wiki::Parser::GetTermURL(text, true);
	CMG_LOG_DEBUG() << "Opening web page: " << url;
	cmg::os::OpenInWebBrowser(url);
}

void CardListView::OnCardDataChanged(Card::sptr card)
{
	Row::sptr row = GetRow(card);
	if (row)
		RefreshRow(row);
}

void CardListView::OnCardAddedOrRemovedFromSet(Card::sptr card, CardSet::sptr cardSet)
{
	// Check if this card is now or is now not in the study set
	
	bool oldInStudySet = cmg::container::Contains(m_cards, card);
	bool newInStudySet = cmg::container::Contains(m_studySet->GetCards(), card);
	if (newInStudySet != oldInStudySet)
	{
		Row::sptr row = GetRow(card);
		if (oldInStudySet && row)
		{
			RefreshRow(row);
		}
		if (!m_isStudySetChanged)
		{
			m_isStudySetChanged = true;
			m_buttonRefreshList.SetText("Refresh (!)");
			m_buttonRefreshList.SetEnabled(true);
		}
	}
}

void CardListView::OnTermDownloaded(const CardWordMatch& wordMatch)
{
	Row::sptr row = GetRow(wordMatch.card);
	if (row)
		RefreshRow(row, wordMatch);
}

void CardListView::RefreshRow(Row::sptr row)
{
	auto& wordDatabase = GetApp()->GetWordDatabase();
	CardWordMatch wordMatch = wordDatabase.GetWordFromCard(row->m_card, true, true);
	RefreshRow(row, wordMatch);
}

void CardListView::RefreshRow(Row::sptr row, const CardWordMatch& wordMatch)
{
	int index = cmg::container::GetIndex(m_cards, row->m_card);

	auto& studyDatabase = GetApp()->GetStudyDatabase();
	const auto& studyData = studyDatabase.GetCardStudyData(row->m_card);

	bool isInStudySet = cmg::container::Contains(
		m_studySet->GetCards(), row->m_card);

	// Get the wiki word
	row->m_wikiTerm = wordMatch.GetTerm();
	row->m_wikiWord = wordMatch.GetWord();

	// Number, colorize by word source
	String numberText = std::to_string(index + 1);
	if (!isInStudySet)
		numberText += " (!)";
	row->m_labelNumber.SetText(numberText);
	Color labelColor = GUIConfig::color_text_box_background_text;
	auto part = wordMatch.GetPart();
	if (part && part->isLoaded)
	{
		if (!part->word)
			labelColor = Config::k_colorRed;
		else if (part->isMatchingType && part->isMatchingText)
			labelColor = Config::k_colorGreen;
		else
			labelColor = Config::k_colorYellow;
	}
	else if (wordMatch.Empty())
		labelColor = Config::k_colorRed;
	row->m_labelNumber.SetColor(labelColor);

	// Get card tags from the wiki word, if relevant
	EnumFlags<CardTags> cardTags = row->m_card->GetTags();
	if (part && part->word && part->isMatchingType)
		cardTags = (cardTags | part->word->GetTags());

	row->m_labelType.SetText(EnumToShortString(row->m_card->GetWordType()));
	row->m_labelRussian.SetText(row->m_card->GetRussian());
	row->m_labelEnglish.SetText(row->m_card->GetEnglish());
	row->m_tagEditBox.SetTags(cardTags);

	// Score
	if (!studyData.IsEncountered())
	{
		row->m_labelScore.SetBackgroundColor(Color(0, 0, 0, 0));
		row->m_labelScore.SetText("");
	}
	else
	{
		float historyScore = studyData.GetHistoryScore();
		std::stringstream scoreStr;
		scoreStr.setf(std::ios::fixed);
		scoreStr << std::setprecision(2) << studyData.GetHistoryScore();
		row->m_labelScore.SetText(scoreStr.str());
		Color scoreColor = Config::GetHistoryScoreColor(historyScore);
		scoreColor.a = 128;
		row->m_labelScore.SetBackgroundColor(scoreColor);
	}
}

void CardListView::GoToNextPage()
{
	if (m_pageIndex + 1 < m_pageCount)
	{
		SetPage(m_pageIndex + 1, true);
		if (m_pageIndex + 1 < m_pageCount)
			m_buttonNextPage.Focus();
		else
			m_buttonPrevPage.Focus();
	}
}

void CardListView::GoToPrevPage()
{
	if (m_pageIndex > 0)
	{
		SetPage(m_pageIndex - 1, true);
		if (m_pageIndex > 0)
			m_buttonPrevPage.Focus();
		else
			m_buttonNextPage.Focus();
	}
}

void CardListView::OnClickRefresh()
{
	RepopulateCardList(false, false);
}

void CardListView::RepopulateCardList(bool forceRefresh, bool changeFocus)
{
	// Update header labels
	for (auto it : m_headerLabelMap)
	{
		AccentedText name = m_headerNameMap[it.first];
		if (m_sortKey == it.first)
			name += unistr(m_sortAscending ? u" (v)" : u" (^)");
		it.second->SetColor(GUIConfig::color_text_box_background_text);
		it.second->SetText(name);
	}

	// Get sorted list of cards
	Array<Card::sptr> newCards;
	bool changed = GetSortedCardList(newCards);
	if (!forceRefresh && !changed)
		return;
	m_cards = newCards;

	// Calculate page count
	m_pageCount = (m_cards.size() / m_cardsPerPage) + 1;
	if (m_cards.size() % m_cardsPerPage == 0)
		m_pageCount--;

	// Get study metrics
	auto& studyDatabase = GetApp()->GetStudyDatabase();
	StudySetMetrics metrics = studyDatabase.GetStudySetMetrics(m_studySet);
	int realCount = (int) (metrics.GetHistoryScore() * metrics.GetTotalCount());
	m_labelCount.SetText(std::to_string(realCount));
	m_topProficiencyBar.SetMetrics(metrics);

	m_isStudySetChanged = false;
	m_buttonRefreshList.SetText("Refresh");
	m_buttonRefreshList.SetEnabled(false);

	SetPage(Math::Min(m_pageIndex, m_pageCount - 1), changeFocus);
}

bool CardListView::GetSortedCardList(Array<Card::sptr>& outCardList)
{
	StudyDatabase& studyDatabase = GetApp()->GetStudyDatabase();

	// Sort cards
	struct RowComparator
	{
		bool sortAscending;
		CardListView::SortKey sortKey;
		StudyDatabase& studyDatabase;

		RowComparator(StudyDatabase& studyDatabase,
			CardListView::SortKey sortKey, bool sortAscending) :
			studyDatabase(studyDatabase),
			sortKey(sortKey),
			sortAscending(sortAscending)
		{
		}

		inline bool operator()(Card::sptr cardA, Card::sptr cardB)
		{
			if (sortKey == CardListView::SortKey::kRussian)
			{
				CardRuKey keyA = cardA->GetRuKey();
				CardRuKey keyB = cardB->GetRuKey();
				auto tupleA = std::tuple<unistr, String>(
					keyA.russian, EnumToShortString(keyA.type));
				auto tupleB = std::tuple<unistr, String>(
					keyB.russian, EnumToShortString(keyB.type));
				return (sortAscending ? tupleA < tupleB : tupleA > tupleB);
			}
			else if (sortKey == CardListView::SortKey::kEnglish)
			{
				CardEnKey keyA = cardA->GetEnKey();
				CardEnKey keyB = cardB->GetEnKey();
				auto tupleA = std::tuple<unistr, String>(
					keyA.english, EnumToShortString(keyA.type));
				auto tupleB = std::tuple<unistr, String>(
					keyB.english, EnumToShortString(keyB.type));
				return (sortAscending ? tupleA < tupleB : tupleA > tupleB);
			}
			else if (sortKey == CardListView::SortKey::kWordType)
			{
				CardRuKey keyA = cardA->GetRuKey();
				CardRuKey keyB = cardB->GetRuKey();
				auto tupleA = std::tuple<String, unistr>(
					EnumToShortString(keyA.type), keyA.russian);
				auto tupleB = std::tuple<String, unistr>(
					EnumToShortString(keyB.type), keyB.russian);
				return (sortAscending ? tupleA < tupleB : tupleA > tupleB);
			}
			else if (sortKey == CardListView::SortKey::kScore)
			{
				CardRuKey keyA = cardA->GetRuKey();
				CardRuKey keyB = cardB->GetRuKey();
				const auto& studyA = studyDatabase.GetCardStudyData(cardA);
				const auto& studyB = studyDatabase.GetCardStudyData(cardB);
				auto scoreA = (studyA.IsEncountered() ? studyA.GetHistoryScore() : -1.0f);
				auto scoreB = (studyB.IsEncountered() ? studyB.GetHistoryScore() : -1.0f);
				auto tupleA = std::tuple<float, unistr>(
					scoreA, keyA.russian);
				auto tupleB = std::tuple<float, unistr>(
					scoreB, keyB.russian);
				return (sortAscending ? tupleA < tupleB : tupleA > tupleB);
			}
			return false;
		}
	};

	outCardList = m_studySet->GetCards();
	std::sort(outCardList.begin(), outCardList.end(),
		RowComparator(studyDatabase, m_sortKey, m_sortAscending));
	return (m_cards != outCardList);
}

void CardListView::ApplySortKey(SortKey sortKey)
{
	if (sortKey == m_sortKey)
	{
		m_sortAscending = !m_sortAscending;
	}
	else
	{
		m_sortKey = sortKey;
		m_sortAscending = true;
	}
	RepopulateCardList(true, true);
}

CardListView::Row::Row(Card::sptr card) :
	m_card(card)
{
	SetFocusable(true);

	m_labelNumber.SetColor(GUIConfig::color_text_box_background_text);
	m_labelScore.SetAlign(TextAlign::CENTERED);

	m_layout.Add(&m_labelNumber, 0.5f);
	m_layout.Add(&m_labelType, 0.8f);
	m_layout.Add(&m_labelRussian, 4.0f);
	m_layout.Add(&m_labelEnglish, 4.0f);
	m_layout.Add(&m_tagEditBox, 1.0f);
	m_layout.Add(&m_labelScore, 0.5f);
	
	m_tagEditBox.SetReadOnly(true);

	SetLayout(&m_layout);
}
