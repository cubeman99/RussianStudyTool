#include "WikiTermView.h"
#include "RussianApp.h"
#include "widgets/CardEditWidget.h"


WikiTermView::WikiTermView(wiki::Term::sptr term) :
	m_term(term)
{
	m_labelTitle.SetText(term->GetText());
	m_layoutTitle.Add(&m_labelTitle);
	m_titleWidget.SetLayout(&m_layoutTitle);
	m_titleWidget.SetBackgroundColor(GUIConfig::color_background_light);

	m_listCards.SetLabel("Found in Cards:");

	m_widgetWordList.SetLayout(&m_layoutWordList);
	m_scrollArea.SetWidget(&m_widgetWordList);
	m_mainLayout.Add(&m_titleWidget, 0.0f);
	m_mainLayout.Add(&m_listCards, 0.0f);
	m_mainLayout.Add(&m_scrollArea, 1.0f);
	SetLayout(&m_mainLayout);
	SetBackgroundColor(GUIConfig::color_background);
	m_widgetWordList.SetBackgroundColor(GUIConfig::color_background_alternate);
}

void WikiTermView::OnInitialize()
{
	SetTerm(m_term);
}

void WikiTermView::SetTerm(wiki::Term::sptr term)
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	auto& wordDatabase = GetApp()->GetWordDatabase();

	m_term = term;
	m_layoutWordList.Clear();

	unistr text = m_term->GetText().GetString();
	
	bool first = true;
	for (auto it : m_term->GetWords())
	{
		WikiWordWidget* wordWidget = AllocateObject<WikiWordWidget>(it.second);
		wordWidget->Clicked().Connect(this, wordWidget, &WikiTermView::OpenOptionsMenu);
		m_layoutWordList.Add(wordWidget);
		if (first)
			wordWidget->Focus();
		first = false;
	}

	m_listCards.Clear();
	for (auto it : cardDatabase.GetCards())
	{
		auto card = it.second;
		auto word = wordDatabase.GetWordFromCard(card, false);
		for (auto variant : word.variants)
		{
			for (auto part : variant.parts)
			{
				if (part.text == text && part.isMatchingText)
				{
					RelatedWordWidget* widget =
						m_listCards.AddWord(card->GetRussian());
					widget->Clicked().Connect(this, card, &WikiTermView::OnClickCard);
				}
			}
		}
	}
}

void WikiTermView::OpenOptionsMenu(WikiWordWidget* widget)
{
	wiki::Word::sptr word = widget->GetWord();
	AccentedText title = widget->m_wordDefinitionWidget.GetWordText();

	MenuWidget* menu = new MenuWidget(title);
	menu->AddCancelOption("Cancel");

	menu->AddMenuOption("Create as Card", true, new CaptureMethodDelegate(
		GetApp(), word, &RussianStudyToolApp::OpenCreateCardViewFromWord));
	menu->AddMenuOption("Open in Wiktionary", true, new CaptureMethodDelegate(
		GetApp(), m_term, &RussianStudyToolApp::OpenTermInWiktionary));

	menu->AddMenuOption("Back", true,
		new MethodDelegate((Widget*) this, &Widget::Close));

	GetApp()->PushState(menu);
}

void WikiTermView::OnClickCard(Card::sptr card)
{
	GetApp()->PushState(new CardEditWidget(card));
}


