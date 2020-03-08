#pragma once

#include "study/CardStudyData.h"
#include "widgets/AppWidget.h"


class StudyHistoryTimeline : public AppWidget
{
public:
	StudyHistoryTimeline();

	const CardStudyData& GetCardStudyData() const;
	void SetCardStudyData(const CardStudyData& studyData,
		AppTimestamp currentTimestamp, AppTimestamp durationSeconds);

	void OnRender(AppGraphics& g, float timeDelta) override;

private:
	CardStudyData m_studyData;
	AppTimestamp m_durationSeconds = 10.0;
	AppTimestamp m_currentTimestamp = 0.0;
};


class StudyHistoryWidget : public AppWidget
{
public:
	StudyHistoryWidget();

	void SetCardStudyData(const CardStudyData& studyData);

private:
	StudyHistoryTimeline m_timeline;
	Label m_labelAge;
	Label m_labelScore;
	HBoxLayout m_layout;
};
