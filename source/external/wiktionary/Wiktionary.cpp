﻿#include "Wiktionary.h"
#include "core/JsonUtility.h"

namespace wiki
{

Wiktionary::Wiktionary(RequestInterface& requests) :
	m_parser(requests)
{
}

Wiktionary::~Wiktionary()
{
}

const Path& Wiktionary::GetDataPath() const
{
	return m_savePath;
}

void Wiktionary::GetTerm(const unistr& text, Term::sptr& outTerm, bool& needsDownload)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutex);
	auto it = m_terms.find(text);
	if (it != m_terms.end())
	{
		needsDownload = TermNeedsReDownload(it->second);
		outTerm = it->second;
	}
	else
	{
		outTerm = nullptr;
		needsDownload = !cmg::container::Contains(m_404Terms, text);
	}
}

Term::sptr Wiktionary::GetTerm(const unistr& text, bool download)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutex);
	auto it = m_terms.find(text);
	Term::sptr term = nullptr;
	bool redownload = false;
	if (it != m_terms.end())
	{
		redownload = TermNeedsReDownload(it->second);
		if (!redownload)
			term = it->second;
	}
	if (!term && download)
	{
		if (redownload)
			CMG_LOG_DEBUG() << "Re-downloading wiki term: " << text;
		term = DownloadTerm(text);
	}
	return term;
}

Map<unistr, Term::sptr>& Wiktionary::GetAllTerms()
{
	return m_terms;
}

Term::sptr Wiktionary::DownloadTerm(Term::sptr term)
{
	if (term)
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutex);
		m_terms.erase(term->GetKey());
	}
	return DownloadTerm(term->GetKey());
}

Term::sptr Wiktionary::DownloadTerm(const unistr& text)
{
	if (cmg::container::Contains(m_404Terms, text))
		return nullptr;

	Term::sptr term = m_parser.DownloadTerm(text);

	if (term)
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutex);
		std::lock_guard<std::mutex> guardQueue(m_mutexLoadedTermsQueue);
		m_terms[text] = term;
		m_loadedTermsQueue.push_back(term);
	}
	else
	{
		m_404Terms.insert(text);
	}

	if (!m_savePath.ToString().empty())
		Save();

	return term;
}

void Wiktionary::SetDataPath(const Path & path)
{
	m_savePath = path;
}

Error Wiktionary::Load()
{
	return Load(m_savePath);
}

Error Wiktionary::Load(const Path& path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutex);
	CMG_LOG_INFO() << "Loading Wiktionary data";

	m_terms.clear();
	m_errorTerms.clear();
	m_404Terms.clear();
	m_noWordTerms.clear();

	// Open the json file
	m_savePath = path;
	rapidjson::Document document;
	Error error = json::LoadDocumentFromFile(path, document);
	if (error.Failed())
		return error.Uncheck();

	// Error term lists
	rapidjson::Value& termListData404 = document["404_terms"];
	for (auto it = termListData404.Begin(); it != termListData404.End(); it++)
		m_404Terms.insert(ConvertFromUTF8(it->GetString()));
	rapidjson::Value& termListDataError = document["error_terms"];
	for (auto it = termListDataError.Begin(); it != termListDataError.End(); it++)
		m_errorTerms.insert(ConvertFromUTF8(it->GetString()));
	rapidjson::Value& termListDataNoWords = document["no_word_terms"];
	for (auto it = termListDataNoWords.Begin(); it != termListDataNoWords.End(); it++)
		m_noWordTerms.insert(ConvertFromUTF8(it->GetString()));

	// Deserialize all terms
	Array<wiki::Term::sptr> noWordTerms;
	Array<wiki::Term::sptr> termsNeedRedownload;
	rapidjson::Value& termsDataList = document["terms"];
	for (auto it = termsDataList.MemberBegin();
		it != termsDataList.MemberEnd(); it++)
	{
		unistr key = ConvertFromUTF8(it->name.GetString());
		Term::sptr term = cmg::make_shared<Term>(key);
		error = term->Deserialize(it->value);
		if (error.Failed())
			return error.Uncheck();
		m_terms[key] = term;

		if (TermNeedsReDownload(term))
		{
			termsNeedRedownload.push_back(term);
		}
		if (term->GetWords().empty())
		{
			noWordTerms.push_back(term);
			CMG_LOG_WARN() << "No words: " << term->GetKey();
		}
	}

	CMG_LOG_DEBUG() << "Loaded " << m_terms.size() << " Wiktionary terms (" <<
		m_404Terms.size() << " not-found terms)";
	if (!termsNeedRedownload.empty())
		CMG_LOG_DEBUG() << termsNeedRedownload.size() << " terms need to be re-downloaded";

	for (auto term : noWordTerms)
	{
		//DownloadTerm(term);
	}

	return CMG_ERROR_SUCCESS;
}

