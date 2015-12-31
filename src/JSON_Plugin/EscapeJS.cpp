#include "EscapeJS.h"

#include <sstream>

std::string escapeJsonString(const std::string& input)
{
	std::ostringstream ss;

	for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++)
	{
		switch (*iter)
		{
			case '\\': ss << "\\\\"; break;
			case '"': ss << "\\\""; break;
			case '/': ss << "\\/"; break;
			case '\b': ss << "\\b"; break;
			case '\f': ss << "\\f"; break;
			case '\n': ss << "\\n"; break;
			case '\r': ss << "\\r"; break;
			case '\t': ss << "\\t"; break;
			default: ss << *iter; break;
		}
	}

	return ss.str();
}
