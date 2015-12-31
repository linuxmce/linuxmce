/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file HttpUtils.h
 Header file for HttpPost.
 */
#include <map>
#include <string>
#include <vector>

using namespace std;

string HttpPost(string sURL, const vector<string>& vectHeaders, const map<string, string>& mapParams,
				string User = "", string Password = "", int nPort = 80);
string HttpGet(string sURL, string *buffer, string User = "", string Password = "", int nPort = 80);
void CurlInit();
