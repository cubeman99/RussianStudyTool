#pragma once

#include "widgets/CardSearchWidget.h"
#include "cards/CardSet.h"

struct CardRow
{
	using sptr = cmg::shared_ptr<CardRow>;

	bool IsEmpty() const;
	bool IsNewCard() const;
	bool IsNewToSet() const;
	bool IsModified() const;
	bool IsValid() const;
	WordType GetWordType() const;
	AccentedText GetRussian() const;
	AccentedText GetEnglish() const;

	void Initialize();
	void OnRussianChanged();
	void OnEnglishChanged();
	void OnTypeChanged();
	void RefreshAll();
	void RefreshType();
	void RefreshRussian();
	void RefreshEnglish();
	void UpdateState();
	void OnModified();

	Card::sptr m_card;
	bool m_isNewCard = false;
	RussianStudyToolApp* m_app = nullptr;
	CardSet::sptr m_cardSet;

	TextEdit* m_inputRussian = nullptr;
	TextEdit* m_inputEnglish = nullptr;
	TextEdit* m_inputType = nullptr;
	Button* m_buttonEdit = nullptr;
	Button* m_buttonRemove = nullptr;
	EventSignal<> typeModified;
	EventSignal<> russianModified;
	EventSignal<> englishModified;
	EventSignal<> modified;

	// Cached
	WordType m_wordType;
	CardRuKey m_ruKey;
	CardEnKey m_enKey;
	TranslationPair m_text;
	bool m_isModified = false;
	bool m_validType = false;
	bool m_validRussian = false;
	bool m_validEnglish = false;
	bool m_isEnKeyUnique = true;
	bool m_isRuKeyUnique = true;
};

class CardSetEditWidget : public AppWidget
{
public:
	CardSetEditWidget(CardSet::sptr cardSet);

	void SelectCardSet(CardSet::sptr cardSet);
	CardRow::sptr AddCard(Card::sptr card);
	CardRow::sptr AddEmptyRow();
	void ApplyChanges();
	void Refresh();

private:
	bool IsEnglishKeyUnique(Card::sptr card, const CardEnKey& key);
	bool IsRussianKeyUnique(Card::sptr card, const CardRuKey& key);
	void AddRow(CardRow::sptr row);
	void OnRemoveRow(CardRow::sptr row);
	void OnCardEdited(CardRow::sptr row);
	CardRow::sptr GetOrCreateAdjacentRow(CardRow::sptr row, bool previous);
	CardRow::sptr GetOrCreateRow(uint32 index);

	CardSet::sptr m_cardSet;
	VBoxLayout m_mainLayout;
	HBoxLayout m_layoutName;
	HBoxLayout m_layoutType;
	Button m_buttonSave;
	Button m_buttonDone;
	Button m_buttonCancel;
	HBoxLayout m_layoutButtons;
	TextEdit m_inputName;
	TextEdit m_inputType;
	GenericTableWidget<CardRow::sptr> m_table;
	Label m_labelName;
	Label m_labelType;
	CardSearchWidget m_searchWidget;

	// Cached
	Set<Card::sptr> m_allCards;
	Map<CardEnKey, Card::sptr> m_englishToCards;
	Map<CardRuKey, Card::sptr> m_russianToCards;
};
