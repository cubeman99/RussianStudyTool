#pragma once

#include "gui/ScrollBar.h"
#include "gui/GridLayout.h"


class SubRegionLayout : public Layout
{
public:
	SubRegionLayout();

	Widget* GetWidget();
	const Vector2f& GetOffset() const;

	void SetWidget(Widget* widget);
	void SetOffset(const Vector2f& offset);

	virtual uint32 GetNumChildren() const override;
	virtual GUIObject* GetChild(uint32 index) override;
	virtual void CalcSizes();
	virtual void Update(float timeDelta);
	virtual void Render(AppGraphics& g, float timeDelta);

private:
	Widget* m_widget = nullptr;
	Vector2f m_offset = Vector2f::ZERO;
	RenderTarget m_renderTarget;
	Color m_backgroundColor;
};



class AbstractScrollArea : public Widget
{
public:
	AbstractScrollArea();

	Widget* GetWidget();
	void SetWidget(Widget* widget);

	virtual void OnUpdate(float timeDelta) override;

private:
	ScrollBar m_scrollBars[2];
	SubRegionLayout m_areaLayout;
	Widget m_cornerWidget;
	GridLayout m_gridLayout;
};

