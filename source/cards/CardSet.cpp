#include "CardSet.h"


std::ostream& operator<<(std::ostream& out, const CardSetKey& key)
{
	out << "<CardSetKey(" << (AccentedText) key.name << ")>";
	return out;
}
