#pragma once

#include "widgets/AppWidget.h"
#include "widgets/GenericTableWidget.h"
#include "cards/Card.h"
#include "gui/Events.h"


template <class T_Item>
class SearchWidget : public AppWidget
{
public:
	using FilterFunc = Delegate<bool, T_Item>;
	using ButtonClickedDelegate = Delegate<void, T_Item>;

	SearchWidget();
	virtual ~SearchWidget() {}

	virtual uint32 FindResults(unistr searchText, Array<T_Item>& results) = 0;

	TextEdit& GetSearchInputBox() { return m_inputSearch; }
	GenericTableWidget<T_Item>& GetTable() { return m_table; }
	T_Item GetTopResult();
	bool MatchesFilter(T_Item item);

	void SetSearchText(const unistr& text);
	void SetFilter(FilterFunc* function) { m_filterFunction = function; }
	EventSignal<T_Item>& ItemClicked() { return m_itemClicked; }
	void RefreshResults();
	
	virtual void OnInitialize() override;

private:
	struct ButtonInfo
	{
		AccentedText name;
		EventSignal<T_Item>* clicked;
	};
	Widget* CreateButton(ButtonInfo info, T_Item item);

private:
	EventSignal<T_Item> m_itemClicked;
	VBoxLayout m_mainLayout;
	HBoxLayout m_layoutSearchBox;
	HBoxLayout m_layoutResultsLabel;
	TextEdit m_inputSearch;
	AbstractScrollArea m_scrollArea;
	GenericTableWidget<T_Item> m_table;
	Label m_labelResultCount;
	Label m_labelResults;
	Label m_labelSearch;
	FilterFunc* m_filterFunction;
};

template<class T_Item>
inline SearchWidget<T_Item>::SearchWidget()
{
	SetBackgroundColor(GUIConfig::color_background);

	m_layoutSearchBox.Add({&m_labelSearch, &m_inputSearch});
	m_layoutResultsLabel.Add({&m_labelResults, &m_labelResultCount});
	m_scrollArea.SetWidget(&m_table);
	m_mainLayout.Add({&m_layoutSearchBox, &m_layoutResultsLabel, &m_scrollArea});

	m_layoutSearchBox.SetMargins(0.0f);
	m_layoutResultsLabel.SetMargins(0.0f);
	m_labelSearch.SetText("Search:");
	m_labelResults.SetText("Results:");
	m_labelResultCount.SetText("");

	SetLayout(&m_mainLayout);

	// Connect signals
	m_inputSearch.TextEdited().Connect(
		this, &SearchWidget::RefreshResults);
}

template<class T_Item>
inline void SearchWidget<T_Item>::SetSearchText(const unistr & text)
{
	m_inputSearch.SetText(text);
}

template<class T_Item>
inline void SearchWidget<T_Item>::RefreshResults()
{
	// Find the results
	unistr searchText = m_inputSearch.GetText();
	Array<T_Item> results;
	uint32 resultCount = FindResults(searchText, results);

	// Update the table
	m_table.Clear();
	for (const T_Item& result : results)
		m_table.AddItem(result);
	m_labelResultCount.SetText(std::to_string(resultCount));
}

template<class T_Item>
inline void SearchWidget<T_Item>::OnInitialize()
{
	RefreshResults();
}

template<class T_Item>
inline bool SearchWidget<T_Item>::MatchesFilter(T_Item item)
{
	return (!m_filterFunction || m_filterFunction->Call(item));
}

template<class T_Item>
inline T_Item SearchWidget<T_Item>::GetTopResult()
{
	if (m_table.GetCount() > 0)
		return m_table.GetItem(0);
	return nullptr;
}

template<class T_Item>
inline Widget* SearchWidget<T_Item>::CreateButton(ButtonInfo info, T_Item card)
{
	Button* button = AllocateObject<Button>(info.name);
	button->Clicked().Connect(card, info.clickedCallback.Emit)
		//new CaptureMethodDelegate<Delegate<void, T_Item>, T_Item, void>(
		//	card, info.clickedCallback));
	return (Widget*) button;
}
