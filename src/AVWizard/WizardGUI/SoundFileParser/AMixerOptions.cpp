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
#include "amixeroptions.h"

AMixerOptions::~AMixerOptions(void)
{
	std::vector <char*>::iterator Item = Options.begin();
	while(Item < Options.end())
	{
		free(*Item);
		Item++;
	}

}

int AMixerOptions::AddOption(char* OptionName)
{
	size_t LenString = strlen(OptionName);
	char* CopyString = (char*) malloc (LenString+1);
	strncpy(CopyString, OptionName, LenString+1);
	Options.push_back(CopyString);

	return -1;
}

int AMixerOptions::Locate(char* OptionName)
{
	std::vector <char*>::iterator Item = Options.begin();
	int Position = 0;
	while(Item < Options.end())
	{
		if (strcmp(OptionName, *Item) == 0)
			return Position;

		Position++;
		Item++;
	}
	return -1;
}