#include "CardSetEditWidget.h"
#include "RussianApp.h"

CardSetEditWidget::CardSetEditWidget(CardSet::sptr cardSet) :
	m_layoutName({&m_labelName, &m_inputName}),
	m_layoutType({&m_labelType, &m_inputType}),
	m_layoutButtons({&m_buttonSave, &m_buttonDone, &m_buttonCancel}),
	m_mainLayout({&m_layoutName, &m_layoutType, &m_table, &m_layoutButtons}),
	m_cardSet(cardSet)
{
	SetBackgroundColor(GUIConfig::color_background);

	m_buttonSave.SetText("Save");
	m_buttonDone.SetText("Done");
	m_buttonCancel.SetText("Cancel");
	m_labelName.SetText("Name:");
	m_labelType.SetText("Type:");
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
	m_table.AddColumn("E", [](CardRow::sptr row) {
		row->m_buttonEdit = new Button("E");
		return (Widget*) row->m_buttonEdit;
	}, 0.5f);
	m_table.AddColumn("X", [](CardRow::sptr row) {
		row->m_buttonRemove = new Button("X");
		return (Widget*) row->m_buttonRemove;
	}, 0.5f);

	SelectCardSet(cardSet);

	// Connect signals
	m_buttonSave.Clicked().Connect(this, &CardSetEditWidget::ApplyChanges);
		
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

CardRow::sptr CardSetEditWidget::AddCard(Card::sptr card)
{
	CardRow::sptr row = cmg::make_shared<CardRow>();
	row->m_card = card;
	row->m_isNewCard = false;
	AddRow(row);
	row->RefreshAll();
	return row;
}

CardRow::sptr CardSetEditWidget::AddEmptyRow()
{
	CardRow::sptr row = cmg::make_shared<CardRow>();
	row->m_card = cmg::make_shared<Card>();
	row->m_isNewCard = true;
	AddRow(row);
	row->m_inputType->SetText("");
	row->RefreshAll();
	return row;
}

void CardSetEditWidget::ApplyChanges()
{
	CMG_LOG_INFO() << "Applying changes";

	auto& cardDatabase = GetApp()->GetCardDatabase();

	uint32 index = 0;
	for (CardRow::sptr row : m_table.GetItems())
	{
		Card::sptr card = row->m_card;
		row->RefreshAll();

		CardData cardData;
		cardData.text = row->m_text;
		cardData.type = row->m_wordType;
		cardData.tags = card->GetTags();

		if (row->IsNewCard())
		{
			// Create a new card
			card->SetData(cardData);
			cardDatabase.AddCard(card);
			row->m_isNewCard = false;
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

		index++;
	}

	for (CardRow::sptr row : m_table.GetItems())
		row->RefreshAll();
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
		row->RefreshAll();

		m_englishToCards[row->m_enKey] = row->m_card;
		m_russianToCards[row->m_ruKey] = row->m_card;
		m_allCards.insert(row->m_card);
		if (!row->IsValid())
			valid = false;
		if (row->IsModified() || row->IsNewCard() || row->IsNewToSet())
			modified = true;
	}
	for (CardRow::sptr row : m_table.GetItems())
	{
		row->m_isEnKeyUnique = IsEnglishKeyUnique(row->m_card, row->m_enKey);
		row->m_isRuKeyUnique = IsRussianKeyUnique(row->m_card, row->m_ruKey);
		row->RefreshAll();
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

void CardSetEditWidget::AddRow(CardRow::sptr row)
{
	row->m_app = GetApp();
	row->m_cardSet = m_cardSet;
	m_table.AddItem(row);

	row->m_buttonEdit->Clicked().Connect([this, row]() {
		// TODO: edit card
	});
	row->m_buttonRemove->Clicked().Connect([this, row]() {
		OnRemoveRow(row);
	});
	row->m_inputType->ReturnPressed().Connect([this, row]() {
		GetOrCreateAdjacentRow(row, false)->m_inputType->Focus();
	});
	row->m_inputRussian->ReturnPressed().Connect([this, row]() {
		GetOrCreateAdjacentRow(row, false)->m_inputRussian->Focus();
	});
	row->m_inputEnglish->ReturnPressed().Connect([this, row]() {
		GetOrCreateAdjacentRow(row, false)->m_inputEnglish->Focus();
	});
	row->modified.Connect([this, row]() {
		OnCardEdited(row);
	});

	row->Initialize();
}

void CardSetEditWidget::OnRemoveRow(CardRow::sptr row)
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

void CardSetEditWidget::OnCardEdited(CardRow::sptr row)
{
	Refresh();
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

bool CardRow::IsEmpty() const
{
	return (m_inputType->GetText().empty() &&
		m_inputRussian->GetText().empty() &&
		m_inputEnglish->GetText().empty());
}

bool CardRow::IsNewCard() const
{
	return m_isNewCard;
}

bool CardRow::IsNewToSet() const
{
	return m_isNewCard; // TODO: IsNewToSet
}

bool CardRow::IsModified() const
{
	return m_isModified;
}

bool CardRow::IsValid() const
{
	return (m_validType && m_validRussian && m_validEnglish);
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
	m_inputRussian->TextEdited().Connect(this, &CardRow::OnRussianChanged);
	m_inputEnglish->TextEdited().Connect(this, &CardRow::OnEnglishChanged);
	m_inputType->TextEdited().Connect(this, &CardRow::OnTypeChanged);
}

void CardRow::OnRussianChanged()
{
	RefreshRussian();
	UpdateState();
	OnModified();
}

void CardRow::OnEnglishChanged()
{
	RefreshEnglish();
	UpdateState();
	OnModified();
}

void CardRow::OnTypeChanged()
{
	RefreshAll();
	OnModified();
}

void CardRow::RefreshAll()
{
	RefreshType();
	RefreshRussian();
	RefreshEnglish();
	UpdateState();
}

void CardRow::RefreshType()
{
	String text = ConvertToUTF8(m_inputType->GetText());
	m_wordType = WordType::k_other;
	m_validType = TryStringToEnum(text, m_wordType, true);

	if (!m_validType)
		m_inputType->SetBackgroundColor(Config::k_colorEditedInvalid);
	else if (m_isNewCard)
		m_inputType->SetBackgroundColor(Config::k_colorEditedNew);
	else if (m_wordType != m_card->GetWordType())
		m_inputType->SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputType->SetBackgroundColor(GUIConfig::color_text_box_background);
}

void CardRow::RefreshRussian()
{
	auto& cardDatabase = m_app->GetCardDatabase();

	// Convert 'ээ' to an accent mark (for when typing in russian mode)
	unistr russian = m_inputRussian->GetText();
	size_t pos = russian.find(u"ээ");
	if (pos != std::string::npos)
	{
		russian.replace(pos, 2, u"'");
		m_inputRussian->SetText(russian);
		m_inputRussian->SetCursorPosition(pos + 1);
	}

	AccentedText cardRussian(russian);
	m_text.russian = cardRussian;
	m_ruKey = CardRuKey(m_wordType, cardRussian);

	Card::sptr found = cardDatabase.GetCard(m_ruKey);

	m_validRussian = true;
	if (russian.empty())
	{
		m_validRussian = false;
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedInvalid);
	}
	else if (!m_isRuKeyUnique)
	{
		m_validRussian = false;
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedDuplicate);
		m_inputType->SetBackgroundColor(Config::k_colorEditedDuplicate);
	}
	else if (m_isNewCard)
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedNew);
	else if (cardRussian != m_card->GetRussian())
		m_inputRussian->SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputRussian->SetBackgroundColor(GUIConfig::color_text_box_background);
}

void CardRow::RefreshEnglish()
{
	auto& cardDatabase = m_app->GetCardDatabase();

	AccentedText cardEnglish(m_inputEnglish->GetText());
	m_text.english = cardEnglish;
	m_enKey = CardEnKey(m_wordType, cardEnglish, m_card->GetTags());

	Card::sptr found = cardDatabase.GetCard(m_enKey);
	bool duplicate = (found != nullptr && found != m_card);

	m_validEnglish = true;
	if (cardEnglish.empty())
	{
		m_validEnglish = false;
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedInvalid);
	}
	else if (!m_isEnKeyUnique)
	{
		m_validEnglish = false;
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedDuplicate);
		m_inputType->SetBackgroundColor(Config::k_colorEditedDuplicate);
	}
	else if (m_isNewCard)
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedNew);
	else if (cardEnglish != m_card->GetEnglish())
		m_inputEnglish->SetBackgroundColor(Config::k_colorEditedModified);
	else
		m_inputEnglish->SetBackgroundColor(GUIConfig::color_text_box_background);
}

void CardRow::UpdateState()
{
	m_isModified = (m_isNewCard || m_ruKey != m_card->GetRuKey() ||
		m_enKey != m_card->GetEnKey());
}

void CardRow::OnModified()
{
	AccentedText russian = GetRussian();
	AccentedText english = GetEnglish();
	WordType wordType = GetWordType();
	UpdateState();
	modified.Emit();
}
