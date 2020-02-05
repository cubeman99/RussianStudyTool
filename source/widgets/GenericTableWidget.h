#pragma once

#include "widgets/AppWidget.h"
#include "gui/GridLayout.h"

template <class T_Item>
class GenericTableWidget : public AppWidget
{
public:
	using WidgetCreateFunc = Widget* (*)(T_Item item);
	GenericTableWidget();

	T_Item& GetItem(uint32 index);
	Array<T_Item>& GetItems();
	int32 GetIndex(T_Item item) const;
	uint32 GetCount() const;

	void AddTextColumn(const AccentedText& name, float stretch = 0.0f);
	void AddColumn(const AccentedText& name, WidgetCreateFunc func, float stretch = 0.0f);
	void Clear();
	void AddItem(T_Item item);
	void RemoveItem(T_Item item);
	void InsertItem(uint32 index, T_Item item);

private:
	struct Column
	{
		AccentedText name;
		float stretch = 0.0f;
		Label* headerLabel = nullptr;
		WidgetCreateFunc createFunction = nullptr;
	};
	void AddColumn(Column column);

	GridLayout m_gridLayout;
	Array<Column> m_columns;
	Array<T_Item> m_items;
};

template<class T_Item>
inline T_Item & GenericTableWidget<T_Item>::GetItem(uint32 index)
{
	return m_items[index];
}

template<class T_Item>
inline Array<T_Item>& GenericTableWidget<T_Item>::GetItems()
{
	return m_items;
}

template<class T_Item>
inline int32 GenericTableWidget<T_Item>::GetIndex(T_Item item) const
{
	return cmg::container::GetIndex(m_items, item);
}

template<class T_Item>
inline uint32 GenericTableWidget<T_Item>::GetCount() const
{
	return m_items.size();
}

template <class T_Item>
GenericTableWidget<T_Item>::GenericTableWidget()
{
	m_gridLayout.SetMargins(0);
	m_gridLayout.SetSpacing(0);
	SetLayout(&m_gridLayout);
}

template <class T_Item>
void GenericTableWidget<T_Item>::AddTextColumn(const AccentedText& name, float stretch)
{
	Column column;
	column.name = name;
	column.stretch = stretch;
	column.createFunction = nullptr;
	AddColumn(column);
}

template <class T_Item>
void GenericTableWidget<T_Item>::AddColumn(const AccentedText& name, WidgetCreateFunc func, float stretch)
{
	Column column;
	column.name = name;
	column.stretch = stretch;
	column.createFunction = func;
	AddColumn(column);
}

template<class T_Item>
inline void GenericTableWidget<T_Item>::Clear()
{
	m_items.clear();
	m_gridLayout.SetRowCount(1);
}

template <class T_Item>
inline void GenericTableWidget<T_Item>::AddItem(T_Item item)
{
	InsertItem(m_items.size(), item);
}

template<class T_Item>
inline void GenericTableWidget<T_Item>::InsertItem(uint32 index, T_Item item)
{
	m_items.insert(m_items.begin() + index, item);
	m_gridLayout.InsertRow(index + 1);
	for (uint32 columnIndex = 0; columnIndex < m_columns.size(); columnIndex++)
	{
		Column& column = m_columns[columnIndex];
		if (column.createFunction)
		{
			Widget* widget = column.createFunction(item);
			m_gridLayout.Add(widget, index + 1, columnIndex);
		}
	}
}

template<class T_Item>
inline void GenericTableWidget<T_Item>::RemoveItem(T_Item item)
{
	auto it = std::find(m_items.begin(), m_items.end(), item);
	if (it != m_items.end())
	{
		uint32 index = it - m_items.begin();
		m_gridLayout.RemoveRow(index + 1, true);
		m_items.erase(it);
	}
}

template <class T_Item>
void GenericTableWidget<T_Item>::AddColumn(Column column)
{
	uint32 columnIndex = m_columns.size();
	column.headerLabel = new Label(column.name);
	m_gridLayout.Add(column.headerLabel, 0, columnIndex);
	m_gridLayout.SetColumnStretch(columnIndex, column.stretch);
	m_columns.push_back(column);
}
