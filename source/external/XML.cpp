#include "XML.h"
//#define LIBXML_STATIC
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

namespace xml {

Node::Node()
{
}
Node::Node(Document* document) :
	m_document(document)
{
}
Node::Node(Document* document, xmlNodePtr node) :
	m_document(document),
	m_node(node)
{
}

Node Node::Children()
{
	return Node(m_document, m_node->children);
}
Node Node::Next()
{
	return Node(m_document, m_node->next);
}
String Node::GetName()
{
	return String((const char*) m_node->name);
}
String Node::Content()
{
	if (m_node->content)
		return String((const char*) m_node->content);
	return "";
}
String Node::Text()
{
	String text = "";
	if (m_node->type == XML_TEXT_NODE)
		return String((const char*) m_node->content);
	for (auto child = Children(); child; child = child.Next())
		text += child.Text();
	return text;
}

bool Node::HasAttribute(const char* name)
{
	return xmlGetProp(m_node, (const xmlChar*) name) != nullptr;
}

bool Node::HasAttribute(const String& name)
{
	return HasAttribute(name.c_str());
}

bool Node::HasAttribute(const char* name, const char* value)
{
	xmlChar* attr = xmlGetProp(m_node, (const xmlChar*) name);
	bool matches = (attr && xmlStrcmp(attr, (const xmlChar*) value) == 0);
	String result((char*) value);
	xmlFree(attr);
	return matches;
}

String Node::GetAttribute(const char* name)
{
	xmlChar* value = xmlGetProp(m_node, (const xmlChar*) name);
	CMG_ASSERT(value != nullptr);
	String result((char*) value);
	xmlFree(value);
	return result;
}

String Node::GetAttribute(const String& name)
{
	return GetAttribute(name.c_str());
}

bool Node::TryGetAttribute(const char* name, String& outValue)
{
	xmlChar* value = xmlGetProp(m_node, (const xmlChar*) name);
	if (value == nullptr)
		return false;
	outValue = (char*) value;
	xmlFree(value);
	return true;
}

bool Node::TryGetAttribute(const String& name, String& outValue)
{
	return TryGetAttribute(name.c_str(), outValue);
}

Node::operator bool() const {
	return m_node != nullptr;
}

Node Document::GetRootNode()
{
	return Node(this, m_root);
}

Document::Document(const String& xml)
{
	m_document = xmlReadMemory(
		xml.c_str(), xml.length(), "noname.xml", NULL, 0);
	if (!m_document)
	{
		m_parseError = CMG_ERROR_FAILURE;
	}
	else
	{
		m_parseError = CMG_ERROR_SUCCESS;
		m_root = xmlDocGetRootElement(m_document);
		m_xpathContext = xmlXPathNewContext(m_document);
	}
}

Document::~Document()
{
	if (m_xpathContext)
		xmlXPathFreeContext(m_xpathContext);
	if (m_document)
		xmlFreeDoc(m_document);
}

Error Document::GetParseError() const
{
	return m_parseError;
}


Node Node::FindFirst(const String& expression)
{
	xmlXPathObjectPtr nodes = xmlXPathNodeEval(m_node,
		(const xmlChar*) expression.c_str(), m_document->m_xpathContext);
	xmlNodePtr result = nullptr;
	if (nodes->nodesetval->nodeNr != 0)
		result = nodes->nodesetval->nodeTab[0];
	xmlXPathFreeObject(nodes);
	return Node(m_document, result);
}

Array<Node> Node::FindAll(const String& expression)
{
	Array<Node> results;
	FindAll(expression, results);
	return results;
}

void Node::FindAll(const String & expression, Array<Node>& outNodes)
{
	xmlXPathObjectPtr nodes = xmlXPathNodeEval(m_node,
		(const xmlChar*) expression.c_str(), m_document->m_xpathContext);
	xmlNodePtr result = nullptr;
	for (int i = 0; i < nodes->nodesetval->nodeNr; i++)
		outNodes.push_back(Node(m_document, nodes->nodesetval->nodeTab[i]));
	xmlXPathFreeObject(nodes);
}

}