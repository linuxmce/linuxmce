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
#include "amixerparser.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>

AMixerParser::AMixerParser(void)
{
}

AMixerParser::~AMixerParser(void)
{
}

AMixerOptions* AMixerParser::ParseConfig()
{
	char* FileName;
#ifdef WIN32
	FileName = "mixercontrols.txt";
#else
	FileName = "/tmp/mixercontrols.txt"
	char Command[1024];
	sprintf (Command, "amixer scontrols > %s", FileName);
	system(Command);
#endif
	std::ifstream FInput;

	char Buffer[1024];

	AMixerOptions* Mixer = new AMixerOptions();

	FInput.open(FileName);
	if(!FInput)
		return NULL;

	while(!FInput.eof())
	{
		FInput.getline(Buffer, 1024);
		BuildOptions(Mixer, Buffer);	
	}

	return NULL;
}

void AMixerParser::BuildOptions(AMixerOptions* Options, char* Line)
{
	if(Line == NULL) 
		return;
	if(strlen(Line) == 0)
		return;

	char* MixerName = strtok(Line, "'");

	MixerName = strtok(NULL, "'");

	std::cout<<MixerName<<std::endl;

}

void AMixerParser::GetControlLowerAndUpper(char* Line, int& LowerBound, int& UpperBound)
{
	char* Token = strtok (Line, " ");
	Token = strtok(NULL, " ");
	Token = strtok(NULL, " ");
	sscanf(Token, "%d", &LowerBound);
	Token = strtok(NULL, " ");
	Token = strtok(NULL, " ");
	sscanf(Token, "%d", &UpperBound);
}

void AMixerParser::GetControlValue(char* Line, int& Value)
{
	char* Token = strtok (Line, " ");
	Token = strtok(NULL, " ");
	Token = strtok(NULL, " ");
	Token = strtok(NULL, " ");
	sscanf(Token, "%d", &Value);
}



bool AMixerParser::GetControlLimits(char* Line, int& LowerBound, int& UpperBound, int& Value)
{
	int ParseMode = 0;
	//That line may content the Lower/Upper bound information
	if(strstr(Line, "Limits: ") != NULL)
		ParseMode = 1;

	//That line may content the current control value
	if(strstr(Line, "Front Left: Playback ") != NULL)
		ParseMode = 2;

	if (!ParseMode)
		return false;

	switch(ParseMode) {
	case 1:
		GetControlLowerAndUpper(Line, LowerBound, UpperBound);
		break;
	case 2:
		GetControlValue(Line, Value);
		break;
	}

	return true;
}

bool AMixerParser::GetControlOptions(char* ControlName, int& LowerBound, int& UpperBound, int& Value)
{
	char FileName[32];
#ifdef WIN32
	strcpy(FileName, "mixerlimits.txt");
#else
	strcpy(FileName, "/tmp/mixerlimits.txt");

	char Command[1024];
	sprintf (Command, "amixer sget %s > %s", ControlName, FileName);
	system(Command);
#endif

	char Buffer[1024];
	std::ifstream FInput;
	FInput.open(FileName);
	if(!FInput)
		return false;

	while(!FInput.eof())
	{
		FInput.getline(Buffer, 1024);
		GetControlLimits(Buffer, LowerBound, UpperBound, Value);	
	}

	FInput.close();
	return true;
}
