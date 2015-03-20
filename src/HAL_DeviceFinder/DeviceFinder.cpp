/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
// DeviceFinder.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"

#ifndef WIN32
#include "udev_helper.h"
#endif

int main(int argc, char* argv[])
{
	string sFilename = "input.txt";
	char c;
	bool bError = false;

	for(int optnum=1; optnum < argc; ++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			bError = true;
		}

		c = argv[optnum][1];
		switch (c)
		{
		case 'f':
			sFilename = argv[++optnum];
			break;

		default:
			bError=true;
			break;
		};
	}

	if (bError)
	{
		cout << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
			<< "Usage: DeviceFinder -f <Input Filename>" << endl;
		exit(1);
	}

	vector<string> vectLines;
	map<unsigned long, vector<string> > mapDevices;
	FileUtils::ReadFileIntoVector(sFilename, vectLines);

	if(vectLines.size() > 0)
	{
		for(vector<string>::iterator it = vectLines.begin(), end = vectLines.end(); it != end; ++it)
		{
			string sLine = *it;
			vector<string> vectTokens;
			sLine = StringUtils::Replace(sLine, "||", "| |");
			StringUtils::Tokenize(sLine, "|", vectTokens);

			if(vectTokens.size() != 5)
			{
				cerr << "Error: Ignoring line '" << sLine << "'. It doesn't have 5 tokens" << endl;
			}
			else
			{
				string sVendorModelIDs = vectTokens[0];
				unsigned int ulVendorModelID = 0;

				vector<string> vectVendors;
				StringUtils::Tokenize(sVendorModelIDs, ":", vectVendors);

				for(vector<string>::iterator itv = vectVendors.begin(), endv = vectVendors.end(); itv != endv; ++itv)
				{
					string sVendorModelID = *itv;
					sscanf(sVendorModelID.c_str(), "%x", &ulVendorModelID);

					//cerr << "adding vendor id in the map: " << ulVendorModelID << endl;
					mapDevices.insert(make_pair((unsigned long)ulVendorModelID, vectTokens));
				}
			}
		}

#ifndef WIN32
		UdevHelper::FindDevice(mapDevices);
#endif
	}
	else
	{
		cerr << "Error: No vendor id found!" << endl;
	}

	return 0;
}

