#pragma once

#include "gui/ScrollBar.h"
#include "gui/GridLayout.h"


class SubRegionLayout : public Layout
{
public:
	SubRegionLayout(bool horizontalStretch, bool verticalStretch);

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
	bool m_axisStretch[2] = { true, true };
};



class AbstractScrollArea : public Widget
{
public:
	AbstractScrollArea(bool horizontalScroll = false, bool verticalScroll = true);

	Widget* GetWidget();
	void SetWidget(Widget* widget);

	void OnMoveCursor();

	virtual void OnInitialize() override;
	virtual void OnUpdate(float timeDelta) override;

private:
	ScrollBar m_scrollBars[2];
	SubRegionLayout m_areaLayout;
	Widget m_cornerWidget;
	GridLayout m_gridLayout;
};