WikiWordWidget::WikiWordWidget(wiki::Word::sptr word) :
	m_word(word)
{
	m_listCounterparts.SetLabel("Counterparts:");
	m_listRelatedCards.SetLabel("Related Cards:");
	m_listRelatedTerms.SetLabel("Related Terms:");
	m_listDerivedTerms.SetLabel("Derived Terms:");
	m_listSynonyms.SetLabel("Synonyms:");
	m_listAntonyms.SetLabel("Antonyms:");

	m_layoutNounInfo.SetMargins(0.0f);
	m_layoutNounInfo.Add(&m_tableNounDeclension);
	m_widgetNounInfo.SetLayout(&m_layoutNounInfo);

	m_layoutAdjectiveInfo.SetMargins(0.0f);
	m_layoutAdjectiveInfo.Add(&m_tableAdjectiveDeclension);
	m_widgetAdjectiveInfo.SetLayout(&m_layoutAdjectiveInfo);

	m_layoutVerbParticiples.SetMargins(0.0f);
	m_layoutVerbParticiples.SetSpacing(4.0f);
	m_layoutVerbParticiples.Add(&m_labelInfinitive);
	m_layoutVerbParticiples.AddStretch();
	m_layoutVerbParticiples.Add(&m_tableVerbParticiples);
	m_layoutVerbInfo.Add(&m_tableVerbNonPast);
	m_layoutVerbInfo.Add(&m_tableVerbPast);
	m_layoutVerbInfo.Add(&m_layoutVerbParticiples);
	m_layoutVerbInfo.SetMargins(0.0f);
	m_layoutVerbInfo.SetSpacing(8.0f);
	m_widgetVerbInfo.SetLayout(&m_layoutVerbInfo);

	m_layoutDefinition.Add(&m_wordDefinitionWidget);
	m_layoutDefinition.AddStretch();
	m_mainLayout.Add(&m_layoutDefinition);
	SetLayout(&m_mainLayout);
	SetBackgroundColor(GUIConfig::color_background);
	SetFocusable(true);
}

wiki::Word::sptr WikiWordWidget::GetWord()
{
	return m_word;
}

void WikiWordWidget::OnInitialize()
{
	m_wordDefinitionWidget.SetWord(m_word);

	// Word-specific info and declensions
	if (m_word->GetWordType() == WordType::k_noun)
	{
		m_layoutDefinition.Add(&m_widgetNounInfo);
		wiki::Noun::sptr noun =
			std::dynamic_pointer_cast<wiki::Noun>(m_word);
		m_tableNounDeclension.InitNoun(noun->GetDeclension());
	}
	else if (m_word->GetWordType() == WordType::k_adjective)
	{
		m_layoutDefinition.Add(&m_widgetAdjectiveInfo);
		wiki::Adjective::sptr adjective =
			std::dynamic_pointer_cast<wiki::Adjective>(m_word);
		m_tableAdjectiveDeclension.InitAdjective(adjective->GetDeclension());
	}
	else if (m_word->GetWordType() == WordType::k_verb)
	{
		m_layoutDefinition.Add(&m_widgetVerbInfo);

		wiki::Verb::sptr verb =
			std::dynamic_pointer_cast<wiki::Verb>(m_word);

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
			widget->Clicked().Connect(this, widget, &WikiWordWidget::OnClickWordBox);
		}
		m_mainLayout.Add(&m_listCounterparts);
	}

	// Related terms
	if (PopulateTermList(m_listRelatedTerms, m_word->GetRelatedTerms()))
		m_mainLayout.Add(&m_listRelatedTerms);
	if (PopulateTermList(m_listDerivedTerms, m_word->GetDerivedTerms()))
		m_mainLayout.Add(&m_listDerivedTerms);
	if (PopulateTermList(m_listSynonyms, m_word->GetSynonyms()))
		m_mainLayout.Add(&m_listSynonyms);
	if (PopulateTermList(m_listAntonyms, m_word->GetAntonyms()))
		m_mainLayout.Add(&m_listAntonyms);
}

bool WikiWordWidget::PopulateTermList(RelatedWordList& termList,
	const Set<AccentedText>& terms)
{
	termList.Clear();
	if (terms.empty())
		return false;
	for (const AccentedText& term : terms)
	{
		RelatedWordWidget* widget = termList.AddWord(term);
		widget->Clicked().Connect(this, widget, &WikiWordWidget::OnClickWordBox);
	}
	return true;
}

void WikiWordWidget::OnClickWordBox(RelatedWordWidget* widget)
{
	auto& wiktionary = GetApp()->GetWiktionary();
	wiki::Term::sptr term = widget->GetWiktionaryTerm();
	if (!term)
		term = wiktionary.GetTerm(widget->GetText().GetString(), true);

	if (term)
	{
		GetApp()->PushState(new WikiTermView(term));
	}
}
