#include "GridLayout.h"

GridLayout::GridLayout()
{
	SetMargins(8);
	SetSpacing(4);
}

void GridLayout::Clear()
{
	m_children.clear();
	for (auto it : m_children)
		it.second->SetParent(nullptr);
}

uint32 GridLayout::GetRowCount() const
{
	return m_axes[Axis::k_row].count;
}

uint32 GridLayout::GetColumnCount() const
{
	return m_axes[Axis::k_column].count;
}

GUIObject* GridLayout::Get(uint32 row, uint32 column)
{
	return Get(GridLoc(column, row));
}

void GridLayout::SetSpacing(float spacing)
{
	m_axes[Axis::k_column].spacing = spacing;
	m_axes[Axis::k_row].spacing = spacing;
}

void GridLayout::SetMargins(float margins)
{
	m_axes[Axis::k_column].marginBegin = margins;
	m_axes[Axis::k_column].marginEnd = margins;
	m_axes[Axis::k_row].marginBegin = margins;
	m_axes[Axis::k_row].marginEnd = margins;
}

void GridLayout::SetRowCount(uint32 rowCount)
{
	SetSliceCount(Axis::k_row, rowCount);
}

void GridLayout::SetColumnCount(uint32 columnCount)
{
	SetSliceCount(Axis::k_column, columnCount);
}

void GridLayout::SetRowStretch(uint32 row, float stretch)
{
	m_axes[Axis::k_row].stretchFactors[row] = stretch;
}

void GridLayout::SetColumnStretch(uint32 column, float stretch)
{
	m_axes[Axis::k_column].stretchFactors[column] = stretch;
}

void GridLayout::Add(GUIObject* item, uint32 row, uint32 column)
{
	GridLoc loc(column, row);
	if (row + 1 > m_axes[Axis::k_row].count)
		m_axes[Axis::k_row].count = row + 1;
	if (column + 1 > m_axes[Axis::k_column].count)
		m_axes[Axis::k_column].count = column + 1;
	auto it = m_children.find(loc);
	m_children[loc] = item;

	// Order is important here
	item->SetParent(this);
	if (it != m_children.end())
		it->second->SetParent(nullptr);
}

GUIObject* GridLayout::Remove(uint32 row, uint32 column)
{
	return Remove(GridLoc(column, row));
}

void GridLayout::RemoveRow(uint32 row, bool shiftUp)
{
	RemoveSlice(Axis::k_row, row, shiftUp);
}

void GridLayout::InsertRow(uint32 row)
{
	InsertSlice(Axis::k_row, row, 1);
}

void GridLayout::MoveRow(uint32 from, uint32 to)
{
	MoveSlice(Axis::k_row, from, to);
}

uint32 GridLayout::GetNumChildren() const
{
	return m_children.size();
}

GUIObject* GridLayout::GetChild(uint32 index)
{
	auto it = m_children.begin();
	for (uint32 i = 0; i < index; i++)
		it++;
	return it->second;
}

void GridLayout::CalcSizes()
{
	m_minSize = Vector2f(0.0f, 0.0f);
	m_maxSize = Vector2f(GUIObject::DEFAULT_MAX_SIZE);

	for (uint32 axis = 0; axis < 2; axis++)
	{
		Axis& axisInfo = m_axes[axis];
		uint32 itemCount = m_axes[1 - axis].count;
		axisInfo.slices.resize(axisInfo.count);
		axisInfo.slices.resize(axisInfo.count);
		for (uint32 i = 0; i < axisInfo.count; i++)
		{
			axisInfo.slices[i].minSize = 0.0f;
			axisInfo.slices[i].maxSize = GUIObject::DEFAULT_MAX_SIZE;
		}
		for (auto it : m_children)
		{
			uint32 slice = it.first[axis];
			auto& sliceInfo = axisInfo.slices[slice];
			float minSize = it.second->GetMinSize()[axis];
			if (minSize > sliceInfo.minSize)
				sliceInfo.minSize = minSize;
		}
		m_maxSize[axis] = 0.0f;
		for (uint32 i = 0; i < axisInfo.count; i++)
		{
			m_minSize[axis] += axisInfo.slices[i].minSize;
			m_maxSize[axis] += axisInfo.slices[i].maxSize;
		}

		float pad = axisInfo.marginBegin + axisInfo.marginEnd +
			(axisInfo.spacing * ((float) axisInfo.count - 1));
		m_minSize[axis] += pad;
		m_maxSize[axis] += pad;
	}
}

