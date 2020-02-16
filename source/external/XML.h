#pragma once

#include <cmgCore/cmg_core.h>
#include "core/Unicode.h"
#include <libxml/tree.h>
#include <libxml/xpath.h>


namespace xml {

class Document;

class Node
{
public:
	Node();
	Node(Document* document);
	Node(Document* document, xmlNodePtr node);

	Node Children();
	Node Next();
	String GetName();
	String Content();
	String Text();
	bool HasAttribute(const char* name);
	bool HasAttribute(const String& name);
	bool HasAttribute(const char* name, const char* value);
	String GetAttribute(const char* name);
	String GetAttribute(const String& name);
	bool TryGetAttribute(const char* name, String& outValue);
	bool TryGetAttribute(const String& name, String& outValue);

	operator bool() const;

	Node FindFirst(const String& xpath);
	Array<Node> FindAll(const String& expression);
	void FindAll(const String& expression, Array<Node>& outNodes);

private:
	Document* m_document = nullptr;
	xmlNodePtr m_node = nullptr;
};


class Document
{
public:
	friend class Node;

	Document(const String& html);
	~Document();

	Error GetParseError() const;
	Node GetRootNode();

private:
	xmlDocPtr m_document = nullptr;
	xmlNodePtr m_root = nullptr;
	xmlXPathContextPtr m_xpathContext = nullptr;
	Error m_parseError;
};

} // namespace xml
