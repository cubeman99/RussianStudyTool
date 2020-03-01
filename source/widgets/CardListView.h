#pragma once

#include "Graphics.h"
#include "cards/CardSetPackage.h"
#include "gui/gui.h"
#include "widgets/StudyProficiencyBar.h"
#include "widgets/AppWidget.h"
#include "widgets/MenuWidget.h"
#include "widgets/GenericTableWidget.h"


class CardListView : public AppWidget
{
public:
	CardListView(IStudySet* studySet);

	void SetCards(IStudySet* studySet);
	void SetPage(uint32 pageIndex);

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
		Label displayNumber;
		Label displayType;
		Label displayRussian;
		Label displayEnglish;
		Label displayTags;
		Label m_labelScore;
	};

	Row::sptr AddRow(Card::sptr card);
	void OpenPauseMenu(Card::sptr card);
	void OpenCardEditView(Card::sptr card);
	void OpenRelatedCardsView(Card::sptr card);
	void OpenAddCardToSetView(Card::sptr card);
	void OnCardDataChanged(Card::sptr card);
	void RefreshRow(Row::sptr row);
	void GoToNextPage();
	void GoToPrevPage();

	IStudySet* m_studySet;

	uint32 m_cardsPerPage = 100;
	uint32 m_pageIndex = 0;
	uint32 m_pageCount = 0;

	VBoxLayout m_mainLayout;
	HBoxLayout m_layoutTitle;
	Widget m_titleWidget;
	StudyProficiencyBar m_topProficiencyBar;
	Label m_labelName;
	Label m_labelCount;
	Label m_labelPage;
	Button m_buttonNextPage;
	Button m_buttonPrevPage;

	AbstractScrollArea m_scrollArea;
	Widget m_widgetCardList;
	VBoxLayout m_layoutCardList;
	Map<Card::sptr, Row::sptr> m_cardToRowMap;
	Array<Row::sptr> m_rows;
};
