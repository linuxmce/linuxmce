/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include <curl/curl.h>
#include <curl/easy.h>

#include "HttpUtils.h"

string HttpPost(string sURL, const vector<string>& vectHeaders, const map<string, string>& mapParams, 
				string sUser, string sPassword, int nPort)
{
	string sResponse;

	CURLcode res;
	struct curl_slist *headerlist = NULL;
	string sParams;

	CurlInit();
	CURL *curl = curl_easy_init();

	for(map<string, string>::const_iterator it = mapParams.begin(); it != mapParams.end();)	
	{
		string sParamName = it->first;
		string sParamValue = it->second;
		sParams += sParamName + "=" + sParamValue;
		++it;
		
		if(it != mapParams.end())
			sParams += "&";
	}

	for(vector<string>::const_iterator ith = vectHeaders.begin(); ith != vectHeaders.end(); ++ith)
	{
		string sHeader = *ith;
		curl_slist_append(headerlist, sHeader.c_str()); 
	}

	string Authenticate = sUser + ":" + sPassword;

	if(curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, sURL.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);// allow redirects 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sParams.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sParams.length());
		curl_easy_setopt(curl, CURLOPT_POST, 1); // set POST method 
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); 
		curl_easy_setopt(curl, CURLOPT_PORT, nPort);

		if(sUser != "")
			curl_easy_setopt(curl, CURLOPT_USERPWD, Authenticate.c_str()); 

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

		res = curl_easy_perform(curl);

		//always cleanup 
		curl_easy_cleanup(curl);

		//free slist
		curl_slist_free_all(headerlist);
	}

	return res == CURLE_OK ? "OK" : "ERROR";
}
extern "C" {
int ReceiveData(char *data, size_t size, size_t nmemb, string *buffer)
{
	int result = 0;
	if (buffer != NULL)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}
}

string HttpGet(string sURL, string *buffer, string sUser, string sPassword, int nPort)
{
	string sResponse;

	CURLcode res;

	CurlInit();
	CURL *curl = curl_easy_init();

	string Authenticate = sUser + ":" + sPassword;
	char errorBuffer[CURL_ERROR_SIZE];

	if(curl) 
	{
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, sURL.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);// allow redirects 
		curl_easy_setopt(curl, CURLOPT_HEADER, 0); 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ReceiveData); 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

		if(sUser != "")
			curl_easy_setopt(curl, CURLOPT_USERPWD, Authenticate.c_str()); 

		res = curl_easy_perform(curl);
	
		curl_easy_cleanup(curl);

	}
	
	return res == CURLE_OK ? "OK" : "ERROR";
}

bool bCurlInited = false;
void CurlInit()
{
	if (!bCurlInited)
	{
		bCurlInited = true;
		curl_global_init(CURL_GLOBAL_ALL);
	}
}
