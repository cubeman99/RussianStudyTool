#pragma once

#include "widgets/AppWidget.h"
#include "examples/ExampleDatabase.h"
#include "cards/Card.h"


class ExampleTextWidget : public Label
{
public:
	ExampleTextWidget(const SentenceMatch& match);

	virtual void OnRender(AppGraphics& g, float timeDelta) override;

private:
	Array<std::pair<uint32, uint32>> m_matchLocations;
};
