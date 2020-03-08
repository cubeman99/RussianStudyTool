#include "CardEditWidget.h"
#include "RussianApp.h"

CardEditWidget::CardEditWidget(Card::sptr card) :
	CardEditWidget()
{
	SelectCard(card);
}

CardEditWidget::CardEditWidget(const CardData& cardData) :
	CardEditWidget()
{
	SetCardData(cardData);
}

CardEditWidget::CardEditWidget() :
	m_labelEnglish("English:"),
	m_labelRussian("Russian:"),
	m_labelType("Word Type:"),
	m_labelCardTags("Tags:"),
	m_buttonSave("Save"),
	m_buttonDone("Done"),
	m_buttonCancel("Cancel"),
	m_labelTitle("Create New Card")
{
	SetBackgroundColor(GUIConfig::color_background);

	m_layoutTitle.Add(&m_labelTitle);
	m_widgetTitle.SetBackgroundColor(GUIConfig::color_background_light);
	m_widgetTitle.SetLayout(&m_layoutTitle);

	// Create layouts
	m_layoutRussian.Add(&m_labelRussian);
	m_layoutRussian.Add(&m_inputRussian);
	m_layoutEnglish.Add(&m_labelEnglish);
	m_layoutEnglish.Add(&m_inputEnglish);
	m_layoutType.Add(&m_labelType);
	m_layoutType.Add(&m_inputType);
	m_layoutCardTags.Add(&m_labelCardTags);
	m_layoutCardTags.Add(&m_inputCardTags);
	m_layoutButtons.Add(&m_buttonSave);
	m_layoutButtons.Add(&m_buttonDone);
	m_layoutButtons.Add(&m_buttonCancel);
	m_mainLayout.Add(&m_widgetTitle);
	m_mainLayout.Add(&m_layoutRussian);
	m_mainLayout.Add(&m_layoutEnglish);
	m_mainLayout.Add(&m_layoutType);
	m_mainLayout.Add(&m_layoutCardTags);
	m_mainLayout.Add(&m_wordDefinitionWidget);
	m_mainLayout.AddStretch();
	m_mainLayout.Add(&m_layoutButtons);
	SetLayout(&m_mainLayout);

	Refresh();

	// Connect signals
	m_buttonSave.Clicked().Connect(this, &CardEditWidget::OnClickSave);
	m_buttonDone.Clicked().Connect(this, &CardEditWidget::OnClickDone);
	m_buttonCancel.Clicked().Connect(this, &CardEditWidget::OnClickCancel);
	m_inputRussian.TextEdited().Connect(this, &CardEditWidget::OnRussianEdited);
	m_inputEnglish.TextEdited().Connect(this, &CardEditWidget::OnEnglishEdited);
	m_inputType.TextEdited().Connect(this, &CardEditWidget::OnTypeEdited);
	m_inputRussian.ReturnPressed().Connect(this, &CardEditWidget::OnRussianReturnPressed);
	m_inputEnglish.ReturnPressed().Connect((Widget*) &m_inputType, &Widget::Focus);
	m_inputType.ReturnPressed().Connect((Widget*) &m_inputCardTags, &Widget::Focus);
	AddKeyShortcut("Ctrl+S", [this]() {
		if (m_buttonSave.IsEnabled())
		{
			ApplyChanges();
			return true;
		}
		return false;
	});
}

void CardEditWidget::OnInitialize()
{
	Refresh();
	m_inputRussian.Focus();
	if (m_card)
	{
		auto& wordDatabase = GetApp()->GetWordDatabase();
		wordDatabase.GetWordFromCard(m_card, m_term, m_wikiWord);
		m_wordDefinitionWidget.SetWord(m_wikiWord);
	}
}

void CardEditWidget::SelectCard(Card::sptr card)
{
	m_card = card;
	m_term = nullptr;
	m_wikiWord = nullptr;
	CardData cardData;
	if (card)
	{
		cardData = card->GetData();
		m_labelTitle.SetText("Editing Card " + card->GetRussian());
	}
	else
	{
		m_labelTitle.SetText("Create New Card");
	}
	SetCardData(cardData);
	Refresh();
}

void CardEditWidget::SetCardData(const CardData& cardData)
{
	m_inputRussian.SetText(cardData.text.russian.ToMarkedString());
	m_inputEnglish.SetText(cardData.text.english.ToMarkedString());
	m_inputType.SetText(EnumToShortString(cardData.type));
	m_cardTags = cardData.tags;

	String tagStr = "";
	for (auto it : m_cardTags)
	{
		if (it.second)
		{
			if (!tagStr.empty())
				tagStr += ", ";
			tagStr += EnumToShortString(it.first);
		}
	}
	m_inputCardTags.SetText(tagStr);
}

