
#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include <cmgGraphics/cmg_graphics.h>
#include <cmgInput/cmg_input.h>
#include <cmgApplication/cmg_application.h>
#include "gui/gui.h"
#include "RussianApp.h"
#include "external/RequestInterface.h"
#include "examples/ExampleDatabase.h"
#include "study/SchedulerBase.h"

/*
struct TestItem
{
	using sptr = cmg::shared_ptr<TestItem>;

	uint32 id;
	bool encountered = true;
	int32 repeatInterval = 8;
	Array<bool> showList;

	TestItem(uint32 id, bool encountered, int32 repeatInterval) :
		id(id),
		encountered(encountered),
		repeatInterval(repeatInterval)
	{
	}

};

class TestItemSet
{
public:
	Array<TestItem::sptr> GetCards()
	{
		return items;
	}

	Array<TestItem::sptr> items;
};

class TestScheduler : public SchedulerBase<TestItem::sptr, TestItemSet>
{
public:
	TestScheduler() {}

	virtual bool IsItemEncountered(TestItem::sptr item) override
	{
		return item->encountered;
	}

	virtual int32 GetRepeatInterval(TestItem::sptr item) override
	{
		return item->repeatInterval;
	}
};
*/


int main(int argc, char* argv[])
{
	using namespace ru;

	Random::SeedTime();
	cmg::core::console::EnableUnicode();
	Config::Initialize();

	/*
	TestScheduler scheduler;
	TestItemSet set;
	uint32 setSize = 60;
	for (int32 i = 0; i < setSize; i++)
		set.items.push_back(cmg::make_shared<TestItem>(i, true, 1 + (i * 2)));
	scheduler.Begin(&set);
	for (int32 i = 0; i < 1000; i++)
	{
		auto shownItem = scheduler.NextItem();
		scheduler.MarkItem(shownItem);
		for (auto item : set.items)
			item->showList.push_back(item == shownItem);
	}
	for (auto item : set.items)
	{
		String str = "";
		uint32 count = 0;
		for (uint32 i = 0; i < item->showList.size(); i++)
		{
			bool x = item->showList[i];
			if (x)
				count++;
			if (i >= 500 && str.length() < 100)
				str += x ? "X" : " ";
		}
		float percent = (float) count / (float) item->showList.size();
		uint32 actualRepeatInterval = item->showList.size() / count;

		CMG_LOG_DEBUG() <<
			std::setw(4) << item->id <<
			std::setw(6) << item->repeatInterval <<
			std::setw(6) << actualRepeatInterval <<
			std::setw(8) << (int) (percent * 100.0f + 0.5f) << "%" <<
			" : " << str;
	}
	system("pause");
	return 0;
	*/

	/*
	RequestInterface requests;
	wiki::Wiktionary wiki(requests);
	wiki::Term::sptr term = wiki.DownloadTerm(u"человек");
	wiki::Word::sptr word = term->GetWord(WordType::k_noun);
	wiki::Noun::sptr noun = std::dynamic_pointer_cast<wiki::Noun>(word);
	CMG_LOG_DEBUG() << "Gender  = " << EnumToString(noun->GetDeclension().GetGender());
	CMG_LOG_DEBUG() << "Animacy = " << EnumToString(noun->GetDeclension().GetAnimacy());
	system("pause");
	return 0;
	*/

	RussianStudyToolApp app;
	app.Initialize("Russian Study Tool", 1000, 800);
	app.Run();
	return 0;
}
