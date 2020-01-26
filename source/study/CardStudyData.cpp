#include "CardStudyData.h"


float CardStudyData::CalcHistoryScore(const Array<bool>& history)
{
	if (history.empty())
		return 0.0f;
	float score = 1.0f;
	for (uint32 i = 0; i < history.size(); i++)
	{
		if (!history[i])
			score -= 0.5f / (i + 2);
	}
	uint32 minLength = 6;
	if (history.size() < minLength)
		score /= (minLength - history.size() + 1.0f);
	return score;
}
