#include "StudyProficiencyBar.h"

StudyProficiencyBar::StudyProficiencyBar()
{
	//m_layout.Add(&m_labelScore, 0.15f);
	//m_layout.Add(&m_bar, 1.0f);
	//m_layout.Add(&m_labelCount, 0.15f);
	m_layout.Add(&m_labelScore, 1.0f);
	m_layout.Add(&m_bar, 1.0f);
	m_layout.Add(&m_labelCount, 1.0f);
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

void ProficiencyBarBoxWidget::OnRender(AppGraphics & g, float timeDelta)
{
	float offset = 0.0f;
	uint32 totalCount = m_metrics.GetTotalCount();
	Rect2f bounds = GetBounds();

	g.FillRect(bounds, Config::GetProficiencyLevelColor(ProficiencyLevel::k_new));

	if (totalCount > 0)
	{
		for (int32 i = (int32) ProficiencyLevel::k_count - 1; i >= 0; i--)
		{
			ProficiencyLevel level = (ProficiencyLevel) i;
			uint32 count = m_metrics.GetCount(level);
			if (count > 0)
			{
				float levelWidth = GetBounds().GetWidth() *
					((float) count / (float) totalCount);
				Rect2f levelRect = bounds;
				levelRect.position.x += offset;
				levelRect.size.x = levelWidth;
				g.FillRect(levelRect, Config::GetProficiencyLevelColor(level));
				offset += levelWidth;
			}
		}
	}
}
