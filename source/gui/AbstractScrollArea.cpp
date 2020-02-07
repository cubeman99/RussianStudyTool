#include "AbstractScrollArea.h"

AbstractScrollArea::AbstractScrollArea() :
	m_scrollBars{0, 1}
{
	m_gridLayout.Add(&m_areaLayout, 0, 0);
	m_gridLayout.Add(&m_scrollBars[0], 1, 0);
	m_gridLayout.Add(&m_scrollBars[1], 0, 1);
	m_gridLayout.Add(&m_cornerWidget, 1, 1);
	m_gridLayout.SetRowStretch(0, 1.0f);
	m_gridLayout.SetColumnStretch(0, 1.0f);
	m_gridLayout.SetRowStretch(1, 0.0f);
	m_gridLayout.SetColumnStretch(1, 0.0f);
	m_gridLayout.SetSpacing(0.0f);
	m_gridLayout.SetMargins(0.0f);
	SetLayout(&m_gridLayout);

	m_cornerWidget.SetBackgroundColor(GUIConfig::color_button_background);

	AddKeyShortcut("PageDown", [this]() {
		m_scrollBars[1].PageDown();
		return true;
	});
	AddKeyShortcut("PageUp", [this]() {
		m_scrollBars[1].PageUp();
			return true;
	});
}

Widget* AbstractScrollArea::GetWidget()
{
	return m_areaLayout.GetWidget();
}

void AbstractScrollArea::SetWidget(Widget* widget)
{
	m_areaLayout.SetWidget(widget);
}

void AbstractScrollArea::OnUpdate(float timeDelta)
{
	Rect2f fullRect = GetBounds();

	float viewportHeight = fullRect.size.y;
	float areaHeight = viewportHeight;
	Widget* widget = m_areaLayout.GetWidget();
	if (widget)
		areaHeight = widget->GetMinSize().y + (viewportHeight * 0.8f);
	m_scrollBars[1].SetMinimumValue(0);
	m_scrollBars[1].SetMaximumValue(
		Math::Max(0.0f, areaHeight - viewportHeight));
	m_scrollBars[1].SetPageStep(viewportHeight);

	Vector2f offset(
		m_scrollBars[0].GetValue(),
		m_scrollBars[1].GetValue());
	m_areaLayout.SetOffset(offset);
}

SubRegionLayout::SubRegionLayout() :
	m_renderTarget(1920, 1080),
	m_backgroundColor(GUIConfig::color_background)
{
	m_renderTarget.CreateColorAttachment(0);
}

Widget* SubRegionLayout::GetWidget()
{
	return m_widget;
}

void SubRegionLayout::SetWidget(Widget* widget)
{
	if (m_widget)
		m_widget->SetParent(nullptr);
	m_widget = widget;
	if (m_widget)
		m_widget->SetParent(this);
}

void SubRegionLayout::SetOffset(const Vector2f& offset)
{
	m_offset = offset;
}

uint32 SubRegionLayout::GetNumChildren() const
{
	return (m_widget ? 1 : 0);
}

GUIObject* SubRegionLayout::GetChild(uint32 index)
{
	return m_widget;
}

const Vector2f& SubRegionLayout::GetOffset() const
{
	return m_offset;
}

void SubRegionLayout::CalcSizes()
{
	m_minSize = Vector2f(DEFAULT_MIN_SIZE);
	m_maxSize = Vector2f(DEFAULT_MAX_SIZE);
}

void SubRegionLayout::Update(float timeDelta)
{
	if (m_widget)
	{
		Vector2f minSize = m_widget->GetMinSize();
		Rect2f widgetRect = GetBounds();
		widgetRect.position -= m_offset;
		widgetRect.size.y = minSize.y;
		m_widget->SetBounds(widgetRect);
		m_widget->Update(timeDelta);
	}
}

void SubRegionLayout::Render(AppGraphics& g, float timeDelta)
{
	if (m_widget)
	{
		RenderTarget* prevRenderTarget = g.GetRenderTarget();
		Matrix4f prevTransform = g.GetTransformation();
		Rect2f dest = GetBounds();
		{
			g.SetRenderTarget(&m_renderTarget);
			g.SetTransformation(Matrix4f::CreateTranslation(
				Vector3f(-dest.position, 0.0f)));
			g.Clear(m_backgroundColor);
			m_widget->Render(g, timeDelta);
			g.SetTransformation(prevTransform);
			g.SetRenderTarget(prevRenderTarget);
		}

		Rect2f source = dest;
		source.position = Vector2f::ZERO;
		source.position.y = (float) m_renderTarget.GetHeight();
		source.size.y = -source.size.y;
		g.DrawTexture(m_renderTarget.GetColorAttachment(0).get(), source, dest, Color::WHITE);
	}
}
