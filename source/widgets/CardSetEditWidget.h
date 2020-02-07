#pragma once

#include "widgets/CardSearchWidget.h"
#include "cards/CardSet.h"

class CardRow
{
public:
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
	void UpdateState();
	void Refresh();

private:
	void OnTypeModified();
	void OnEnglishModified();
	void OnRussianModified();

public:
	Card::sptr m_card;
	bool m_isNewCard = false;
	bool m_isNewToSet = false;
	RussianStudyToolApp* m_app = nullptr;
	CardSet::sptr m_cardSet;

	EnumFlags<CardTags> m_cardTags;
	TextEdit* m_inputRussian = nullptr;
	TextEdit* m_inputEnglish = nullptr;
	TextEdit* m_inputType = nullptr;
	TextEdit* m_inputCardTags = nullptr;
	Button* m_buttonEdit = nullptr;
	Button* m_buttonRemove = nullptr;
	EventSignal<> typeModified;
	EventSignal<> russianModified;
	EventSignal<> englishModified;
	EventSignal<> modified;

	// Cached
	WordType m_wordType;
	TranslationPair m_text;
	CardRuKey m_ruKey;
	CardEnKey m_enKey;
	bool m_isModified = false;
	bool m_validType = false;
	bool m_validRussian = false;
	bool m_validEnglish = false;
	bool m_isEnKeyUnique = true;
	bool m_isRuKeyUnique = true;
	bool m_isEmpty = true;
};

class CardSetEditWidget : public AppWidget
{
public:
	CardSetEditWidget(CardSet::sptr cardSet);

	void SelectCardSet(CardSet::sptr cardSet);
	CardRow::sptr AddCard(Card::sptr card, int32 index = - 1);
	CardRow::sptr AddEmptyRow(int32 index = -1);
	void ApplyChanges();
	void Refresh();

private:
	bool IsEnglishKeyUnique(Card::sptr card, const CardEnKey& key);
	bool IsRussianKeyUnique(Card::sptr card, const CardRuKey& key);
	void AddRow(CardRow::sptr row, int32 index = -1);
	void RemoveRow(CardRow::sptr row);
	void OnCardEnglishModified(CardRow::sptr row);
	void OnCardRussianModified(CardRow::sptr row);
	void OnCardEdited(CardRow::sptr row);
	void OnPressEnterRussian(CardRow::sptr row);
	void OnPressEnterType(CardRow::sptr row);
	void OnPressEnterEnglish(CardRow::sptr row);
	void OnClickEditCard(CardRow::sptr row);
	CardRow::sptr GetOrCreateAdjacentRow(CardRow::sptr row, bool previous);
	CardRow::sptr GetOrCreateRow(uint32 index);
	CardRow::sptr AutoCompleteRow(CardRow::sptr row);
	bool SearchFilter(Card::sptr card);

	CardSet::sptr m_cardSet;
	VBoxLayout m_setEditLayout;
	HBoxLayout m_mainLayout;
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
	AbstractScrollArea m_scrollArea;

	// Cached
	Set<Card::sptr> m_allCards;
	Map<CardEnKey, Card::sptr> m_englishToCards;
	Map<CardRuKey, Card::sptr> m_russianToCards;
};
