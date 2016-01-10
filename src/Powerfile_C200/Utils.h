#ifndef PowerfileUtils_H
#define PowerfileUtils_H

#include <string>
#include <vector>
using namespace std;

// Call Tokenize directly if you don't need to strip out eventual empty fields
void ExtractFields(string &sRow, vector<string> &vect_sResult, const char * separator = " \t");

#endif /*PowerfileUtils_H*/
