#pragma once

#include "gui/Layout.h"


class GridLayout : public Layout
{
public:
	GridLayout();

	void Clear();

	uint32 GetRowCount() const;
	uint32 GetColumnCount() const;
	GUIObject* Get(uint32 row, uint32 column);

	void SetSpacing(float spacing);
	void SetMargins(float margins);
	void SetRowCount(uint32 rowCount);
	void SetColumnCount(uint32 columnCount);
	void SetRowStretch(uint32 row, float stretch);
	void SetColumnStretch(uint32 column, float stretch);
	void Add(GUIObject* item, uint32 row, uint32 column);
	GUIObject* Remove(uint32 row, uint32 column);
	void RemoveRow(uint32 row, bool shiftUp);
	void MoveRow(uint32 from, uint32 to);

	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes();
	virtual void Update(float timeDelta);
	virtual void Render(AppGraphics& g, float timeDelta);

private:
	void RemoveSlice(uint32 axis, uint32 row, bool shiftUp);
	void MoveSlice(uint32 axis, uint32 from, uint32 to);
	void SetSliceCount(uint32 axis, uint32 count);

	struct GridLoc
	{
		GridLoc() {}
		GridLoc(uint32 x, uint32 y) :
			x(x), y(y)
		{}

		bool operator <(const GridLoc& other) const
		{
			return (x * 30000) + y < (other.x * 30000) + other.y;
			//return (y * 30000) + x < (other.y * 30000) + other.x;
		}

		uint32& operator [](uint32 axis)
		{
			return (&x)[axis];
		}

		const uint32& operator [](uint32 axis) const
		{
			return (&x)[axis];
		}

		uint32 x;
		uint32 y;
	};

	struct AxisSlice
	{
		float minSize;
		float maxSize;
		bool isMaxed;
		bool isMinned;
		float offset;
		float size;
		float stretch;
	};

	struct Axis
	{
		enum
		{
			k_column = 0,
			k_row = 1,
		};

		float GetStretchFactor(uint32 slice)
		{
			auto it = stretchFactors.find(slice);
			if (it != stretchFactors.end())
				return it->second;
			return 0.0f;
		}

		Array<AxisSlice> slices;
		Map<uint32, float> stretchFactors;
		uint32 count = 0;
		float spacing = 4.0f;
		float marginBegin = 8.0f;
		float marginEnd = 8.0f;
	};

	GUIObject* Get(const GridLoc& location);
	GUIObject* Remove(const GridLoc& location);
	void DoRemove(const GridLoc& location);

	Axis m_axes[2];
	Map<GridLoc, GUIObject*> m_children;
};

