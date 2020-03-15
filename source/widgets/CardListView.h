#pragma once

#include "Graphics.h"
#include "cards/CardSetPackage.h"
#include "gui/gui.h"
#include "widgets/StudyProficiencyBar.h"
#include "widgets/AppWidget.h"
#include "widgets/MenuWidget.h"
#include "widgets/GenericTableWidget.h"
#include "russian/WordDatabase.h"


class CardListView : public AppWidget
{
public:
	enum class SortKey
	{
		kNumber = 0,
		kWordType,
		kRussian,
		kEnglish,
		kTags,
		kScore,
	};

	CardListView(IStudySet* studySet);

	void SetCards(IStudySet* studySet);
	void SetPage(uint32 pageIndex, bool changeFocus);

	virtual void OnInitialize() override;
	virtual void OnUpdate(float timeDelta) override;

private:
	class Row : public AppWidget
	{
	public:
		using sptr = cmg::shared_ptr<Row>;

		Row(Card::sptr card);

		EventSignal<>& Clicked() { return m_clicked; }
		virtual void OnPress() override { m_clicked.Emit(); }

		HBoxLayout m_layout;
		EventSignal<> m_clicked;

		Card::sptr m_card;
		wiki::Term::sptr m_wikiTerm;
		wiki::Word::sptr m_wikiWord;
		Label m_labelNumber;
		Label m_labelType;
		Label m_labelRussian;
		Label m_labelEnglish;
		HBoxLayout m_layoutTags;
		Label m_labelScore;
	};

	Row::sptr GetRow(Card::sptr card);
	Row::sptr AddRow(Card::sptr card);
	void OpenPauseMenu(Card::sptr card);
	void OpenCardEditView(Card::sptr card);
	void OpenRelatedCardsView(Card::sptr card);
	void OpenAddCardToSetView(Card::sptr card);
	void OpenCardInWebBrowser(Card::sptr card);
	void OpenInWebBrowser(const unistr& text);
	void OnCardDataChanged(Card::sptr card);
	void OnCardAddedOrRemovedFromSet(Card::sptr card, CardSet::sptr cardSet);
	void OnTermDownloaded(const CardWordMatch& wordMatch);
	void RefreshRow(Row::sptr row);
	void RefreshRow(Row::sptr row, const CardWordMatch& wordMatch);
	void GoToNextPage();
	void GoToPrevPage();
	void OnClickRefresh();
	void RepopulateCardList(bool forceRefresh, bool changeFocus);
	bool GetSortedCardList(Array<Card::sptr>& outCardList);
	void ApplySortKey(SortKey sortKey);

	IStudySet* m_studySet;
	Array<Card::sptr> m_cards;
	bool m_isStudySetChanged = false;

	uint32 m_cardsPerPage = 100;
	uint32 m_pageIndex = 0;
	uint32 m_pageCount = 0;

	SortKey m_sortKey = SortKey::kRussian;
	bool m_sortAscending = true;

	VBoxLayout m_mainLayout;
	VBoxLayout m_layoutTitle;
	HBoxLayout m_layoutTitleBar;
	HBoxLayout m_layoutPageNavigation;
	HBoxLayout m_layoutTableHeader;
	Widget m_titleWidget;
	StudyProficiencyBar m_topProficiencyBar;
	Label m_labelName;
	Label m_labelCount;
	Label m_labelPage;
	Button m_buttonNextPage;
	Button m_buttonPrevPage;
	Button m_buttonRefreshList;
	Label m_labelHeaderNumber;
	Label m_labelHeaderWordType;
	Label m_labelHeaderRussian;
	Label m_labelHeaderEnglish;
	Label m_labelHeaderTags;
	Label m_labelHeaderScore;
	Map<SortKey, Label*> m_headerLabelMap;
	Map<SortKey, AccentedText> m_headerNameMap;

	AbstractScrollArea m_scrollArea;
	Widget m_widgetCardList;
	VBoxLayout m_layoutCardList;
	Map<Card::sptr, Row::sptr> m_cardToRowMap;
	Array<Row::sptr> m_rows;
};
