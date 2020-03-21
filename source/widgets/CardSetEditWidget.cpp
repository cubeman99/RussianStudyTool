#include "CardSetEditWidget.h"
#include "RussianApp.h"


CardSetEditWidget::CardSetEditWidget(CardSet::sptr cardSet) :
	m_cardSet(cardSet),
	m_scrollAreaWordInfo(true, false)
{
	SetBackgroundColor(GUIConfig::color_background);

	m_layoutName.Add({&m_labelName, &m_inputName});
	m_layoutType.Add({&m_labelType, &m_inputType});
	m_layoutButtons.Add({&m_buttonSave, &m_buttonDone, &m_buttonCancel});
	m_scrollArea.SetWidget(&m_table);
	m_scrollAreaWordInfo.SetWidget(&m_wordDefinitionWidget);
	m_setEditLayout.Add({&m_layoutName, &m_layoutType, &m_scrollArea, &m_layoutButtons});

	m_buttonSave.SetText("Save");
	m_buttonDone.SetText("Done");
	m_buttonCancel.SetText("Cancel");
	m_labelName.SetText("Name:");
	m_labelType.SetText("Type:");

	m_table.SetSpacing(1.0f);
	m_layoutName.SetMargins(0.0f);
	m_layoutType.SetMargins(0.0f);
	m_layoutButtons.SetMargins(0.0f);
	m_setEditLayout.SetStretch(&m_scrollArea, 1.0f);
	m_layoutRight.Add(&m_searchWidget, 1.0f);
	m_layoutRight.Add(&m_scrollAreaWordInfo, 0.0f);
	m_mainLayout.Add(&m_setEditLayout, 2.0f);
	m_mainLayout.Add(&m_layoutRight, 1.0f);
	SetLayout(&m_mainLayout);

	// Create table
	m_table.AddColumn("#", 0.5f);
	m_table.AddColumn("Type", 1.0f);
	m_table.AddColumn("Russian", 5.0f);
	m_table.AddColumn("English", 5.0f);
	m_table.AddColumn("Tags", 1.0f);
	m_table.AddColumn("E", 0.5f);
	m_table.AddColumn("X", 0.5f);
	m_searchWidget.SetFilter(new MethodDelegate(
		this, &CardSetEditWidget::SearchFilter));
	m_searchWidget.ItemClicked().Connect(
		this, &CardSetEditWidget::OnClickSearchedCard);

	// Connect signals
	m_table.RowCreated().Connect(this, &CardSetEditWidget::OnRowCreated);
	m_buttonSave.Clicked().Connect(this, &CardSetEditWidget::ApplyChanges);
	AddKeyShortcut("Ctrl+S", [this]() {
		if (m_buttonSave.IsEnabled())
		{
			ApplyChanges();
			return true;
		}
		return false;
	});

	SelectCardSet(cardSet);
}

void CardSetEditWidget::SelectCardSet(CardSet::sptr cardSet)
{
	m_cardSet = cardSet;
	m_lastSelectedRow = nullptr;

	m_inputName.SetText(cardSet->GetName());
	m_inputType.SetText(EnumToString(cardSet->GetCardSetType()));

	// Add the card rows
	m_table.Clear();
	for (auto card : m_cardSet->GetCards())
		AddCard(card);
	AddEmptyRow();
	if (!m_table.GetItems().empty())
		m_table.GetItems().front()->m_inputRussian.Focus();

	m_buttonSave.SetEnabled(false);
}

CardRow::sptr CardSetEditWidget::AddCard(Card::sptr card, int32 index)
{
	CardRow::sptr row = cmg::make_shared<CardRow>();
	row->m_card = card;
	row->m_isNewCard = false;
	AddRow(row, index);
	row->m_isNewToSet = !m_cardSet->HasCard(card);
	Refresh();
	return row;
}

CardRow::sptr CardSetEditWidget::AddEmptyRow(int32 index)
{
	CardRow::sptr row = cmg::make_shared<CardRow>();
	row->m_card = cmg::make_shared<Card>();
	row->m_isNewCard = true;
	AddRow(row, index);
	row->m_inputType.SetText("");
	row->m_isNewToSet = true;
	Refresh();
	return row;
}

