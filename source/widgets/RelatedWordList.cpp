#include "RelatedWordList.h"
#include "RussianApp.h"

RelatedWordList::RelatedWordList()
{
	m_layoutWords.SetSpacing(8.0f);
	m_layoutWords.SetMargins(0.0f);
	m_layout.SetSpacing(8.0f);
	m_layout.SetMargins(0.0f);
	m_layout.Add(&m_label);
	m_layout.Add(&m_layoutWords);
	m_layout.AddStretch();
	SetLayout(&m_layout);
}

void RelatedWordList::Clear()
{
	m_layoutWords.Clear();
}

void RelatedWordList::SetLabel(const AccentedText & text)
{
	m_label.SetText(text);
}

RelatedWordWidget* RelatedWordList::AddWord(const AccentedText& text)
{
	RelatedWordWidget* widget = AllocateObject<RelatedWordWidget>(text);
	widget->m_label.SetFont(m_font);
	m_layoutWords.Add(widget);
	return widget;
}

void RelatedWordList::OnInitialize()
{
	m_font = GetApp()->GetResourceManager()->Get<Font>(Res::FONT_SMALL);
	m_label.SetFont(m_font);
}

RelatedWordWidget::RelatedWordWidget(const AccentedText& text) :
	m_label(text)
{
	m_label.SetFocusable(false);
	m_mainLayout.SetMargins(0.0f);
	m_mainLayout.SetSpacing(0.0f);
	m_mainLayout.Add(&m_label);
	SetLayout(&m_mainLayout);
	m_label.SetBackgroundColor(GUIConfig::color_background_alternate);
}

void RelatedWordWidget::OnInitialize()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	auto& wiktionary = GetApp()->GetWiktionary();

	m_card = nullptr;
	m_term = nullptr;
	
	CheckForCard();

	unistr text = m_label.GetText().GetString();
	ru::ToLowerIP(text);
	m_term = wiktionary.GetTerm(text);

	Refresh();

	cardDatabase.CardCreated().Connect(this, &RelatedWordWidget::OnCardEdited);
	cardDatabase.CardDataChanged().Connect(this, &RelatedWordWidget::OnCardEdited);
	wiktionary.TermLoaded().Connect(this, &RelatedWordWidget::OnTermLoaded);
}

void RelatedWordWidget::Refresh()
{
	auto& studyDatabase = GetApp()->GetStudyDatabase();

	Color color = GUIConfig::color_background_alternate;
	if (m_card)
	{
		const CardStudyData& studyData = studyDatabase.GetCardStudyData(m_card);
		color = Config::GetHistoryScoreColor(studyData.GetHistoryScore());
		color = Color::Lerp(color, GUIConfig::color_background, 0.4f);
	}
	else if (m_term)
	{
		color = Color::DARK_BLUE;
	}
	m_label.SetBackgroundColor(color);
}

void RelatedWordWidget::OnTermLoaded(wiki::Term::sptr term)
{
	if (!m_term && term->GetText().GetString() == m_label.GetText().GetString())
	{
		m_term = term;
		Refresh();
	}
}

bool RelatedWordWidget::CheckForCard()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();

	Card::sptr newCard = nullptr;
	unistr text = m_label.GetText().GetString();
	ru::ToLowerIP(text);

	for (WordType wordType : EnumValues<WordType>())
	{
		CardRuKey key(wordType, text);
		cmg::string::ReplaceAll(key.russian, u"ё", u"е");
		newCard = cardDatabase.GetCard(key);
		if (newCard)
			break;
	}

	if (newCard != m_card)
	{
		m_card = newCard;
		return true;
	}
	return false;
}

void RelatedWordWidget::OnCardEdited(Card::sptr card)
{
	if (CheckForCard())
		Refresh();
}
