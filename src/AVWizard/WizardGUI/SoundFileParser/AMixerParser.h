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
#ifndef AMIXER_PARSER_H
#define AMIXER_PARSER_H

#include "AMixerOptions.h"

class AMixerParser
{
	void AMixerParser::GetControlLowerAndUpper(char* Line, int& LowerBound, int& UpperBound);
	bool GetControlLimits(char* Line, int& LowerBound, int& UpperBound, int& Value);

	void GetControlValue(char* Line, int& Value);

public:
	AMixerParser();
	~AMixerParser();

	AMixerOptions* ParseConfig();

	void BuildOptions(AMixerOptions* Options, char* Line);

	bool GetControlOptions(char* ControlName, int& LowerBound, int& UpperBound, int& Value);
};

#endif