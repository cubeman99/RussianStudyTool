#include "CardSetEditWidget.h"
#include "RussianApp.h"

CardSetEditWidget::CardSetEditWidget(CardSet::sptr cardSet) :
	m_cardSet(cardSet)
{
	SetBackgroundColor(GUIConfig::color_background);

	m_layoutName.Add({&m_labelName, &m_inputName});
	m_layoutType.Add({&m_labelType, &m_inputType});
	m_layoutButtons.Add({&m_buttonSave, &m_buttonDone, &m_buttonCancel});
	m_scrollArea.SetWidget(&m_table);
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
	m_mainLayout.Add(&m_setEditLayout, 2.0f);
	m_mainLayout.Add(&m_searchWidget, 1.0f);
	SetLayout(&m_mainLayout);

	// Create table
	m_table.AddColumn("Type", [](CardRow::sptr row) {
		row->m_inputType = new TextEdit(
			EnumToShortString(row->m_card->GetWordType()));
		return (Widget*) row->m_inputType;
	}, 1.0f);
	m_table.AddColumn("Russian", [](CardRow::sptr row) {
		row->m_inputRussian = new TextEdit(
			row->m_card->GetRussian().ToMarkedString());
		return (Widget*) row->m_inputRussian;
	}, 5.0f);
	m_table.AddColumn("English", [](CardRow::sptr row) {
		row->m_inputEnglish = new TextEdit(
			row->m_card->GetEnglish().ToMarkedString());
		return (Widget*) row->m_inputEnglish;
	}, 5.0f);
	m_table.AddColumn("Tags", [](CardRow::sptr row) {
		row->m_inputCardTags = new TextEdit("");
		return (Widget*) row->m_inputCardTags;
	}, 1.0f);
	m_table.AddColumn("E", [](CardRow::sptr row) {
		row->m_buttonEdit = new Button("E");
		return (Widget*) row->m_buttonEdit;
	}, 0.5f);
	m_table.AddColumn("X", [](CardRow::sptr row) {
		row->m_buttonRemove = new Button("X");
		return (Widget*) row->m_buttonRemove;
	}, 0.5f);
	m_searchWidget.SetFilter(new MethodDelegate(
		this, &CardSetEditWidget::SearchFilter));

	SelectCardSet(cardSet);

	// Connect signals
	m_buttonSave.Clicked().Connect(this, &CardSetEditWidget::ApplyChanges);
	AddKeyShortcut("Ctrl+S", [this]() {
		if (m_buttonSave.IsEnabled())
		{
			ApplyChanges();
			return true;
		}
		return false;
	});
}

void CardSetEditWidget::SelectCardSet(CardSet::sptr cardSet)
{
	m_cardSet = cardSet;

	m_inputName.SetText(cardSet->GetName());
	m_inputType.SetText(EnumToString(cardSet->GetCardSetType()));

	// Add the card rows
	m_table.Clear();
	for (auto card : m_cardSet->GetCards())
		AddCard(card);
	AddEmptyRow();

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
	row->m_inputType->SetText("");
	row->m_isNewToSet = true;
	Refresh();
	return row;
}