void CardEditWidget::UpdateState()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();

	// Get and validate card info
	String typeText = ConvertToUTF8(m_inputType.GetText());
	m_wordType = WordType::k_other;
	m_validType = TryStringToEnum(typeText, m_wordType, true);
	m_text.russian = AccentedText(m_inputRussian.GetText());
	m_text.english = AccentedText(m_inputEnglish.GetText());
	m_ruKey = CardRuKey(m_wordType, m_text.russian);
	m_enKey = CardEnKey(m_wordType, m_text.english, m_cardTags);

	Card::sptr cardFromEnKey = cardDatabase.GetCard(m_enKey);
	Card::sptr cardFromRuKey = cardDatabase.GetCard(m_ruKey);
	m_isEnKeyUnique = (cardFromEnKey == nullptr || cardFromEnKey == m_card);
	m_isRuKeyUnique = (cardFromRuKey == nullptr || cardFromRuKey == m_card);

	m_validRussian = m_isRuKeyUnique && !m_text.russian.empty();
	m_validEnglish = m_isEnKeyUnique && !m_text.english.empty();
	m_isModified = (m_card == nullptr || m_ruKey != m_card->GetRuKey() ||
		m_enKey != m_card->GetEnKey() || m_text != m_card->GetText());
}

void CardEditWidget::Refresh()
{
	UpdateState();

	// WordType color
	if (!m_validType)
		m_inputType.SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isEnKeyUnique || !m_isRuKeyUnique)
		m_inputType.SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_card == nullptr)
		m_inputType.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_wordType != m_card->GetWordType())
		m_inputType.SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputType.SetBackgroundColor(GUIConfig::color_text_box_background);

	// Russian color
	if (m_text.russian.empty())
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isRuKeyUnique)
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_card == nullptr)
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_text.russian != m_card->GetRussian())
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputRussian.SetBackgroundColor(GUIConfig::color_text_box_background);

	// English color
	if (m_text.english.empty())
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isEnKeyUnique)
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_card == nullptr)
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_text.english != m_card->GetEnglish())
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputEnglish.SetBackgroundColor(GUIConfig::color_text_box_background);

	// Card tags color
	if (m_card == nullptr)
		m_inputCardTags.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_cardTags != m_card->GetTags())
		m_inputCardTags.SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputCardTags.SetBackgroundColor(GUIConfig::color_text_box_background);

	bool valid = (m_validType && m_validRussian && m_validEnglish);
	m_buttonSave.SetEnabled(valid && m_isModified);
	m_buttonDone.SetEnabled(valid);
}

void CardEditWidget::OnClickSave()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	ApplyChanges();
}

void CardEditWidget::OnRussianEdited()
{
	// Convert 'ээ' to an accent mark (for when typing in russian mode)
	unistr russian = m_inputRussian.GetText();
	size_t pos = russian.find(u"ээ");
	if (pos != std::string::npos)
	{
		russian.replace(pos, 2, u"'");
		m_inputRussian.SetText(russian);
		m_inputRussian.SetCursorPosition(pos + 1);
	}

	Refresh();
}

void CardEditWidget::OnEnglishEdited()
{
	Refresh();
}

void CardEditWidget::OnTypeEdited()
{
	Refresh();
}

void CardEditWidget::OnRussianReturnPressed()
{
	// Predict the word type from the russian ending
	if (m_inputType.GetText().length() == 0)
	{
		UpdateState();
		WordType predictedWordType;
		if (ru::TryPredictWordType(m_ruKey.russian, predictedWordType))
		{
			unistr typeName = ConvertFromUTF8(EnumToShortString(predictedWordType));
			m_inputType.SetText(typeName);
		}
	}

	m_inputEnglish.Focus();
}

void CardEditWidget::OnClickDone()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	ApplyChanges();
}

void CardEditWidget::OnClickCancel()
{
	Close();
}

void CardEditWidget::ApplyChanges()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	auto& studyDatabase = GetApp()->GetStudyDatabase();

	UpdateState();

	CardData cardData;
	if (m_card)
		cardData = m_card->GetData();
	cardData.text = m_text;
	cardData.type = m_wordType;
	cardData.tags = m_cardTags;

	if (!m_card)
	{
		// Create a new card
		cardDatabase.CreateCard(cardData, m_card);
	}
	else
	{
		// Apply card changes
		cardDatabase.ModifyCard(m_card, cardData);
	}

	cardDatabase.SaveChanges();
	studyDatabase.SaveChanges();

	SelectCard(m_card);
	m_eventCardModified.Emit(m_card);
}
