#include "Parser.h"
#include "external/XML.h"


namespace wiki
{

struct PageSection
{
	PageSection(const String& name, uint32 depth, PageSection* parent) :
		name(name),
		depth(depth),
		parent(parent)
	{}

	void Print()
	{
		for (uint32 i = 0; i < depth; i++)
			std::cout << "  ";
		std::cout << name << std::endl;
		for (auto child : children)
		{
			child->Print();
		}
	}

	String Text()
	{
		String text;
		for (auto node : nodes)
			text += node.Text();
		return text;
	}

	PageSection* GetSection(const String& section)
	{
		for (auto child : children)
		{
			if (child->name == section)
				return child;
		}
		return nullptr;
	}

	void FindAll(const String& expression, Array<xml::Node>& outNodes)
	{
		for (auto node : nodes)
			node.FindAll(expression, outNodes);
	}

	xml::Node FindFirst(const String& expression)
	{
		xml::Node node;
		for (auto node : nodes)
		{
			if (node.GetName() == expression)
				return node;
			node = node.FindFirst(expression);
			if (node)
				return node;
		}
		return node;
	}

	PageSection* parent;
	uint32 depth;
	String name;
	Array<xml::Node> nodes;
	Array<PageSection*> children;
};


namespace
{

Word::sptr ParseWord(PageSection* wordSection,
	WordType wordType, const AccentedText& text);
void ParseTermList(PageSection* section,
	Set<AccentedText>& outTerms);
Definition ParseDefinition(xml::Node node);
ru::NounDeclension ParseNounDeclension(PageSection* section);
ru::AdjectiveDeclension ParseAdjectiveDeclension(PageSection* section);
ru::VerbConjugation ParseVerbConjugation(PageSection* section);


Word::sptr ParseWord(PageSection* wordSection,
	WordType wordType, const AccentedText& text)
{
	Word::sptr word;
	if (wordType == WordType::k_noun)
		word = cmg::make_shared<Noun>();
	else if (wordType == WordType::k_adjective)
		word = cmg::make_shared<Adjective>();
	else if (wordType == WordType::k_verb)
		word = cmg::make_shared<Verb>();
	else
		word = cmg::make_shared<Word>(wordType);

	xml::Node infoNode = wordSection->FindFirst("p");
	word->SetText(text);
	if (infoNode)
	{
		xml::Node headWord = infoNode.FindFirst(".//*[@lang='ru']");
		word->SetText(headWord.Text());
	}

	// Parse list of definitions
	xml::Node defList = wordSection->FindFirst("ol");
	auto items = defList.FindAll(".//li");
	for (uint32 index = 0; index < items.size(); index++)
	{
		Definition definition = ParseDefinition(items[index]);
		word->GetDefinitions().push_back(definition);
	}

	// Parse etymology information
	PageSection* etymology = wordSection->GetSection("etymology");
	if (etymology)
		word->SetEtymology(cmg::string::Trim(etymology->Text()));

	// Parse term lists (related/derived words/synonymns)
	PageSection* termSection = wordSection->GetSection("related terms");
	if (termSection)
		ParseTermList(termSection, word->GetRelatedTerms());
	termSection = wordSection->GetSection("derived terms");
	if (termSection)
		ParseTermList(termSection, word->GetDerivedTerms());
	termSection = wordSection->GetSection("synonyms");
	if (termSection)
		ParseTermList(termSection, word->GetSynonyms());
	termSection = wordSection->GetSection("antonyms");
	if (termSection)
		ParseTermList(termSection, word->GetAntonyms());

	// Parse word-type-specific data like declension tables
	if (wordType == WordType::k_noun)
	{
		Noun::sptr noun = std::dynamic_pointer_cast<Noun>(word);
		auto declensionSection = wordSection->GetSection("declension");
		if (declensionSection)
		{
			ru::NounDeclension nounDeclension =
				ParseNounDeclension(declensionSection);
			noun->SetDeclension(nounDeclension);
		}

		if (infoNode)
		{
			// Parse gender
			if (infoNode.FindFirst(".//*[@title='masculine gender']"))
				noun->GetDeclension().SetGender(Gender::k_masculine);
			else if (infoNode.FindFirst(".//*[@title='feminine gender']"))
				noun->GetDeclension().SetGender(Gender::k_feminine);
			else if (infoNode.FindFirst(".//*[@title='neuter gender']"))
				noun->GetDeclension().SetGender(Gender::k_neuter);

			// Parse animacy
			if (infoNode.FindFirst(".//*[@title='inanimate']"))
				noun->GetDeclension().SetAnimacy(Animacy::k_inanimate);
			else if (infoNode.FindFirst(".//*[@title='animate']"))
				noun->GetDeclension().SetAnimacy(Animacy::k_animate);
		}
	}
	else if (wordType == WordType::k_adjective)
	{
		Adjective::sptr adjective =
			std::dynamic_pointer_cast<Adjective>(word);
		auto declensionSection = wordSection->GetSection("declension");
		if (declensionSection)
		{
			ru::AdjectiveDeclension declension =
				ParseAdjectiveDeclension(declensionSection);
			adjective->SetDeclension(declension);
		}
	}
	else if (wordType == WordType::k_verb)
	{
		Verb::sptr verb = std::dynamic_pointer_cast<Verb>(word);

		// Parse verb conjugation
		auto conjugationSection = wordSection->GetSection("conjugation");
		if (conjugationSection)
		{
			ru::VerbConjugation conjugation =
				ParseVerbConjugation(conjugationSection);
			verb->SetConjugation(conjugation);
		}

		// Parse verb counterparts
		if (infoNode)
		{
			bool foundCounterparts = false;
			for (auto infoSubNode = infoNode.Children();
				infoSubNode; infoSubNode = infoSubNode.Next())
			{
				String infoSubNodeText = infoSubNode.Text();
				if (infoSubNodeText == "imperfective" ||
					infoSubNodeText == "perfective")
					foundCounterparts = true;
				if (foundCounterparts && infoSubNode.HasAttribute("lang", "ru"))
					verb->GetCounterparts().insert(AccentedText(infoSubNodeText));
			}
		}
	}

	return word;
}

ru::NounDeclension ParseNounDeclension(PageSection* section)
{
	ru::NounDeclension declension;
	auto navFrame = section->FindFirst(".//*[@class='NavFrame']");
	if (!navFrame)
		return declension;
	String label;

	// Parse each row in the table
	for (auto row : navFrame.FindAll(".//tr"))
	{
		// Get the list of columns in this row
		auto columns = row.FindAll(".//td");
		if (columns.empty())
			continue;

		// Parse the case from the row header
		auto header = row.FindFirst(".//th");
		if (header)
		{
			label = cmg::string::Trim(header.Text());
			if (label.find("accusative") != std::string::npos)
				label = "accusative";
		}
		Case nounCase;
		if (!TryStringToEnum(label, nounCase))
			continue;
		// TODO: locative case

		// Parse the declension forms from the columns
		uint32 columnIndex = 0;
		for (auto form : columns)
		{
			auto nodeForm = form.FindFirst(".//*[@lang='ru']");
			if (nodeForm)
			{
				AccentedText formText(cmg::string::Trim(nodeForm.Text()));
				if (columnIndex == 0)
					declension.SetDeclension(nounCase, Plurality::k_singular, formText);
				else if (columnIndex == 1)
					declension.SetDeclension(nounCase, Plurality::k_plural, formText);
				columnIndex++;
			}
		}
	}

	return declension;
}

ru::AdjectiveDeclension ParseAdjectiveDeclension(PageSection* section)
{
	ru::AdjectiveDeclension declension;
	auto navFrame = section->FindFirst(".//*[@class='NavFrame']");
	if (!navFrame)
		return declension;
	String label;

	Gender columnGenders[4] = {
		Gender::k_masculine, Gender::k_neuter,
		Gender::k_feminine, Gender::k_plural};

	// Parse each row in the table
	for (auto row : navFrame.FindAll(".//tr"))
	{
		// Get the list of columns in this row
		auto columns = row.FindAll(".//td");
		if (columns.empty())
			continue;

		// Parse the case from the row header
		auto header = row.FindFirst(".//th");
		Case adjectiveCase = (Case) -1;
		bool shortForm = false;
		if (header)
		{
			label = cmg::string::Trim(header.Text());
			if (label.find("inanimate") != std::string::npos)
				continue;
			else if (label.find("animate") != std::string::npos)
				adjectiveCase = Case::k_accusative;
			else if (label.find("short") != std::string::npos)
				shortForm = true;
			else if (!TryStringToEnum(label, adjectiveCase))
				continue;
		}

		// Parse the declension forms from the columns
		uint32 columnIndex = 0;
		for (xml::Node& columnNode : columns)
		{
			auto nodeForm = columnNode.FindFirst(".//*[@lang='ru']");
			uint32 columnSpan = 1;
			String columnSpanStr;
			if (columnNode.TryGetAttribute("colspan", columnSpanStr))
				columnSpan = std::stoi(columnSpanStr);

			if (nodeForm)
			{
				AccentedText formText(cmg::string::Trim(nodeForm.Text()));
				for (uint32 i = 0; i < columnSpan; i++)
				{
					Gender gender = columnGenders[columnIndex];
					if (shortForm)
						declension.SetShortForm(gender, formText);
					else
						declension.SetDeclension(adjectiveCase, gender, formText);
					columnIndex++;
					if (columnIndex >= 4)
						break;
				}
			}
		}
	}

	return declension;
}

ru::VerbConjugation ParseVerbConjugation(PageSection* section)
{
	ru::VerbConjugation conjugation;
	auto navFrame = section->FindFirst(".//*[@class='NavContent']");
	if (!navFrame)
		return conjugation;
	String label;
	ru::VerbConjugationClass verbClass;


	Gender columnGenders[4] = {
		Gender::k_masculine, Gender::k_neuter,
		Gender::k_feminine, Gender::k_plural};
	uint32 nonPastColumn = 0;

	auto navHead = section->FindFirst(".//*[@class='NavHead']");
	if (navHead)
	{
		unistr conjugationClassText = ConvertFromUTF8(navHead.Text());
		if (conjugationClassText.find(u"reflexive") != String::npos)
			conjugation.SetTransitivity(Transitivity::k_reflexive);
		else if (conjugationClassText.find(u"intransitive") != String::npos)
			conjugation.SetTransitivity(Transitivity::k_intransitive);
		else if (conjugationClassText.find(u"transitive") != String::npos)
			conjugation.SetTransitivity(Transitivity::k_transitive);
		conjugation.SetImpersonal(conjugationClassText.find(u"impersonal") != String::npos);
		bool found = false;
		for (verbClass.classNumber = 0;
			verbClass.classNumber <= 16 && !found; verbClass.classNumber++)
		{
			for (VerbAccentPattern accentPattern : EnumValues<VerbAccentPattern>())
			{
				verbClass.accentPattern = accentPattern;
				for (verbClass.variantIndex = 0;
					verbClass.variantIndex < 2 && !found; verbClass.variantIndex++)
				{
					found = (conjugationClassText.find(
						verbClass.ToString()) != String::npos);
					if (found)
						conjugation.SetVerbClass(verbClass);
				}
			}
		}
	}

	// Parse each row in the table
	for (auto row : navFrame.FindAll(".//tr"))
	{
		// Get the list of columns in this row
		auto columnNodes = row.FindAll(".//td");

		// Parse the row header label
		xml::Node headerNode = row.FindFirst(".//th");
		Case adjectiveCase = (Case) -1;
		bool shortForm = false;
		if (!headerNode)
			continue;
		String headerText = cmg::string::Trim(headerNode.Text());
		if (headerText.length() > 0)
			label = headerText;

		// If row is just a header, try to parse the aspect
		if (columnNodes.empty())
		{
			if (label == "imperfective aspect")
			{
				conjugation.SetAspect(Aspect::k_imperfective);
				nonPastColumn = 0;
			}
			else if (label == "perfective aspect")
			{
				conjugation.SetAspect(Aspect::k_perfective);
				nonPastColumn = 1;
			}
			continue;
		}

		// Get the text for all forms in the columns
		uint32 columnIndex = 0;
		Array<AccentedText> forms;
		for (xml::Node& columnNode : columnNodes)
		{
			auto nodeForm = columnNode.FindFirst(".//*[@lang='ru']");
			if (nodeForm)
				forms.push_back(AccentedText(cmg::string::Trim(nodeForm.Text())));
			else
				forms.push_back(AccentedText());
		}
		while (forms.size() < 2)
			forms.push_back(AccentedText());

		// Put the forms into the conjugation based on the header
		if (label == "infinitive")
			conjugation.SetInfinitive(forms[0]);
		else if (label.find("1st singular") != std::string::npos)
			conjugation.SetNonPast(Person::k_first, Plurality::k_singular, forms[nonPastColumn]);
		else if (label.find("2nd singular") != std::string::npos)
			conjugation.SetNonPast(Person::k_second, Plurality::k_singular, forms[nonPastColumn]);
		else if (label.find("3rd singular") != std::string::npos)
			conjugation.SetNonPast(Person::k_third, Plurality::k_singular, forms[nonPastColumn]);
		else if (label.find("1st plural") != std::string::npos)
			conjugation.SetNonPast(Person::k_first, Plurality::k_plural, forms[nonPastColumn]);
		else if (label.find("2nd plural") != std::string::npos)
			conjugation.SetNonPast(Person::k_second, Plurality::k_plural, forms[nonPastColumn]);
		else if (label.find("3rd plural") != std::string::npos)
			conjugation.SetNonPast(Person::k_third, Plurality::k_plural, forms[nonPastColumn]);
		else if (label == "imperative")
		{
			conjugation.SetImperative(Plurality::k_singular, forms[0]);
			conjugation.SetImperative(Plurality::k_plural, forms[1]);
		}
		else if (label.find("masculine") != std::string::npos)
		{
			conjugation.SetPast(Gender::k_masculine, forms[0]);
			conjugation.SetPast(Gender::k_plural, forms[1]);
		}
		else if (label.find("feminine") != std::string::npos)
			conjugation.SetPast(Gender::k_feminine, forms[0]);
		else if (label.find("neuter") != std::string::npos)
			conjugation.SetPast(Gender::k_neuter, forms[0]);
		else if (label.find("active") != std::string::npos)
		{
			conjugation.SetParticiple(Participle::k_active, Tense::k_present, forms[0]);
			conjugation.SetParticiple(Participle::k_active, Tense::k_past, forms[1]);
		}
		else if (label.find("passive") != std::string::npos)
		{
			conjugation.SetParticiple(Participle::k_passive, Tense::k_present, forms[0]);
			conjugation.SetParticiple(Participle::k_passive, Tense::k_past, forms[1]);
		}
		else if (label.find("adverbial") != std::string::npos)
		{
			conjugation.SetParticiple(Participle::k_adverbial, Tense::k_present, forms[0]);
			conjugation.SetParticiple(Participle::k_adverbial, Tense::k_past, forms[1]);
		}
	}
	
	return conjugation;
}

void ParseTermList(PageSection* section,
	Set<AccentedText>& outTerms)
{
	Array<xml::Node> nodes;
	section->FindAll(".//*[@lang='ru']", nodes);
	for (auto node : nodes)
	{
		unistr text = ConvertFromUTF8(node.Text());
		cmg::string::TrimIP(text);
		AccentedText term(text);
		outTerms.insert(term);
	}
}

Definition ParseDefinition(xml::Node node)
{
	Definition definition;
	String defString;
	for (auto child = node.Children();
		child && child.GetName() != "dl"; child = child.Next())
	{
		defString += child.Text();
	}
	cmg::string::TrimIP(defString);
	definition.SetDefinition(defString);

	// TODO: Parse examples

	return definition;
}

}


Parser::Parser(RequestInterface& requests) :
	m_requests(requests)
{
}

Parser::~Parser()
{
}

Term::sptr Parser::DownloadTerm(const unistr& text)
{
	CMG_LOG_DEBUG() << "Downloading wiktionary term: " << text;
	unistr url = GetTermURL(text, false);
	String content;
	Error error = m_requests.Request(ConvertToUTF8(url), content);
	if (error.Failed())
	{
		CMG_LOG_ERROR() << "Failed to download URL " << url;
		return nullptr;
	}
	
	// Parse the document into HTML
	xml::Document html(content);
	if (html.GetParseError().Failed())
	{
		CMG_LOG_ERROR() << "Failed to parse HTML from URL " << url;
		return nullptr;
	}
	xml::Node div = html.GetRootNode().FindFirst("//div[@class='mw-parser-output']");
	if (!div)
		return nullptr;
	PageSection tree("", 0, nullptr);
	PageSection* currentSection = &tree;
	PageSection* russianSection = nullptr;
	Array<String> headerTags = {"h1", "h2", "h3", "h4", "h5", "h6"};
	for (auto child = div.Children(); child; child = child.Next())
	{
		String element = child.GetName();
		if (cmg::container::Contains(headerTags, element))
		{
			uint32 depth = (uint32) element[1] - (uint32) '0';
			xml::Node headline = child.FindFirst(".//span[@class='mw-headline']");
			if (!headline)
				continue;
			String name = headline.Text();
			cmg::string::ToLowerIP(name);
			while (currentSection && depth <= currentSection->depth)
				currentSection = currentSection->parent;
			PageSection* childSection = new PageSection(name, depth, currentSection);
			currentSection->children.push_back(childSection);
			currentSection = childSection;
			if (name == "russian")
				russianSection = currentSection;
		}
		else 
		{
			currentSection->nodes.push_back(child);
		}
	}

	if (!russianSection)
		return nullptr;

	Term::sptr term = cmg::make_shared<Term>(text);

	// Parse top-level etymology
	PageSection* etymology = russianSection->GetSection("etymology 1");
	if (etymology)
		russianSection = etymology;
	else
		etymology = russianSection->GetSection("etymology");
	if (etymology)
		term->m_etymology = cmg::string::Trim(etymology->Text());

	// Parse each potential word type section
	for (WordType wordType : EnumValues<WordType>())
	{
		String wordTypeName = cmg::string::ToLower(EnumToString(wordType));
		PageSection* wordSection = russianSection->GetSection(wordTypeName);
		if (!wordSection && wordType == WordType::k_adjective)
			wordSection = russianSection->GetSection("determiner");
		if (!wordSection && (wordType == WordType::k_adjective || wordType == WordType::k_adverb))
			wordSection = russianSection->GetSection("participle");
		if (!wordSection && wordType == WordType::k_adverb)
			wordSection = russianSection->GetSection("predicative");
		if (!wordSection)
			continue;
		Word::sptr word = ParseWord(wordSection, wordType, text);
		if (word)
			term->m_words[wordType] = word;
	}

	// Log info about the term
	for (auto it : term->GetWords())
	{
		WordType wordType = it.first;
		Word::sptr word = it.second;

		/*auto tags = word->GetTags();
		String tagString = "";
		for (auto it : tags)
		{
			if (it.second)
			{
				CardTags tag = it.first;
				tagString += EnumToString(tag) + ", ";
			}
		}*/

		CMG_LOG_DEBUG() << "  " << word->GetInfoString().ToMarkedString();

		uint32 index = 0;
		for (auto definition : word->GetDefinitions())
		{
			CMG_LOG_DEBUG() << "    " << (index + 1) << ". " <<
				definition.GetDefinition();
			index++;
		}
	}

	term->m_downloadTimestamp = GetAppTimestamp();
	return term;
}

unistr Parser::GetTermURL(const unistr& term, bool russianSection)
{
	unistr termStr = term;
	ru::ToLowerIP(termStr);
	cmg::string::ReplaceAll(termStr, u" ", u"_");
	unistr url = u"https://en.wiktionary.org/wiki/" + termStr;
	if (russianSection)
		url += u"#Russian";
	return url;
}


} // namespace wiki
