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
#include "SoundUtils.h"

#include "AMixerParser.h"

AMixerOptions* GetCurentMixerOptions()
{
	AMixerParser* Parser = new AMixerParser();

	AMixerOptions* Result = Parser->ParseConfig();

	delete Parser;

	return Result;
}

bool GetControlLimits(char* ControlName, int& LowerBound, int& UpperBound, int& Value)
{
	if(ControlName == NULL)
		return false;

	if(strlen(ControlName) == 0)
		return false;

	AMixerParser* Parser = new AMixerParser();

	bool Result = Parser->GetControlOptions(ControlName, LowerBound, UpperBound, Value);

	delete Parser;

	return Result;
}

void SetControlLimits(char* ControlName, int Value)
{
#ifdef WIN32
	char Command[1024];
	sprintf(Command, "amixer sset Master Playback %s", Value);
	system(Command);
#endif
}
