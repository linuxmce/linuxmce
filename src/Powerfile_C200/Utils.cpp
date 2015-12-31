#include "Utils.h"

#include "../PlutoUtils/StringUtils.h"
using namespace StringUtils;

// Call Tokenize directly if you don't need to strip out eventual empty fields
void ExtractFields(string &sRow, vector<string> &vect_sResult, const char * separator)
{
	vector<string> vect_sFields;
	vect_sResult.clear();
	vect_sFields.clear();
	Tokenize(sRow, separator, vect_sFields);

	for (size_t i = 0; i < vect_sFields.size(); i++)
	{
		if (vect_sFields[i] == "")
			continue; // skip empty fields since there is no such thing
		vect_sResult.push_back(vect_sFields[i]);
	}
}
