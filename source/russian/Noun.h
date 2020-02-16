#pragma once

#include "Russian.h"
#include "core/JsonUtility.h"

namespace ru {

class NounDeclension
{
public:
	NounDeclension();

	const AccentedText& GetDeclension(Case nounCase, Plurality plurality) const;
	void SetDeclension(Case nounCase, Plurality plurality, const AccentedText& text);

	void Serialize(rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator);
	Error Deserialize(rapidjson::Value& data);

private:
	Map<std::pair<Case, Plurality>, AccentedText> m_declension;
};

} // namespace ru