void CardSetEditWidget::ApplyChanges()
{
	CMG_LOG_INFO() << "Applying changes";

	auto& cardDatabase = GetApp()->GetCardDatabase();
	auto& studyDatabase = GetApp()->GetStudyDatabase();

	for (CardRow::sptr row : m_table.GetItems())
	{
		Card::sptr card = row->m_card;
		row->UpdateState();
		if (row->IsEmpty())
			continue;

		CardData cardData;
		cardData.text = row->m_text;
		cardData.type = row->m_wordType;
		cardData.tags = row->m_tagEditBox.GetModifiedTags();

		if (row->IsNewCard())
		{
			// Create a new card
			card->SetData(cardData);
			cardDatabase.AddCard(card);
		}
		else if (row->IsModified())
		{
			// Apply card changes
			cardDatabase.ModifyCard(card, cardData);
		}

		if (!m_cardSet->HasCard(card))
		{
			// Add card to set
			cardDatabase.AddCardToSet(card, m_cardSet);
		}

		row->m_tagEditBox.SetTags(cardData.tags);
		row->m_isModified = false;
		row->m_isNewCard = false;
		row->m_isNewToSet = false;
	}

	Refresh();
	cardDatabase.SaveChanges();
	studyDatabase.SaveChanges();
}

void CardSetEditWidget::Refresh()
{
	bool valid = true;
	bool modified = false;
	m_englishToCards.clear();
	m_russianToCards.clear();
	m_allCards.clear();
	for (CardRow::sptr row : m_table.GetItems())
	{
		row->UpdateState();
		m_englishToCards[row->m_enKey] = row->m_card;
		m_russianToCards[row->m_ruKey] = row->m_card;
		m_allCards.insert(row->m_card);
	}
	for (CardRow::sptr row : m_table.GetItems())
	{
		row->m_isEnKeyUnique = (row->m_enKey.english.empty() ||
			IsEnglishKeyUnique(row->m_card, row->m_enKey));
		row->m_isRuKeyUnique = (row->m_ruKey.russian.empty() ||
			IsRussianKeyUnique(row->m_card, row->m_ruKey));
		row->UpdateState();
		if (!row->IsValid() && !row->IsEmpty())
			valid = false;
		if (row->IsModified() || row->IsNewCard() || row->IsNewToSet())
			modified = true;
		row->Refresh();
	}
	m_buttonSave.SetEnabled(valid && modified);
}

void CardSetEditWidget::OnInitialize()
{
	if (!m_table.GetItems().empty())
		m_table.GetItems().front()->m_inputRussian.Focus();

	// Connect signals
	auto& wordDatabase = GetApp()->GetWordDatabase();
	wordDatabase.CardWordMatchLoaded().Connect(this, &CardSetEditWidget::OnWordMatchLoaded);
}

