#include "StudyProficiencyBar.h"

StudyProficiencyBar::StudyProficiencyBar()
{
	m_layout.Add(&m_labelScore, 0.f);
	m_layout.Add(&m_bar, 1.0f);
	m_layout.Add(&m_labelCount, 0.2f);
	m_labelScore.SetAlign(TextAlign::MIDDLE_RIGHT);
	m_labelCount.SetAlign(TextAlign::MIDDLE_LEFT);
	m_layout.SetMargins(0.0f);
	SetLayout(&m_layout);
}

void StudyProficiencyBar::SetMetrics(const StudySetMetrics & metrics)
{
	m_bar.SetMetrics(metrics);
}

void StudyProficiencyBar::OnUpdate(float timeDelta)
{
	const StudySetMetrics& metrics = m_bar.GetMetrics();
	m_labelCount.SetText(std::to_string(metrics.GetTotalCount()));
	std::stringstream ss;
	float score = metrics.GetHistoryScore();
	ss << (int) (score * 100.0f) << "%";
	m_labelScore.SetText(ss.str());
}

const StudySetMetrics & ProficiencyBarBoxWidget::GetMetrics() const
{
	return m_metrics;
}

void ProficiencyBarBoxWidget::SetMetrics(const StudySetMetrics & metrics)
{
	m_metrics = metrics;
}

void ProficiencyBarBoxWidget::OnRender(AppGraphics& g, float timeDelta)
{
	uint32 totalCount = m_metrics.GetTotalCount();
	uint32 numBins = m_metrics.GetNumScoreBins();
	Rect2f bounds = GetBounds();

	// Background
	g.FillRect(bounds, Config::k_colorGray);

	// Histogram of history score values
	float offset = 0.0f;
	for (int32 bin = numBins - 1; bin >= 0; bin--)
	{
		uint32 binCount = m_metrics.GetCountForScoreBin((uint32) bin);
		if (binCount == 0)
			continue;
		float percent = (float) binCount / (float) totalCount;
		float levelWidth = GetBounds().GetWidth() * percent;
		float score = (float) bin / ((float) numBins - 1);
		Color binColor = Config::GetHistoryScoreColor(score);
		Rect2f levelRect = bounds;
		levelRect.position.x += offset;
		levelRect.size.x = levelWidth;
		g.FillRect(levelRect, binColor);
		offset += levelWidth;
	}
}
