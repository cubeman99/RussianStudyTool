#include "Wiktionary.h"
#include "core/JsonUtility.h"

namespace wiki
{

Wiktionary::Wiktionary()
{
}

Wiktionary::~Wiktionary()
{
}

Term::sptr Wiktionary::GetTerm(const unistr& text)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutex);
	auto it = m_terms.find(text);
	if (it != m_terms.end())
		return it->second;
	return nullptr;
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
	m_savePath = path;

	return CMG_ERROR_SUCCESS;
}


} // namespace wiki