void CardSetEditWidget::OnRowCreated(GenericTableWidget<CardRow::sptr>::Row& rowItem)
{
	auto row = rowItem.item;
	auto card = row->m_card;
	uint32 index = m_table.GetIndex(row);
	
	row->m_labelNumber.SetText(std::to_string(index + 1));
	LoadRowWordMatch(row, true);

	row->m_inputType.SetText(EnumToShortString(card->GetWordType()));
	row->m_inputRussian.SetText(card->GetRussian().ToMarkedString());
	row->m_inputEnglish.SetText(card->GetEnglish().ToMarkedString());
	row->m_tagEditBox.SetTags(row->m_card->GetTags());
	row->m_buttonEdit.SetText("E");
	row->m_buttonRemove.SetText("X");

	rowItem.widgets.push_back(&row->m_labelNumber);
	rowItem.widgets.push_back(&row->m_inputType);
	rowItem.widgets.push_back(&row->m_inputRussian);
	rowItem.widgets.push_back(&row->m_inputEnglish);
	rowItem.widgets.push_back(&row->m_tagEditBox);
	rowItem.widgets.push_back(&row->m_buttonEdit);
	rowItem.widgets.push_back(&row->m_buttonRemove);

	// Connect signals
	row->m_buttonEdit.Clicked().Connect(
		this, row, &CardSetEditWidget::OnClickEditCard);
	row->m_buttonRemove.Clicked().Connect(
		this, row, &CardSetEditWidget::RemoveRow);
	row->m_inputType.ReturnPressed().Connect(
		this, row, &CardSetEditWidget::OnPressEnterType);
	row->m_inputRussian.ReturnPressed().Connect(
		this, row, &CardSetEditWidget::OnPressEnterRussian);
	row->m_inputEnglish.ReturnPressed().Connect(
		this, row, &CardSetEditWidget::OnPressEnterEnglish);
	row->modified.Connect(
		this, row, &CardSetEditWidget::OnCardEdited);
	row->typeModified.Connect(
		this, row, &CardSetEditWidget::OnCardWordTypeModified);
	row->englishModified.Connect(
		this, row, &CardSetEditWidget::OnCardEnglishModified);
	row->russianModified.Connect(
		this, row, &CardSetEditWidget::OnCardRussianModified);
	row->m_inputType.GainedFocus().Connect(
		this, row, &CardSetEditWidget::OnRowGainedFocus);
	row->m_inputRussian.GainedFocus().Connect(
		this, row, &CardSetEditWidget::OnRowGainedFocus);
	row->m_inputEnglish.GainedFocus().Connect(
		this, row, &CardSetEditWidget::OnRowGainedFocus);
	row->m_tagEditBox.GainedFocus().Connect(
		this, row, &CardSetEditWidget::OnRowGainedFocus);

	row->m_inputRussian.AddKeyShortcut("Ctrl+Space", [this, row]() {
		auto newRow = AutoCompleteRow(row);
		if (newRow != row)
			newRow->m_inputRussian.Focus();
		return true;
	});
	row->m_inputEnglish.AddKeyShortcut("Ctrl+Space", [this, row]() {
		auto newRow = AutoCompleteRow(row);
		if (newRow != row)
			newRow->m_inputEnglish.Focus();
		return true;
	});
	row->m_inputEnglish.AddKeyShortcut("Ctrl+P", [this, row]() {
		row->m_tagEditBox.ToggleTag(CardTags::k_perfective);
		if (row->m_tagEditBox.GetModifiedTags()[CardTags::k_perfective])
			row->m_tagEditBox.SetTag(CardTags::k_imperfective, false);
		Refresh();
		return true;
	});
	row->m_inputEnglish.AddKeyShortcut("Ctrl+I", [this, row]() {
		row->m_tagEditBox.ToggleTag(CardTags::k_imperfective);
		if (row->m_tagEditBox.GetModifiedTags()[CardTags::k_imperfective])
			row->m_tagEditBox.SetTag(CardTags::k_perfective, false);
		Refresh();
		return true;
	});
	row->m_inputEnglish.AddKeyShortcut("Ctrl+U", [this, row]() {
		row->m_tagEditBox.ToggleTag(CardTags::k_unidirectional);
		if (row->m_tagEditBox.GetModifiedTags()[CardTags::k_unidirectional])
			row->m_tagEditBox.SetTag(CardTags::k_multidirectional, false);
		Refresh();
		return true;
	});
	row->m_inputEnglish.AddKeyShortcut("Ctrl+M", [this, row]() {
		row->m_tagEditBox.ToggleTag(CardTags::k_multidirectional);
		if (row->m_tagEditBox.GetModifiedTags()[CardTags::k_multidirectional])
			row->m_tagEditBox.SetTag(CardTags::k_unidirectional, false);
		Refresh();
		return true;
	});
}

