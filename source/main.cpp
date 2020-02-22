﻿
#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>
#include <cmgGraphics/cmg_graphics.h>
#include <cmgInput/cmg_input.h>
#include <cmgApplication/cmg_application.h>
#include "gui/gui.h"
#include "RussianApp.h"
#include "external/RequestInterface.h"
#include "examples/ExampleDatabase.h"

class TestClass
{
public:

	void Callback(int32 x)
	{
		CMG_LOG_INFO() << "x = " << x;
	}
};

int main(int argc, char* argv[])
{
	using namespace ru;

	Random::SeedTime();
	cmg::core::console::EnableUnicode();
	Config::Initialize();

	/*
	ExampleDatabase m_exampleDatabase;
	Story::sptr story;
	m_exampleDatabase.LoadExamples(PathU16(u"C:/workspace/python/russian-study-tool/data/examples"));
	//m_exampleDatabase.LoadStory(PathU16(u"C:/workspace/python/russian-study-tool/data/examples/stories/Маленький секрет.story"), story);

	//Array<unistr> words = { u"нужнО", u"Наказать" };
	Array<unistr> words = { u"по", u"крайней" };
	auto matches = m_exampleDatabase.GetExampleSentences(words);
	for (auto match : matches)
	{
		CMG_LOG_DEBUG() << match.sentence;
	}
	//Array<std::pair<uint32, uint32>> results;
	//uint32 count = m_exampleDatabase.GetExamplesInSentence(
	//	u"— Ты была плохой девочкой, Луна. Тебя за это ну'жно наказа'ть. – Сказала принцесса. НУЖНО НАКАЗА'ТЬ!!!", words, results);
	//std::cout << count << std::endl;

	system("pause");
	return 0;
	*/

	/*
	RequestInterface requests;
	wiki::Wiktionary wiktionary(requests);
	unistr result;
	auto term = wiktionary.DownloadTerm(u"стол");

	system("pause");
	return 0;*/
	/*
	
	TestClass test;
	EventSignal<int32> signal;

	signal.Connect(&test, &TestClass::Callback);

	signal.Emit(42);
	signal.Emit(37);
	signal.Clear();
	signal.Emit(37);
	
	std::cout << EnumToString(CardTags::k_inanimate) << std::endl;
	std::cout << EnumToString(CardTags::k_past) << std::endl;
	std::cout << EnumToString(CardTags::k_perfective) << std::endl;
	std::cout << EnumToString(StringToEnum<CardTags>("inanimate")) << std::endl;
	std::cout << EnumToString(StringToEnum<CardTags>("past")) << std::endl;
	std::cout << EnumToString(StringToEnum<CardTags>("perfective")) << std::endl;
	CardTags x;
	std::cout << TryStringToEnum<CardTags>("perfective", x) << std::endl;
	std::cout << TryStringToEnum<CardTags>("perasdfective", x) << std::endl;
	*/

	/*
	using TagFlags = EnumFlags<CardTags, (uint32_t) CardTags::k_count, uint8>;

	TagFlags a = CardTags::k_accusative;
	TagFlags b = CardTags::k_perfective;
	TagFlags c = a | b;

	std::cout << a[CardTags::k_accusative] << std::endl;
	std::cout << a[CardTags::k_past] << std::endl;
	std::cout << a[CardTags::k_perfective] << std::endl;
	a |= CardTags::k_past;
	std::cout << a[CardTags::k_accusative] << std::endl;
	std::cout << a[CardTags::k_past] << std::endl;
	std::cout << a[CardTags::k_perfective] << std::endl;

	std::cout << a.GetValue(0) << ", " << a.GetValue(1) << std::endl;
	std::cout << a.GetCount() << std::endl;
	std::cout << a.Size() << std::endl;
	std::cout << b.GetValue() << std::endl;
	std::cout << c.GetValue() << std::endl;
	std::cout << c[Case::k_accusative] << std::endl;
	std::cout << c[Case::k_genitive] << std::endl;
	std::cout << c[Case::k_nominative] << std::endl;
	std::cout << c[Case::k_dative] << std::endl;
	c.Set(Case::dative, true);
	std::cout << c[Case::dative] << std::endl;
	std::cout << c[Case::nominative] << std::endl;
	c |= Case::nominative;
	std::cout << c[Case::nominative] << std::endl;*/

	//AccentedText text(u"Hello Приве'т, don't stop now!! Не`т спаси´бо");
	//std::cout << text << std::endl;
	//text += u"asdasd";
	//std::cout << AccentedText(u"Hello Приве'т, don't stop now!! Не`т спаси´бо") << std::endl;
	//std::cout << "User-preferred locale setting is " << std::locale().name().c_str() << "!" << std::endl;
	//AccentedText text(u"Hello Приве'т, don't stop now!! Не`т спаси´бо");
	//std::cout << text << std::endl;
	//std::cout << text.GetString() << std::endl;
	//std::cout << ru::ToLower(text.GetString()) << std::endl;
	//std::cout << ru::ToUpper(text.GetString()) << std::endl;
	//system("pause");
	//return 0;
	

	RussianStudyToolApp app;
	app.Initialize("Russian Study Tool", 1000, 800);
	app.Run();
	return 0;
}
