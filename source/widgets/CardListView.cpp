#include "CardListView.h"
#include "RussianApp.h"
#include "widgets/CardSetEditWidget.h"
#include "widgets/MenuWidget.h"
#include "widgets/RelatedCardsWidget.h"
#include "widgets/AddCardToSetWidget.h"
#include "widgets/CardEditWidget.h"


CardListView::CardListView(IStudySet* studySet) :
	m_studySet(studySet)
{
	m_layoutTitle.Add(&m_labelName);
	m_layoutTitle.Add(&m_labelPage);
	m_layoutTitle.Add(&m_labelCount);
	m_layoutTitle.Add(&m_topProficiencyBar);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);
	m_titleWidget.SetLayout(&m_layoutTitle);

	m_layoutCardList.SetItemBackgroundColors(true);
	m_widgetCardList.SetLayout(&m_layoutCardList);
	m_scrollArea.SetWidget(&m_widgetCardList);

	m_buttonPrevPage.SetText("Previous page");
	m_buttonNextPage.SetText("Next page");

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
}

void CardListView::SetCards(IStudySet* studySet)
{
	m_studySet = studySet;

	m_labelName.SetText(m_studySet->GetName());

	// Get study metrics
	auto& studyDatabase = GetApp()->GetStudyDatabase();
	StudySetMetrics metrics = studyDatabase.GetStudySetMetrics(m_studySet);
	int realCount = (int) (metrics.GetHistoryScore() * metrics.GetTotalCount());
	m_labelCount.SetText(std::to_string(realCount));
	m_topProficiencyBar.SetMetrics(metrics);

	m_pageCount = (m_studySet->GetCards().size() / m_cardsPerPage) + 1;
	if (m_studySet->GetCards().size() % m_cardsPerPage == 0)
		m_pageCount--;

	SetPage(0);
}

void CardListView::SetPage(uint32 pageIndex)
{
	m_pageIndex = pageIndex;
	Widget* itemToSelect = nullptr;
	m_layoutCardList.Clear();

	// Previous page button
	if (m_pageIndex > 0)
	{
		m_layoutCardList.Add(&m_buttonPrevPage);
		itemToSelect = &m_buttonPrevPage;
	}

	// Card rows
	m_rows.clear();
	m_cardToRowMap.clear();
	auto& cards = m_studySet->GetCards();
	uint32 start = pageIndex * m_cardsPerPage;
	uint32 end = Math::Min(start + m_cardsPerPage, cards.size());
	for (uint32 i = start; i < end; i++)
	{
		Row::sptr row = AddRow(cards[i]);
		if (!itemToSelect)
			itemToSelect = row.get();
	}

	// Next page button
	if (m_pageIndex + 1 < m_pageCount)
		m_layoutCardList.Add(&m_buttonNextPage);

	m_labelPage.SetText("Page " + std::to_string(m_pageIndex + 1) +
		"/" + std::to_string(m_pageCount));

	if (itemToSelect)
		itemToSelect->Focus();
}

void CardListView::OnInitialize()
{
	SetCards(m_studySet);

	// Connect signals
	auto& cardDatabase = GetApp()->GetCardDatabase();
	cardDatabase.CardDataChanged().Connect(this, &CardListView::OnCardDataChanged);
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

void CardListView::RefreshRow(Row::sptr row)
{
	int index = cmg::container::GetIndex(m_rows, row);

	String tagStr = "";
	for (auto it : row->m_card->GetTags())
	{
		if (it.second)
		{
			if (!tagStr.empty())
				tagStr += ", ";
			tagStr += EnumToShortString(it.first);
		}
	}
	auto& studyDatabase = GetApp()->GetStudyDatabase();
	const auto& studyData = studyDatabase.GetCardStudyData(row->m_card);

	row->displayNumber.SetText(std::to_string(index + 1));

	row->displayType.SetText(EnumToShortString(row->m_card->GetWordType()));
	row->displayRussian.SetText(row->m_card->GetRussian());
	row->displayEnglish.SetText(row->m_card->GetEnglish());
	row->displayTags.SetText(tagStr);

	// History score & proficiency level
	if (studyData.GetProficiencyLevel() == ProficiencyLevel::k_new)
	{
		row->m_labelScore.SetBackgroundColor(Color(0, 0, 0, 0));
		row->m_labelScore.SetText("");
	}
	else
	{
		std::stringstream scoreStr;
		scoreStr.setf(std::ios::fixed);
		scoreStr << std::setprecision(2) << studyData.GetHistoryScore();
		row->m_labelScore.SetText(scoreStr.str());
		Color scoreColor = Config::GetProficiencyLevelColor(
			studyData.GetProficiencyLevel());
		scoreColor.a = 128;
		row->m_labelScore.SetBackgroundColor(scoreColor);
	}
}

void CardListView::GoToNextPage()
{
	if (m_pageIndex + 1 < m_pageCount)
	{
		SetPage(m_pageIndex + 1);
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
		SetPage(m_pageIndex - 1);
		if (m_pageIndex > 0)
			m_buttonPrevPage.Focus();
		else
			m_buttonNextPage.Focus();
	}
}

CardListView::Row::Row(Card::sptr card) :
	m_card(card)
{
	SetFocusable(true);

	displayNumber.SetColor(GUIConfig::color_text_box_background_text);

	m_layout.Add(&displayNumber, 0.5f);
	m_layout.Add(&displayType, 0.8f);
	m_layout.Add(&displayRussian, 4.0f);
	m_layout.Add(&displayEnglish, 4.0f);
	m_layout.Add(&displayTags, 0.8f);
	m_layout.Add(&m_labelScore, 0.5f);
	
	SetLayout(&m_layout);
}