bool CardSetEditWidget::IsEnglishKeyUnique(Card::sptr card, const CardEnKey& key)
{
	// Check conflicts with cards in the card database
	auto& cardDatabase = GetApp()->GetCardDatabase();
	Card::sptr cardInDatabase = cardDatabase.GetCard(key);
	if (cardInDatabase != nullptr && cardInDatabase != card &&
		m_allCards.find(cardInDatabase) == m_allCards.end())
	{
		return false;
	}
	
	// Check conflicts with cards in the current set being edited
	for (auto c : m_table.GetItems())
	{
		if (c->m_card != card && c->m_enKey == key)
			return false;
	}

	return true;
}

bool CardSetEditWidget::IsRussianKeyUnique(Card::sptr card, const CardRuKey& key)
{
	// Check conflicts with cards in the card database
	auto& cardDatabase = GetApp()->GetCardDatabase();
	Card::sptr cardInDatabase = cardDatabase.GetCard(key);
	if (cardInDatabase != nullptr && cardInDatabase != card &&
		m_allCards.find(cardInDatabase) == m_allCards.end())
	{
		return false;
	}

	// Check conflicts with cards in the current set being edited
	for (auto c : m_table.GetItems())
	{
		if (c->m_card != card && c->m_ruKey == key)
			return false;
	}

	return true;
}

void CardSetEditWidget::AddRow(CardRow::sptr row, int32 index)
{
	row->m_app = GetApp();
	row->m_cardSet = m_cardSet;
	if (index >= 0)
		m_table.InsertItem(index, row);
	else
		m_table.AddItem(row);

	row->Initialize();
}

void CardSetEditWidget::RemoveRow(CardRow::sptr row)
{
	if (m_lastSelectedRow == row)
		m_lastSelectedRow = nullptr;
	m_table.RemoveItem(row);
	if (!row->m_isNewCard)
	{
		GetApp()->GetCardDatabase().RemoveCardFromSet(
			row->m_card, m_cardSet);
		GetApp()->GetCardDatabase().SaveCardSet(m_cardSet);
	}
	OnCardEdited(row);
}

void CardSetEditWidget::OnCardWordTypeModified(CardRow::sptr row)
{
}

void CardSetEditWidget::OnCardRussianModified(CardRow::sptr row)
{
	if (row->m_inputRussian.IsFocused())
	{
		auto str = row->GetRussian().GetString();
		if (!str.empty())
			m_searchWidget.SetSearchText(str);
	}
}

void CardSetEditWidget::OnCardEnglishModified(CardRow::sptr row)
{
	if (row->m_inputEnglish.IsFocused())
	{
		auto str = row->GetEnglish().GetString();
		if (!str.empty())
			m_searchWidget.SetSearchText(str);
	}
}

void CardSetEditWidget::OnCardEdited(CardRow::sptr row)
{
	Refresh();

	// Update word match, but do not download new words yet
	LoadRowWordMatch(row, false);
}

void CardSetEditWidget::OnRowGainedFocus(CardRow::sptr row)
{
	m_lastSelectedRow = row;
	m_wordDefinitionWidget.SetWord(row->m_wordMatch.GetWord());
}

void CardSetEditWidget::OnPressEnterType(CardRow::sptr row)
{
	// Download any new words
	LoadRowWordMatch(row, true);
	GetOrCreateAdjacentRow(row, false)->m_inputType.Focus();
}

void CardSetEditWidget::OnPressEnterRussian(CardRow::sptr row)
{
	// Predict the word type from the russian ending
	if (row->m_inputType.GetText().length() == 0)
	{
		row->UpdateState();
		WordType predictedWordType;
		if (ru::TryPredictWordType(row->m_ruKey.russian, predictedWordType))
		{
			unistr typeName = ConvertFromUTF8(EnumToShortString(predictedWordType));
			row->m_inputType.SetText(typeName);
			row->UpdateState();
		}
	}

	// Download any new words
	LoadRowWordMatch(row, true);

	GetOrCreateAdjacentRow(row, false)->m_inputRussian.Focus();
}

void CardSetEditWidget::OnPressEnterEnglish(CardRow::sptr row)
{
	GetOrCreateAdjacentRow(row, false)->m_inputEnglish.Focus();
}

void CardSetEditWidget::OnClickEditCard(CardRow::sptr row)
{
	// TODO: edit card
}

