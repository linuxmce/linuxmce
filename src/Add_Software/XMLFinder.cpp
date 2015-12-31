/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "XMLFinder.h"

#include "../PlutoUtils/FileUtils.h"
#include "../DCE/Logger.h"

using namespace std;
using namespace DCE;
//--------------------------------------------------------------------------------------------------
void XMLFinder::FindXMLs(string sFilename, vector<string>& vectXmls)
{
	const string csPackagesToken = "packages.xml";
	const string csHrefToken = "href=\"";
	const size_t cnNeighbourhood = 100;

	string sHTMLData;
	if(FileUtils::ReadTextFile(sFilename, sHTMLData))
	{
		size_t pos = 0;
		while((pos = sHTMLData.find(csPackagesToken)) != string::npos)
		{
			string sUsefullData = sHTMLData.substr(pos > cnNeighbourhood ? pos - cnNeighbourhood : 0, 
				cnNeighbourhood + csPackagesToken.length());

			size_t nHrefTagPos = sUsefullData.find(csHrefToken);
			size_t nPackageTagPos = sUsefullData.find(csPackagesToken);

			if(nHrefTagPos != string::npos && nPackageTagPos != string::npos)
			{
				string sUrlRawData = sUsefullData.substr(
					nHrefTagPos + csHrefToken.length(), 
					nPackageTagPos + csPackagesToken.length() - nHrefTagPos - csHrefToken.length());

				vectXmls.push_back(sUrlRawData);
			}

			sHTMLData = sHTMLData.substr(pos + csPackagesToken.length(), sHTMLData.length() - pos - 1);
		}
	}
}
//--------------------------------------------------------------------------------------------------
void XMLFinder::FindURLs(string sFilename, vector<string>& vectUrls)
{
	const string csHrefToken = "href=\"http";
	const size_t cnNeighbourhood = 600;

	string sHTMLData;
	if(FileUtils::ReadTextFile(sFilename, sHTMLData))
	{
		const string csStartToken = "http://www.google.ro/preferences";
		size_t nStartPos = sHTMLData.find(csStartToken);
		if(nStartPos != string::npos)
			sHTMLData = sHTMLData.substr(nStartPos + csStartToken.length(), sHTMLData.length() - nStartPos - 1);

		size_t pos = 0;
		while((pos = sHTMLData.find(csHrefToken)) != string::npos)
		{
			string sUsefullData = sHTMLData.substr(pos + csHrefToken.length() - 4, cnNeighbourhood);
			size_t nEndPos = sUsefullData.find("\"");

			if(nEndPos != string::npos)
			{
				string sUrlRawData =  sUsefullData.substr(0, nEndPos);
				vectUrls.push_back(sUrlRawData);
			}

			sHTMLData = sHTMLData.substr(pos + csHrefToken.length(), sHTMLData.length() - pos - 1);
		}
	}
}
//--------------------------------------------------------------------------------------------------
#ifdef UNIT_TEST_XML_FINDER

int main()
{
/*
	vector<string> vectUrls;
	XMLFinder::FindURLs("/temp/search.htm", vectUrls);

	for(vector<string>::iterator it = vectUrls.begin(); it != vectUrls.end(); ++it)
	{
		string sUrl = *it;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found html : %s", sUrl.c_str());
	}*/

	vector<string> vectUrls;
	XMLFinder::FindXMLs("/temp/out.html", vectUrls);

	for(vector<string>::iterator it = vectUrls.begin(); it != vectUrls.end(); ++it)
	{
		string sUrl = *it;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found xml : %s", sUrl.c_str());
	}

	return 0;
}
#endif
