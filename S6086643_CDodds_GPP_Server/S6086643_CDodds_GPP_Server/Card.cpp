#include "Card.h"
#include <sstream>

std::string Card::toString()
{
	std::ostringstream outString;
	outString << (int)suit << "::" << (int)value << "::" << (int)seen;
	return outString.str();
}