void CardSetEditWidget::ApplyChanges()
{
	CMG_LOG_INFO() << "Applying changes";

	auto& cardDatabase = GetApp()->GetCardDatabase();

	for (CardRow::sptr row : m_table.GetItems())
	{
		Card::sptr card = row->m_card;
		row->UpdateState();
		if (row->IsEmpty())
			continue;

		CardData cardData;
		cardData.text = row->m_text;
		cardData.type = row->m_wordType;
		cardData.tags = row->m_cardTags;

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

		row->m_isModified = false;
		row->m_isNewCard = false;
		row->m_isNewToSet = false;
	}

	Refresh();
	cardDatabase.SaveChanges();
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
		row->m_isEnKeyUnique = IsEnglishKeyUnique(row->m_card, row->m_enKey);
		row->m_isRuKeyUnique = IsRussianKeyUnique(row->m_card, row->m_ruKey);
		row->UpdateState();
		if (!row->IsValid() && !row->IsEmpty())
			valid = false;
		if (row->IsModified() || row->IsNewCard() || row->IsNewToSet())
			modified = true;
		row->Refresh();
	}
	m_buttonSave.SetEnabled(valid && modified);
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

	row->m_cardTags = row->m_card->GetTags();

	row->m_buttonEdit->Clicked().Connect(
		this, row, &CardSetEditWidget::OnClickEditCard);
	row->m_buttonRemove->Clicked().Connect(
		this, row, &CardSetEditWidget::RemoveRow);
	row->m_inputType->ReturnPressed().Connect(
		this, row, &CardSetEditWidget::OnPressEnterType);
	row->m_inputRussian->ReturnPressed().Connect(
		this, row, &CardSetEditWidget::OnPressEnterRussian);
	row->m_inputEnglish->ReturnPressed().Connect(
		this, row, &CardSetEditWidget::OnPressEnterEnglish);
	row->modified.Connect(
		this, row, &CardSetEditWidget::OnCardEdited);
	row->englishModified.Connect(
		this, row, &CardSetEditWidget::OnCardEnglishModified);
	row->russianModified.Connect(
		this, row, &CardSetEditWidget::OnCardRussianModified);

	row->m_inputRussian->AddKeyShortcut("Ctrl+Space", [this, row]() {
		auto newRow = AutoCompleteRow(row);
		if (newRow != row)
			newRow->m_inputRussian->Focus();
		return true;
	});
	row->m_inputEnglish->AddKeyShortcut("Ctrl+Space", [this, row]() {
		auto newRow = AutoCompleteRow(row);
		if (newRow != row)
			newRow->m_inputEnglish->Focus();
		return true;
	});

	row->m_inputEnglish->AddKeyShortcut("Ctrl+P", [this, row]() {
		row->m_cardTags.Toggle(CardTags::k_perfective);
		if (row->m_cardTags[CardTags::k_perfective])
			row->m_cardTags.Set(CardTags::k_imperfective, false);
		Refresh();
		return true;
	});
	row->m_inputEnglish->AddKeyShortcut("Ctrl+I", [this, row]() {
		row->m_cardTags.Toggle(CardTags::k_imperfective);
		if (row->m_cardTags[CardTags::k_imperfective])
			row->m_cardTags.Set(CardTags::k_perfective, false);
		Refresh();
		return true;
	});
	row->m_inputEnglish->AddKeyShortcut("Ctrl+U", [this, row]() {
		row->m_cardTags.Toggle(CardTags::k_unidirectional);
		if (row->m_cardTags[CardTags::k_unidirectional])
			row->m_cardTags.Set(CardTags::k_multidirectional, false);
		Refresh();
		return true;
	});
	row->m_inputEnglish->AddKeyShortcut("Ctrl+M", [this, row]() {
		row->m_cardTags.Toggle(CardTags::k_multidirectional);
		if (row->m_cardTags[CardTags::k_multidirectional])
			row->m_cardTags.Set(CardTags::k_unidirectional, false);
		Refresh();
		return true;
	});

	row->Initialize();
}

void CardSetEditWidget::RemoveRow(CardRow::sptr row)
{
	m_table.RemoveItem(row);
	if (!row->m_isNewCard)
	{
		GetApp()->GetCardDatabase().RemoveCardFromSet(
			row->m_card, m_cardSet);
		GetApp()->GetCardDatabase().SaveCardSet(m_cardSet);
	}
	OnCardEdited(row);
}

void CardSetEditWidget::OnCardEnglishModified(CardRow::sptr row)
{
	if (row->m_inputEnglish->IsFocused())
	{
		auto str = row->GetEnglish().GetString();
		if (!str.empty())
			m_searchWidget.SetSearchText(str);
	}
}

void CardSetEditWidget::OnCardRussianModified(CardRow::sptr row)
{
	if (row->m_inputRussian->IsFocused())
	{
		auto str = row->GetRussian().GetString();
		if (!str.empty())
			m_searchWidget.SetSearchText(str);
	}
}

void CardSetEditWidget::OnCardEdited(CardRow::sptr row)
{
	Refresh();
}

void CardSetEditWidget::OnPressEnterType(CardRow::sptr row)
{
	GetOrCreateAdjacentRow(row, false)->m_inputType->Focus();
}

