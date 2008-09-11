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
#include "XRandrParser.h"

#include <cstring>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <iostream>

XRandrParser* XRandrParser::Instance = NULL;

XRandrParser::XRandrParser()
{
}

XRandrParser* XRandrParser::GetInstance()
{
	if (!Instance) {
    	Instance = new XRandrParser();
	}
	return Instance;
}

ScreenModes* XRandrParser::ParseFile(char* FileName)
{
	//reads a line of maximum 1023 characters
	char Buffer[1024];
	std::ifstream FInput;

	FInput.open(FileName);
	if(!FInput)
		return NULL;

	ScreenModes* Result = new ScreenModes();

	//Skip the first line, which doesn't offer infos for
	// screen modes
	FInput.getline(Buffer, 1024);

	while(!FInput.eof())
	{
		FInput.getline(Buffer, 1024);
		int UsedRefreshRate = -1;
		Resolution* Mode = ParseLine(Buffer, UsedRefreshRate);

		if(Mode)
		{
			Result->AddScreenMode(*Mode);
			if (UsedRefreshRate != -1)
				Result->SetDefault(Mode->GetWidth(), Mode->GetHeight(), UsedRefreshRate);
			
			delete Mode;
		}
	}

	return Result;
}

ScreenModes* ParseXRandrOutput()
{
#ifdef WIN32
	return XRandrParser::GetInstance()->ParseFile("filetoparse.txt");
#else
	system("xrandr -d :0 -q > /tmp/pluto_xrandr.txt");
	return XRandrParser::GetInstance()->ParseFile("/tmp/pluto_xrandr.txt");
#endif
	
}

Resolution* XRandrParser::ParseLine(char* LineToParse, int& UsedRefreshRate)
{
	int Width, Height, Refresh;

	if(LineToParse == NULL)
		return NULL;
	if(strlen(LineToParse) == 0)
		return NULL;

	char* ResToken = strtok(LineToParse, " ");

	bool IsUsedResolution = (*ResToken) == '*';
	if (IsUsedResolution)
	{
		ResToken++;
		UsedRefreshRate = 0;
	}

	ResToken = strtok(NULL, " ");
	Width = StrToInt(ResToken);

	ResToken = strtok(NULL, " "); //expected to be "x"
	ResToken = strtok(NULL, " ");
	Height = StrToInt(ResToken);

	if((Width < 640) || (Height < 480))
		return NULL;

	Resolution* Result = new Resolution(Width, Height);

	ResToken = strtok(NULL, ")");
	ResToken = strtok(NULL, " ");

	while (ResToken != NULL)
	{
		if(*ResToken == '*')
		{
			ResToken++;
			UsedRefreshRate = StrToInt(ResToken);
			Refresh = UsedRefreshRate;
		}
		else
			Refresh = StrToInt(ResToken);

		Result->AddRefreshRate(Refresh);

		ResToken = strtok(NULL, " ");
	}

	return Result;
}

int XRandrParser::StrToInt(char* String)
{
	int Result;

	sscanf(String, "%d", &Result);	
	return Result;
}

