#include "RequestInterface.h"
#include <curl/curl.h>

namespace
{
	size_t CurlWriteFunction(char* buffer, size_t size, size_t nmemb, void* userp)
	{
		((String*) userp)->append(buffer, size * nmemb);
		return size * nmemb;
	}

	size_t CurlWriteBytesFunction(char* buffer, size_t size, size_t nmemb, void* userp)
	{
		Array<uint8>* data = (Array<uint8>*) userp;
		size_t offset = data->size();
		data->resize(offset + (size * nmemb));
		memcpy(data->data() + offset, buffer, size * nmemb);
		return size * nmemb;
	}
}

RequestInterface::RequestInterface()
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();
	curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 0L);
}

RequestInterface::~RequestInterface()
{
	curl_easy_cleanup((CURL*) m_curl);
	curl_global_cleanup();
}

Error RequestInterface::Download(const String& url, Array<uint8>& outData)
{
	char* escapedUrl = curl_easy_escape(
		(CURL*) m_curl, url.c_str(), url.length());
	curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &outData);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, CurlWriteBytesFunction);
	curl_easy_perform(m_curl);
	curl_free(escapedUrl);
	if (outData.empty())
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}

Error RequestInterface::Request(const String& url, String& outResponse)
{
	char* escapedUrl = curl_easy_escape(
		(CURL*) m_curl, url.c_str(), url.length());
	curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &outResponse);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, CurlWriteFunction);
	curl_easy_perform(m_curl);
	curl_free(escapedUrl);
	if (outResponse.length() == 0)
		return CMG_ERROR_FAILURE;
	return CMG_ERROR_SUCCESS;
}

Error RequestInterface::Request(const unistr& url, unistr& outResponse)
{
	String responseRaw;
	Error error = Request(ConvertToUTF8(url), responseRaw);
	if (error.Failed())
		return error.Uncheck();
	outResponse = ConvertFromUTF8(responseRaw);
	return CMG_ERROR_SUCCESS;
}
