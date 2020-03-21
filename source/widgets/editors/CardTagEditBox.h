#pragma once

#include "widgets/AppWidget.h"
#include "external/wiktionary/Term.h"
#include "cards/Card.h"


class CardTagEditBox : public AppWidget
{
public:
	friend class RelatedWordList;

	CardTagEditBox();
	   
	bool IsModified() const;
	const EnumFlags<CardTags>& GetModifiedTags() const;
	const EnumFlags<CardTags>& GetOriginalTags() const;

	void SetTags(const EnumFlags<CardTags>& tags);
	void SetTag(CardTags tag, bool enabled);
	void ToggleTag(CardTags tag);
	void SetReadOnly(bool readOnly);

	virtual void OnInitialize() override;

	EventSignal<>& TagsEdited() { return m_eventTagsEdited; }

private:
	void OnClickRemoveTag(CardTags tag);
	void RemoveTag(CardTags tag);
	void OnTextEdited();
	void OnReturnPressed();
	Widget* AddTagWidget(CardTags tag);
	void Refresh();

	bool m_readOnly = false;
	EnumFlags<CardTags> m_tags;
	EnumFlags<CardTags> m_modifiedTags;
	HBoxLayout m_mainLayout;
	TextEdit m_textEdit;
	Widget m_stretchWidget;
	Map<CardTags, Widget*> m_tagWidgets;
	EventSignal<> m_eventTagsEdited;
};
