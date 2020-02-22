#include "ExampleDatabase.h"
#include "Config.h"
#include "core/JsonUtility.h"
#include <fstream>

ExampleDatabase::ExampleDatabase(wiki::Wiktionary& wiktionary) :
	m_wiktionary(wiktionary)
{
}

ExampleDatabase::~ExampleDatabase()
{
}

uint32 ExampleDatabase::GetExamplesInSentence(const AccentedText& sentence,
	const Array<WordMatchPattern>& wordPatterns,
	Array<std::pair<uint32, uint32>>& outMatchLocations)
{
	const unistr& str = sentence.GetString();
	uint32 length = str.length();
	Array<unistr> sentenceWordList;
	Array<uint32> wordStartIndices;
	unistr sentenceWord;
	uint32 count = 0;
	if (wordPatterns.empty())
		return 0;

	// Iterating to length + 1 here so we can process the end-of-word
	// at the end of the sentence.
	for (uint32 i = 0; i <= length; i++)
	{
		unichar c = (i < length ? ru::ToLowerChar(str[i]) : u'.');
		if (c == u'ё')
			c = u'е';

		if (ru::IsRussian(c))
		{
			if (sentenceWord.length() == 0)
				wordStartIndices.push_back(i);
			sentenceWord += c;
		}
		else if (sentenceWord.length() > 0)
		{
			sentenceWordList.push_back(sentenceWord);
			sentenceWord = u"";

			// Check for any new word matches
			for (auto& words : wordPatterns)
			{
				if (sentenceWordList.size() >= words.words.size())
				{
					// Verify each word in the pattern matches
					bool wordMatches = true;
					uint32 wordIndex = sentenceWordList.size() - words.words.size();
					for (uint32 j = 0; j < words.words.size() && wordMatches; j++)
					{
						wordMatches = false;
						for (const unistr& wordForm : words.words[j])
						{
							wordMatches = (wordForm == sentenceWordList[wordIndex + j]);
							if (wordMatches)
								break;
						}
					}

					if (wordMatches)
					{
						count++;
						outMatchLocations.push_back(
							{wordStartIndices[wordIndex], i});
					}
				}
			}
		}
	}
	return count;
}

Set<unistr> ExampleDatabase::GetWordForms(const unistr& word)
{
	Set<unistr> forms;
	forms.insert(word);

	wiki::Term::sptr term = m_wiktionary.GetTerm(word);
	if (!term)
		term = m_wiktionary.DownloadTerm(word);
	if (term)
	{
		Set<AccentedText> accentedForms;
		term->GetAllForms(accentedForms);
		for (const AccentedText& text : accentedForms)
		{
			unistr str = text.GetString();
			ru::ToLowerIP(str);
			cmg::string::ReplaceAll(str, u"ё", u"e");
			if (str.length() > 0)
				forms.insert(str);
		}
	}

	return forms;
}

Array<SentenceMatch> ExampleDatabase::GetExampleSentences(Card::sptr card)
{
	const unistr& str = card->GetRussian().GetString();
	uint32 length = str.length();
	Array<WordMatchPattern> wordPatterns;
	WordMatchPattern words;
	unistr word;
	uint32 count = 0;

	// Iterating to length + 1 here so we can process the end-of-word
	// at the end of the sentence.
	for (uint32 i = 0; i <= length; i++)
	{
		unichar c = (i < length ? ru::ToLowerChar(str[i]) : u';');
		if (c == u'ё')
			c = u'е';

		if (ru::IsRussian(c))
		{
			word += c;
		}
		else
		{
			if (word.length() > 0)
			{
				words.words.push_back(GetWordForms(word));
				word = u"";
			}
			if (c == u';' && !words.words.empty())
			{
				wordPatterns.push_back(words);
				words.words.clear();
			}
		}
	}

	return GetExampleSentences(wordPatterns);
}

Array<SentenceMatch> ExampleDatabase::GetExampleSentences(
	const Array<WordMatchPattern>& words)
{
	Array<SentenceMatch> matches;
	GetExampleSentences(words, matches);
	return matches;
}

uint32 ExampleDatabase::GetExampleSentences(
	const Array<WordMatchPattern>& words, Array<SentenceMatch>& outMatches)
{
	SentenceMatch match;
	uint32 count = 0;
	for (Story::sptr story : m_stories)
	{
		for (Chapter::sptr chapter : story->m_chapters)
		{
			for (Paragraph::sptr paragraph : chapter->m_paragraphs)
			{
				for (const AccentedText& sentence : paragraph->m_sentences)
				{
					if (GetExamplesInSentence(sentence, words, match.locations) > 0)
					{
						count += match.locations.size();
						match.sentence = sentence;
						outMatches.push_back(match);
						match = SentenceMatch();
					}
				}
			}
		}
	}
	return count;
}

Error ExampleDatabase::LoadExamples(const PathU16& path)
{
	CMG_LOG_INFO() << "Loading examples from: " << path;
	return LoadExamplesInDir(path);
}

Error ExampleDatabase::LoadExamplesInDir(const PathU16& path)
{
	Error error;
	Story::sptr story;

	for (auto& p : std::filesystem::directory_iterator(
		std::filesystem::path((const wchar_t*) path.c_str())))
	{
		PathU16 subPath(p.path().u16string());

		if (p.is_directory())
		{
			error = LoadExamplesInDir(subPath);
			if (error.Failed())
				return error.Uncheck();
		}
		else if (p.is_regular_file())
		{
			unistr extension = p.path().extension().u16string();
			if (extension == u".story")
			{
				error = LoadStory(subPath, story);
				if (error.Failed())
					return error.Uncheck();
			}
		}
	}

	return CMG_ERROR_SUCCESS;
}

Error ExampleDatabase::LoadStory(const PathU16& path, Story::sptr& outStory)
{
	outStory = cmg::make_shared<Story>();
	outStory->m_title = path.GetNameWithoutExtension();
	CMG_LOG_DEBUG() << "Loading story: " << path.GetNameWithoutExtension();

	File file;
	file.Open(path, FileAccess::READ, FileType::TEXT);
	std::ifstream stream(file.GetFile());

	Chapter::sptr chapter;
	bool english = false;

	// Read the file line-by-line
	String line;
	while (std::getline(stream, line))
	{
		cmg::string::TrimIP(line);
		if (line.length() == 0)
			continue;

		if (line[0] == '@')
		{
			Array<String> tokens = cmg::string::Split(line, " ", 2);
			String command = tokens[0].substr(1);
			unistr args = u"";
			if (tokens.size() > 1)
				args = ConvertFromUTF8(tokens[1]);
			if (command == "title")
			 	outStory->m_title = args;
			else if (command == "url")
				outStory->m_url = args;
			else if (command == "chapter")
			{
				chapter = outStory->AddChapter(args);
				english = false;
			}
			else if (command == "english")
			{
				english = true;
			}
		}
		else
		{
			if (!chapter)
				chapter = outStory->AddChapter("");
			if (!english)
				chapter->AddParagraph(AccentedText(ConvertFromUTF8(line)));
		}
	}

	if (!chapter)
		return CMG_ERROR_FAILURE;
	m_stories.push_back(outStory);
	return CMG_ERROR_SUCCESS;
}