void CardSetEditWidget::OnClickSearchedCard(Card::sptr card)
{
	if (m_lastSelectedRow)
	{
		auto newRow = SetCardForRow(m_lastSelectedRow, card);
		if (newRow)
			newRow->m_inputRussian.Focus();
	}
}

CardRow::sptr CardSetEditWidget::GetOrCreateAdjacentRow(
	CardRow::sptr row, bool previous)
{
	int32 index = m_table.GetIndex(row);
	CMG_ASSERT(index >= 0);
	if (previous)
	{
		if (index == 0)
			return row;
		return GetOrCreateRow(index - 1);
	}
	else
	{
		return GetOrCreateRow(index + 1);
	}
}

CardRow::sptr CardSetEditWidget::GetOrCreateRow(uint32 index)
{
	if (index < m_table.GetCount())
		return m_table.GetItem((uint32) index);
	return AddEmptyRow();
}

CardRow::sptr CardSetEditWidget::AutoCompleteRow(CardRow::sptr row)
{
	Card::sptr card = m_searchWidget.GetTopResult();
	if (!card)
		return row;
	return SetCardForRow(row, card);
}

CardRow::sptr CardSetEditWidget::SetCardForRow(CardRow::sptr row, Card::sptr card)
{
	if (!card)
		return row;
	uint32 rowIndex = m_table.GetIndex(row);
	RemoveRow(row);
	CardRow::sptr newRow = AddCard(card, rowIndex);
	newRow = GetOrCreateAdjacentRow(newRow, false);
	m_searchWidget.RefreshResults();
	return newRow;
}

bool CardSetEditWidget::SearchFilter(Card::sptr card)
{
	return !cmg::container::Contains(m_allCards, card);
}

void CardSetEditWidget::LoadRowWordMatch(CardRow::sptr row, bool download)
{
	auto& wordDatabase = GetApp()->GetWordDatabase();
	CardWordMatch wordMatch = wordDatabase.GetWordFromCard(
		row->m_wordType, row->m_text.russian.GetString(), download, true);
	RefreshRowWordMatch(row, wordMatch);
}

void CardSetEditWidget::RefreshRowWordMatch(CardRow::sptr row,
	const CardWordMatch& wordMatch)
{
	row->m_wordMatch = wordMatch;
	Color labelColor = GUIConfig::color_text_box_background_text;
	auto part = wordMatch.GetPart();
	if (part && wordMatch.GetPart()->isLoaded)
	{
		if (!part->word)
			labelColor = Config::k_colorRed;
		else if (part->isMatchingType && part->isMatchingText)
			labelColor = Config::k_colorGreen;
		else
			labelColor = Config::k_colorYellow;
	}
	else if (!part)
		labelColor = Config::k_colorRed;
	row->m_labelNumber.SetColor(labelColor);

	if (row == m_lastSelectedRow)
		m_wordDefinitionWidget.SetWord(wordMatch.GetWord());
}

void CardSetEditWidget::OnWordMatchLoaded(const CardWordMatch& wordMatch)
{
	// Find the row for this card key
	CardRuKey key(wordMatch.wordType, AccentedText(wordMatch.text));
	auto cardIter = m_russianToCards.find(key);
	if (cardIter == m_russianToCards.end())
		return;
	Card::sptr card = cardIter->second;
	CardRow::sptr row;
	for (auto rowIter : m_table.GetItems())
	{
		if (rowIter->m_card == card)
		{
			row = rowIter;
			break;
		}
	}

	if (row)
		RefreshRowWordMatch(row, wordMatch);
}

bool CardRow::IsEmpty() const
{
	return m_isEmpty;
}

bool CardRow::IsNewCard() const
{
	return m_isNewCard;
}

bool CardRow::IsNewToSet() const
{
	return m_isNewToSet;
}

bool CardRow::IsModified() const
{
	return m_isModified;
}

bool CardRow::IsValid() const
{
	return (m_validType && m_validRussian && m_validEnglish &&
		m_isRuKeyUnique && m_isEnKeyUnique && !m_isEmpty);
}

