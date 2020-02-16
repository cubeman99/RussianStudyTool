#include "Wiktionary.h"
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

Term::sptr Wiktionary::GetTerm(const unistr& text)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutex);
	auto it = m_terms.find(text);
	if (it != m_terms.end())
		return it->second;
	return nullptr;
}

Term::sptr Wiktionary::DownloadTerm(const unistr& text)
{
	Term::sptr term = m_parser.DownloadTerm(text);

	if (term)
	{
		std::lock_guard<std::recursive_mutex> guard(m_mutex);
		m_terms[text] = term;
	}

	if (term)
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

	// TOOD: error terms

	// Deserialize all terms
	rapidjson::Value& termsDataList = document["terms"];
	for (auto it = termsDataList.MemberBegin();
		it != termsDataList.MemberEnd(); it++)
	{
		Term::sptr term = cmg::make_shared<Term>();
		unistr key = ConvertFromUTF8(it->name.GetString());
		error = term->Deserialize(it->value);
		if (error.Failed())
			return error.Uncheck();
		m_terms[key] = term;
	}

	CMG_LOG_DEBUG() << "Loaded " << m_terms.size() << " Wiktionary terms";

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


} // namespace wiki