void GridLayout::Update(float timeDelta)
{
	if (m_children.empty())
		return;

	Rect2f rect = GetBounds();

	for (uint32 axis = 0; axis < 2; axis++)
	{
		Axis& axisInfo = m_axes[axis];
		axisInfo.slices.resize(axisInfo.count);
		axisInfo.slices.resize(axisInfo.count);

		float stretchSpace = rect.size[axis];
		stretchSpace -= axisInfo.marginBegin + axisInfo.marginEnd;
		stretchSpace -= axisInfo.spacing * (axisInfo.count - 1);

		float stretchFactorSum = 0.0f;
		bool fixedStretch = false;
		for (uint32 i = 0; i < axisInfo.count; i++)
		{
			auto& sliceInfo = axisInfo.slices[i];
			sliceInfo.stretch = axisInfo.GetStretchFactor(i);
			stretchFactorSum += sliceInfo.stretch;
			sliceInfo.isMaxed = false;
			sliceInfo.isMinned = false;
		}
		if (stretchFactorSum == 0.0f)
			fixedStretch = true; 

		for (uint32 k = 0; k < 5; k++)
		{
			float offset = 0.0f;
			for (uint32 i = 0; i < axisInfo.count; i++)
			{
				auto& sliceInfo = axisInfo.slices[i];
				if (!sliceInfo.isMaxed && !sliceInfo.isMinned)
				{
					float percent = 1.0f / axisInfo.count;
					if (!fixedStretch)
						percent = sliceInfo.stretch / stretchFactorSum;
					sliceInfo.size = stretchSpace * percent;
					sliceInfo.isMaxed = sliceInfo.size > sliceInfo.maxSize;
					sliceInfo.isMinned = sliceInfo.size < sliceInfo.minSize;
					sliceInfo.size = Math::Clamp(sliceInfo.size,
						sliceInfo.minSize, sliceInfo.maxSize);
					if (sliceInfo.isMaxed || sliceInfo.isMinned)
					{
						stretchSpace -= sliceInfo.size;
						stretchFactorSum -= fixedStretch ? 1.0f : sliceInfo.stretch;
					}
				}
				sliceInfo.offset = offset;
				offset += sliceInfo.size + axisInfo.spacing;
			}
		}

		for (auto it : m_children)
		{
			uint32 slice = it.first[axis];
			auto& sliceInfo = axisInfo.slices[slice];
			GUIObject* child = it.second;
			Rect2f childRect = child->GetBounds();
			childRect.position[axis] = axisInfo.marginBegin +
				rect.position[axis] + sliceInfo.offset;
			childRect.size[axis] = sliceInfo.size;
			child->SetBounds(childRect);
		}
	}

	for (auto child : m_children)
		child.second->Update(timeDelta);
}

void GridLayout::Render(AppGraphics& g, float timeDelta)
{
	for (auto child : m_children)
		child.second->Render(g, timeDelta);
}

void GridLayout::InsertSlice(uint32 axis, uint32 sliceIndex, uint32 count)
{
	Axis& axisInfo = m_axes[axis];
	SetSliceCount(axis, axisInfo.count + count);
	for (uint32 i = axisInfo.count - 1; i >= sliceIndex + count; i--)
		MoveSlice(axis, i - count, i);
}

void GridLayout::RemoveSlice(uint32 axis, uint32 index, bool shiftUp)
{
	// Remove children from this slice
	Array<GridLoc> toRemove;
	for (auto it : m_children)
	{
		if (it.first[axis] == index)
			toRemove.push_back(it.first);
	}
	for (const GridLoc& location: toRemove)
		DoRemove(location);

	// Shift slices forward
	if (shiftUp)
	{
		Axis& axisInfo = m_axes[axis];
		for (uint32 i = index; i < axisInfo.count - 1; i++)
			MoveSlice(axis, i + 1, i);
		SetSliceCount(axis, axisInfo.count - 1);
	}
}

void GridLayout::MoveSlice(uint32 axis, uint32 from, uint32 to)
{
	if (from == to)
		return;

	// TODO: Move slice settings

	// Move children
	Array<GridLoc> toRemove;
	Array<GridLoc> toMove;
	for (auto it : m_children)
	{
		if (it.first[axis] == from)
			toMove.push_back(it.first);
		if (it.first[axis] == to)
			toRemove.push_back(it.first);
	}
	for (const GridLoc& location: toRemove)
		DoRemove(location);
	for (GridLoc location: toMove)
	{
		GUIObject* object = m_children[location];
		m_children.erase(location);
		location[axis] = to;
		m_children[location] = object;
	}
}

void GridLayout::SetSliceCount(uint32 axis, uint32 count)
{
	auto& axisInfo = m_axes[axis];
	if (count < axisInfo.count)
	{
		Array<GridLoc> toRemove;
		for (auto it : m_children)
		{
			if (it.first[axis] >= count)
				toRemove.push_back(it.first);
		}
		for (const GridLoc& location : toRemove)
			DoRemove(location);
	}
	axisInfo.count = count;
}

GUIObject* GridLayout::Get(const GridLoc& location)
{
	auto it = m_children.find(location);
	if (it != m_children.end())
		return it->second;
	return nullptr;
}

GUIObject* GridLayout::Remove(const GridLoc& location)
{
	auto it = m_children.find(location);
	if (it != m_children.end())
	{
		m_children.erase(location);
		it->second->SetParent(nullptr);
		return it->second;
	}
	return nullptr;
}

void GridLayout::DoRemove(const GridLoc& location)
{
	GUIObject* object = m_children[location];
	m_children.erase(location);
	object->SetParent(nullptr);
}
