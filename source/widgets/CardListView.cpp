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
	m_layoutTitle.Add(&m_labelName);
	m_layoutTitle.Add(&m_buttonRefreshList);
	m_layoutTitle.Add(&m_labelCount);
	m_layoutTitle.Add(&m_topProficiencyBar);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleWidget.SetLayout(&m_layoutTitle);

	m_layoutPageNavigation.Add(&m_buttonPrevPage);
	m_layoutPageNavigation.Add(&m_labelPage);
	m_layoutPageNavigation.Add(&m_buttonNextPage);
	m_labelPage.SetAlign(TextAlign::CENTERED);

	m_layoutCardList.SetItemBackgroundColors(true);
	m_widgetCardList.SetLayout(&m_layoutCardList);
	m_scrollArea.SetWidget(&m_widgetCardList);

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

	m_layoutCardList.Add(&m_layoutPageNavigation);

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
	cardDatabase.CardDataChanged().Connect(this, &CardListView::OnCardDataChanged);
	cardDatabase.CardAddedToSet().Connect(this, &CardListView::OnCardAddedOrRemovedFromSet);
	cardDatabase.CardRemovedFromSet().Connect(this, &CardListView::OnCardAddedOrRemovedFromSet);
}

void CardListView::OnUpdate(float timeDelta)
{
	auto app = GetApp();
	auto& studyDatabase = app->GetStudyDatabase();

	m_topProficiencyBar.SetMetrics(
		studyDatabase.GetStudySetMetrics(m_studySet));
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

	m_layoutCardList.Add(row.get());
	m_rows.push_back(row);
	RefreshRow(row);
	return row;
}

void CardListView::OpenPauseMenu(Card::sptr card)
{
	AccentedText title = card->GetRussian() + " - " + card->GetEnglish();
	MenuWidget* menu = new MenuWidget(title);
	menu->AddCancelOption("Cancel");
	menu->AddMenuOption("Edit Card", true,
		new CaptureMethodDelegate(this, card, &CardListView::OpenCardEditView));
	menu->AddMenuOption("Edit Related Card", true,
		new CaptureMethodDelegate(this, card, &CardListView::OpenRelatedCardsView));
	menu->AddMenuOption("Add to Card Sets", true,
		new CaptureMethodDelegate(this, card, &CardListView::OpenAddCardToSetView));
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

void CardListView::OnCardDataChanged(Card::sptr card)
{
	auto it = m_cardToRowMap.find(card);
	if (it != m_cardToRowMap.end())
	{
		Row::sptr row = m_cardToRowMap[card];
		RefreshRow(row);
	}
}

void CardListView::OnCardAddedOrRemovedFromSet(Card::sptr card, CardSet::sptr cardSet)
{
	// Check if this card is now or is now not in the study set
	
	bool oldInStudySet = cmg::container::Contains(m_cards, card);
	bool newInStudySet = cmg::container::Contains(m_studySet->GetCards(), card);
	if (newInStudySet != oldInStudySet)
	{
		if (oldInStudySet && m_cardToRowMap.find(card) != m_cardToRowMap.end())
		{
			RefreshRow(m_cardToRowMap[card]);
		}
		if (!m_isStudySetChanged)
		{
			m_isStudySetChanged = true;
			m_buttonRefreshList.SetText("Refresh (!)");
			m_buttonRefreshList.SetEnabled(true);
		}
	}
}

void CardListView::RefreshRow(Row::sptr row)
{
	int index = cmg::container::GetIndex(m_rows, row);

	auto& studyDatabase = GetApp()->GetStudyDatabase();
	const auto& studyData = studyDatabase.GetCardStudyData(row->m_card);

	bool isInStudySet = cmg::container::Contains(
		m_studySet->GetCards(), row->m_card);

	String numberText = std::to_string(index + 1);
	if (!isInStudySet)
		numberText += " (!)";
	row->m_labelNumber.SetText(numberText);

	row->m_labelType.SetText(EnumToShortString(row->m_card->GetWordType()));
	row->m_labelRussian.SetText(row->m_card->GetRussian());
	row->m_labelEnglish.SetText(row->m_card->GetEnglish());
	row->m_layoutTags.Clear();
	for (auto it : row->m_card->GetTags())
	{
		if (it.second)
		{
			Label* tagLabel = AllocateObject<Label>(
				Config::GetCardTagShortDisplayName(it.first));
			Color tagColor = Config::GetCardTagColor(it.first);
			tagColor.a = 128;
			tagLabel->SetBackgroundColor(tagColor);
			row->m_layoutTags.Add(tagLabel);
		}
	}
	row->m_layoutTags.AddStretch();

	// History score & proficiency level
	if (studyData.GetProficiencyLevel() == ProficiencyLevel::k_new)
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
	// Sort cards
	struct RowComparator
	{
		inline bool operator()(Card::sptr cardA, Card::sptr cardB)
		{
			CardRuKey keyA = cardA->GetRuKey();
			CardRuKey keyB = cardB->GetRuKey();
			auto tupleA = std::tuple<unistr, uint32>(
				keyA.russian, (uint32) keyA.type);
			auto tupleB = std::tuple<unistr, uint32>(
				keyB.russian, (uint32) keyB.type);
			return tupleA < tupleB;
		}
	};
	outCardList = m_studySet->GetCards();
	std::sort(outCardList.begin(), outCardList.end(), RowComparator());
	return (m_cards != outCardList);
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
	m_layout.Add(&m_layoutTags, 1.0f);
	m_layout.Add(&m_labelScore, 0.5f);
	
	SetLayout(&m_layout);
}
