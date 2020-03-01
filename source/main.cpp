
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

	RussianStudyToolApp app;
	app.Initialize("Russian Study Tool", 1000, 800);
	app.Run();
	return 0;
}
