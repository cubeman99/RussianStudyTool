#pragma once

#include <cmgCore/cmg_core.h>
#include "core/Unicode.h"


class RequestInterface
{
public:
	RequestInterface();
	~RequestInterface();

	Error Download(const String& url, Array<uint8>& outData);
	Error Request(const String& url, String& outResponse);
	Error Request(const unistr& url, unistr& outResponse);

private:
	void* m_curl = nullptr;
};
