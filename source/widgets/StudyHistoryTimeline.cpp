#include "StudyHistoryTimeline.h"

StudyHistoryTimeline::StudyHistoryTimeline()
{
}

const CardStudyData& StudyHistoryTimeline::GetCardStudyData() const
{
	return m_studyData;
}

void StudyHistoryTimeline::SetCardStudyData(const CardStudyData& studyData,
	AppTimestamp currentTimestamp, AppTimestamp durationSeconds)
{
	m_studyData = studyData;
	m_durationSeconds = durationSeconds;
	m_currentTimestamp = currentTimestamp;
}

void StudyHistoryTimeline::OnRender(AppGraphics& g, float timeDelta)
{
	Rect2f bounds = GetBounds();
	Vector2f center = bounds.GetCenter();
	Vector2f startPoint(bounds.GetLeft(), center.y);
	Vector2f endPoint(bounds.GetRight(), center.y);
	Vector2f tickVector(0.0f, bounds.GetHeight() * 0.5f);

	// Draw tick markers for each history entry
	uint32 count = m_studyData.GetHistorySize();
	if (count > 0)
	{
		for (uint32 i = 0; i < count; i++)
		{
			bool knewIt = m_studyData.GetHistoryKnewIt(i);
			AppTimestamp elapsedSeconds = m_currentTimestamp -
				m_studyData.GetHistoryTimestamp(i);
			if (elapsedSeconds > m_durationSeconds)
				continue;

			// Draw a tick marker
			Vector2f tickStartPoint = Vector2f::Lerp(endPoint, startPoint,
				(float) (elapsedSeconds / m_durationSeconds));
			Vector2f tickEndPoint = tickStartPoint +
				(knewIt ? -tickVector : tickVector);
			// Color tickColor = (knewIt ?
			// 	Config::GetProficiencyLevelColor(ProficiencyLevel::k_learned) :
			// 	Config::GetProficiencyLevelColor(ProficiencyLevel::k_hard));
			Color tickColor = GUIConfig::color_text;
			g.DrawLine(tickStartPoint, tickEndPoint, tickColor, 2.0f);
		}
	}

	// Draw the timeline line
	Color lineColor = GUIConfig::color_text;
	lineColor.a = 128;
	g.DrawLine(startPoint, endPoint, lineColor, 2.0f);
}

StudyHistoryWidget::StudyHistoryWidget()
{
	m_layout.Add(&m_labelAge, 0.0f);
	m_layout.Add(&m_timeline, 1.0f);
	m_layout.Add(&m_labelScore, 0.0f);
	m_layout.SetSpacing(12.0f);
	SetLayout(&m_layout);
}

void StudyHistoryWidget::SetCardStudyData(const CardStudyData& studyData)
{
	// Calculate total duration to display
	uint32 count = studyData.GetHistorySize();
	AppTimestamp currentTimestamp = GetAppTimestamp();
	AppTimestamp durationSeconds = 10.0;
	if (count > 0)
	{
		AppTimestamp oldestTimestamp =
			studyData.GetHistoryTimestamp(count - 1);
		durationSeconds = currentTimestamp - oldestTimestamp;
		durationSeconds *= 1.1;
		durationSeconds = Math::Max(durationSeconds, 10.0);
	}
	m_timeline.SetCardStudyData(studyData, currentTimestamp, durationSeconds);

	AppTimestamp minutes = durationSeconds / 60.0;
	AppTimestamp hours = minutes / 60.0;
	AppTimestamp days = hours / 24.0;
	AppTimestamp amount;
	String units;
	if (minutes < 1.0)
	{
		amount = durationSeconds;
		units = "s";
	}
	else if (hours < 1.0)
	{
		amount = minutes;
		units = "m";
	}
	else if (days < 1.0)
	{
		amount = hours;
		units = "h";
	}
	else
	{
		amount = days;
		units = "d";
	}

	String text = std::to_string(static_cast<int>(amount)) + units;
	m_labelAge.SetText(text);

	std::stringstream ss;
	ss.setf(std::ios::fixed);
	ss << std::setprecision(2) << studyData.GetHistoryScore();
	m_labelScore.SetText(ss.str());
}
