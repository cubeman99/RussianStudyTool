#include "CardEditWidget.h"
#include "RussianApp.h"

CardEditWidget::CardEditWidget(Card::sptr card) :
	m_card(card),
	m_labelEnglish("English:"),
	m_labelRussian("Russian:"),
	m_labelType("Word Type:"),
	m_labelCardTags("Tags:"),
	m_buttonSave("Save"),
	m_buttonDone("Done"),
	m_buttonCancel("Cancel")
{
	SetBackgroundColor(GUIConfig::color_background);

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
	m_mainLayout.Add(&m_layoutRussian);
	m_mainLayout.Add(&m_layoutEnglish);
	m_mainLayout.Add(&m_layoutType);
	m_mainLayout.Add(&m_layoutCardTags);
	m_mainLayout.AddStretch();
	m_mainLayout.Add(&m_layoutButtons);
	SetLayout(&m_mainLayout);

	SelectCard(card);

	// Connect signals
	m_buttonSave.Clicked().Connect(this, &CardEditWidget::OnClickSave);
	m_buttonDone.Clicked().Connect(this, &CardEditWidget::OnClickDone);
	m_buttonCancel.Clicked().Connect(this, &CardEditWidget::OnClickCancel);
	m_inputRussian.TextEdited().Connect(this, &CardEditWidget::OnRussianEdited);
	m_inputEnglish.TextEdited().Connect(this, &CardEditWidget::OnEnglishEdited);
	m_inputType.TextEdited().Connect(this, &CardEditWidget::OnTypeEdited);
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
	m_inputRussian.Focus();
}

void CardEditWidget::SelectCard(Card::sptr card)
{
	m_inputRussian.SetText(card->GetRussian().ToMarkedString());
	m_inputEnglish.SetText(card->GetEnglish().ToMarkedString());
	m_inputType.SetText(EnumToShortString(card->GetWordType()));
	m_cardTags = m_card->GetTags();

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

	Refresh();
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
	m_isModified = (m_isNewCard || m_ruKey != m_card->GetRuKey() ||
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
	else if (m_isNewCard)
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
	else if (m_isNewCard)
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
	else if (m_isNewCard)
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_text.english != m_card->GetEnglish())
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputEnglish.SetBackgroundColor(GUIConfig::color_text_box_background);

	// Card tags color
	if (m_isNewCard)
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

	CardData cardData = m_card->GetData();
	cardData.text = m_text;
	cardData.type = m_wordType;
	cardDatabase.ModifyCard(m_card, cardData);

	cardDatabase.SaveChanges();
	studyDatabase.SaveChanges();

	SelectCard(m_card);
	m_eventCardModified.Emit(m_card);
}
