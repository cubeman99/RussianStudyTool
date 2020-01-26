#pragma once

#include "study/StudySetMetrics.h"
#include "widgets/AppWidget.h"


class ProficiencyBarBoxWidget : public AppWidget
{
public:
	ProficiencyBarBoxWidget() {}

	const StudySetMetrics& GetMetrics() const;
	void SetMetrics(const StudySetMetrics& metrics);
	void OnRender(AppGraphics& g, float timeDelta) override;

private:
	StudySetMetrics m_metrics;
};


class StudyProficiencyBar : public AppWidget
{
public:
	StudyProficiencyBar();

	void SetMetrics(const StudySetMetrics& metrics);

	void OnUpdate(float timeDelta) override;

private:
	HBoxLayout m_layout;
	Label m_labelScore;
	Label m_labelCount;
	ProficiencyBarBoxWidget m_bar;
};