void CardSetEditWidget::OnPressEnterRussian(CardRow::sptr row)
{
	GetOrCreateAdjacentRow(row, false)->m_inputRussian->Focus();
}

void CardSetEditWidget::OnPressEnterEnglish(CardRow::sptr row)
{
	GetOrCreateAdjacentRow(row, false)->m_inputEnglish->Focus();
}

void CardSetEditWidget::OnClickEditCard(CardRow::sptr row)
{
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
	String text = ConvertToUTF8(m_inputType->GetText());
	WordType wordType = WordType::k_other;
	TryStringToEnum(text, wordType, true);
	return wordType;
}

AccentedText CardRow::GetRussian() const
{
	return AccentedText(m_inputRussian->GetText());
}

AccentedText CardRow::GetEnglish() const
{
	return AccentedText(m_inputEnglish->GetText());
}

void CardRow::Initialize()
{
	m_inputRussian->TextEdited().Connect(this, &CardRow::OnRussianModified);
	m_inputEnglish->TextEdited().Connect(this, &CardRow::OnEnglishModified);
	m_inputType->TextEdited().Connect(this, &CardRow::OnTypeModified);
}

void CardRow::UpdateState()
{
	auto& cardDatabase = m_app->GetCardDatabase();
	
	// Get and validate card info
	String typeText = ConvertToUTF8(m_inputType->GetText());
	m_wordType = WordType::k_other;
	m_validType = TryStringToEnum(typeText, m_wordType, true);
	m_text.russian = AccentedText(m_inputRussian->GetText());
	m_text.english = AccentedText(m_inputEnglish->GetText());
	m_ruKey = CardRuKey(m_wordType, m_text.russian);
	m_enKey = CardEnKey(m_wordType, m_text.english, m_cardTags);
	m_isEmpty = (m_isNewCard && typeText.empty() && m_text.russian.empty() &&
		m_text.english.empty());
	m_validRussian = m_isRuKeyUnique && !m_text.russian.empty();
	m_validEnglish = m_isEnKeyUnique && !m_text.english.empty();
	m_isModified = (m_isNewCard || m_ruKey != m_card->GetRuKey() ||
		m_enKey != m_card->GetEnKey());
}

void CardRow::Refresh()
{
	// Validate type
	if (m_isEmpty)
		m_inputType->SetBackgroundColor(GUIConfig::color_text_box_background);
	else if (!m_validType)
		m_inputType->SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isEnKeyUnique || !m_isRuKeyUnique)
		m_inputType->SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_isNewCard)
		m_inputType->SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_wordType != m_card->GetWordType())
		m_inputType->SetBackgroundColor(Config::k_colorEditedModified);
	else if (m_isNewToSet)
		m_inputType->SetBackgroundColor(Config::k_colorEditedMatched);
	else
		m_inputType->SetBackgroundColor(GUIConfig::color_text_box_background);

	// Validate russian
	if (m_isEmpty)
		m_inputRussian->SetBackgroundColor(GUIConfig::color_text_box_background);
	else if (m_text.russian.empty())
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isRuKeyUnique)
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_isNewCard)
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_text.russian != m_card->GetRussian())
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedModified);
	else if (m_isNewToSet)
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedMatched);
	else
		m_inputRussian->SetBackgroundColor(GUIConfig::color_text_box_background);

	// Validate english
	if (m_isEmpty)
		m_inputEnglish->SetBackgroundColor(GUIConfig::color_text_box_background);
	else if (m_text.english.empty())
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (!m_isEnKeyUnique)
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (m_isNewCard)
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_text.english != m_card->GetEnglish())
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedModified);
	else if (m_isNewToSet)
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedMatched);
	else
		m_inputEnglish->SetBackgroundColor(GUIConfig::color_text_box_background);

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
	m_inputCardTags->SetText(tagStr);
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
	unistr russian = m_inputRussian->GetText();
	size_t pos = russian.find(u"ээ");
	if (pos != std::string::npos)
	{
		russian.replace(pos, 2, u"'");
		m_inputRussian->SetText(russian);
		m_inputRussian->SetCursorPosition(pos + 1);
	}

	russianModified.Emit();
	modified.Emit();
}