WordType CardRow::GetWordType() const
{
	String text = ConvertToUTF8(m_inputType.GetText());
	WordType wordType = WordType::k_other;
	TryStringToEnum(text, wordType, true);
	return wordType;
}

AccentedText CardRow::GetRussian() const
{
	return AccentedText(m_inputRussian.GetText());
}

AccentedText CardRow::GetEnglish() const
{
	return AccentedText(m_inputEnglish.GetText());
}

void CardRow::Initialize()
{
	m_inputRussian.TextEdited().Connect(this, &CardRow::OnRussianModified);
	m_inputEnglish.TextEdited().Connect(this, &CardRow::OnEnglishModified);
	m_inputType.TextEdited().Connect(this, &CardRow::OnTypeModified);
	m_tagEditBox.TagsEdited().Connect(this, &CardRow::OnTagsModified);
}

void CardRow::UpdateState()
{
	auto& cardDatabase = m_app->GetCardDatabase();
	
	// Get and validate card info
	String typeText = ConvertToUTF8(m_inputType.GetText());
	m_wordType = WordType::k_other;
	m_validType = TryStringToEnum(typeText, m_wordType, true);
	m_text.russian = AccentedText(m_inputRussian.GetText());
	m_text.english = AccentedText(m_inputEnglish.GetText());
	m_ruKey = CardRuKey(m_wordType, m_text.russian);
	m_enKey = CardEnKey(m_wordType, m_text.english, m_tagEditBox.GetModifiedTags());
	m_isEmpty = (m_isNewCard && typeText.empty() && m_text.russian.empty() &&
		m_text.english.empty());
	m_validRussian = m_isRuKeyUnique && !m_text.russian.empty();
	m_validEnglish = m_isEnKeyUnique && !m_text.english.empty();
	m_isModified = (m_isNewCard || m_ruKey != m_card->GetRuKey() ||
		m_enKey != m_card->GetEnKey() || m_text != m_card->GetText());
}

void CardRow::Refresh()
{
	// WordType color
	if (m_isEmpty)
		m_inputType.SetBackgroundColor(GUIConfig::color_text_box_background);
	else if (!m_validType)
		m_inputType.SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isEnKeyUnique || !m_isRuKeyUnique)
		m_inputType.SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_isNewCard)
		m_inputType.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_wordType != m_card->GetWordType())
		m_inputType.SetBackgroundColor(Config::k_colorEditedModified);
	else if (m_isNewToSet)
		m_inputType.SetBackgroundColor(Config::k_colorEditedMatched);
	else
		m_inputType.SetBackgroundColor(GUIConfig::color_text_box_background);

	// Russian color
	if (m_isEmpty)
		m_inputRussian.SetBackgroundColor(GUIConfig::color_text_box_background);
	else if (m_text.russian.empty())
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isRuKeyUnique)
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_isNewCard)
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_text.russian != m_card->GetRussian())
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedModified);
	else if (m_isNewToSet)
		m_inputRussian.SetBackgroundColor(Config::k_colorEditedMatched);
	else
		m_inputRussian.SetBackgroundColor(GUIConfig::color_text_box_background);

	// English color
	if (m_isEmpty)
		m_inputEnglish.SetBackgroundColor(GUIConfig::color_text_box_background);
	else if (m_text.english.empty())
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isEnKeyUnique)
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_isNewCard)
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_text.english != m_card->GetEnglish())
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedModified);
	else if (m_isNewToSet)
		m_inputEnglish.SetBackgroundColor(Config::k_colorEditedMatched);
	else
		m_inputEnglish.SetBackgroundColor(GUIConfig::color_text_box_background);
}

void CardRow::OnTypeModified()
{
	typeModified.Emit();
	modified.Emit();
}

void CardRow::OnEnglishModified()
{
	englishModified.Emit();
	modified.Emit();
}

void CardRow::OnRussianModified()
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

	russianModified.Emit();
	modified.Emit();
}

void CardRow::OnTagsModified()
{
	modified.Emit();
}
