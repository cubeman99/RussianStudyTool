
#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include <cmgGraphics/cmg_graphics.h>
#include <cmgInput/cmg_input.h>
#include <cmgApplication/cmg_application.h>
#include "gui/gui.h"
#include "RussianApp.h"
#include "external/RequestInterface.h"
#include "examples/ExampleDatabase.h"


int main(int argc, char* argv[])
{
	using namespace ru;

	Random::SeedTime();
	cmg::core::console::EnableUnicode();
	Config::Initialize();

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
