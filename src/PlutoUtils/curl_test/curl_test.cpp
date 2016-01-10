/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
// curl_test.cpp : Defines the entry point for the console application.
//

#include "../HttpUtils.h"

string URLEncode(string sInput)
{
	char hexValues[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	const char *inBuffer = sInput.c_str();
	char *outBuffer = new char[(sInput.length() * 3) + 1];
	char *baseOutBuffer = outBuffer;

	while ( *inBuffer != 0 )
	{
		switch ( *inBuffer )
		{
		case '\"': case '<': case '>': case '%': case '\\':
		case  '^': case '[': case ']': case '`': case '+':
		case  '$': case ',': case '@': case ';': case '/':
		case  '!': case '#': case '?': case '=': case '&':
		case  ':':
			*outBuffer++ = '%';
			*outBuffer++ = hexValues[(*inBuffer >> 4) & 0x0F];
			*outBuffer++ = hexValues[*inBuffer & 0x0F];
			break;
		default:
			if ( *inBuffer > 32 && *inBuffer < 123 )
				*outBuffer++ = *inBuffer;
			else
			{
				*outBuffer++ = '%';
				*outBuffer++ = hexValues[(*inBuffer >> 4) & 0x0F];
				*outBuffer++ = hexValues[*inBuffer & 0x0F];
			}
		}

		inBuffer++;
	}
	*outBuffer = '\0';

	string result(baseOutBuffer);
	delete baseOutBuffer;
	return result;
}

int main(int argc, char* argv[])
{
	vector<string> vectHeaders;
	//vectHeaders.push_back("Accept: text/html, */*");

	map<string, string> mapParams;
	mapParams["XML"] = URLEncode("<CiscoIPPhoneExecute><ExecuteItem URL=\"http://192.168.80.1/lmce-admin/ServicesMenu.php\"/></CiscoIPPhoneExecute>");

	string Response = HttpPost("http://192.168.80.253/CGI/Execute", vectHeaders, mapParams, "a", "a");

	return 0;
}