Error Wiktionary::Save()
{
	return Save(m_savePath);
}

Error Wiktionary::Save(const Path& path)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutex);
	m_savePath = path;

	rapidjson::Document document(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// 404 terms
	rapidjson::Value terms404Data(rapidjson::kArrayType);
	for (const unistr& term : m_404Terms)
	{
		String text = ConvertToUTF8(term);
		terms404Data.PushBack(rapidjson::Value(
			text.c_str(), allocator).Move(), allocator);
	}
	document.AddMember("404_terms", terms404Data, allocator);

	// Error terms
	rapidjson::Value termsErrorData(rapidjson::kArrayType);
	for (const unistr& term : m_errorTerms)
	{
		String text = ConvertToUTF8(term);
		termsErrorData.PushBack(rapidjson::Value(
			text.c_str(), allocator).Move(), allocator);
	}
	document.AddMember("error_terms", termsErrorData, allocator);

	// No word terms
	rapidjson::Value termsNoWordsData(rapidjson::kArrayType);
	for (const unistr& term : m_noWordTerms)
	{
		String text = ConvertToUTF8(term);
		termsNoWordsData.PushBack(rapidjson::Value(
			text.c_str(), allocator).Move(), allocator);
	}
	document.AddMember("no_word_terms", termsNoWordsData, allocator);

	rapidjson::Value termListData(rapidjson::kObjectType);
	for (auto it : m_terms)
	{
		rapidjson::Value termData(rapidjson::kObjectType);
		Term::sptr term = it.second;
		term->Serialize(termData, allocator);
		String termName = ConvertToUTF8(it.first);
		termListData.AddMember(rapidjson::Value(
			termName.c_str(), allocator).Move(), termData, allocator);
	}
	document.AddMember("terms", termListData, allocator);

	// Save to the file
	return json::SaveDocumentToFile(path, document);
}

void Wiktionary::ProcessEvents()
{
	Array<Term::sptr> queue;
	{
		std::lock_guard<std::mutex> guardQueue(m_mutexLoadedTermsQueue);
		queue = m_loadedTermsQueue;
		m_loadedTermsQueue.clear();
	}
	for (auto term : queue)
		m_eventTermLoaded.Emit(term);
}

unistr Wiktionary::GetTermURL(Term::sptr term, bool russianSection)
{
	return Parser::GetTermURL(term->GetText().GetString(), russianSection);
}

bool Wiktionary::TermNeedsReDownload(wiki::Term::sptr term)
{
	for (auto it : term->GetWords())
	{
		WordType wordType = it.first;
		Word::sptr word = it.second;

		if (wordType == WordType::k_noun)
		{
			Noun::sptr noun = std::dynamic_pointer_cast<Noun>(word);
			if (noun->GetDeclension().GetGender() == Gender::k_unknown ||
				noun->GetDeclension().GetAnimacy() == Animacy::k_unknown)
				return true;
		}
		else if (wordType == WordType::k_verb)
		{
			Verb::sptr verb = std::dynamic_pointer_cast<Verb>(word);
		}
	}
	return false;
}


} // namespace wiki
