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
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//---------------------------------------------------------------------------
#include "GUIWizardUtils.h"
//---------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//---------------------------------------------------------------------------
#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#endif

/*static*/ int Utils::StringToInt32(const char* String)
{
	int Result;
	sscanf(String, "%d", &Result);
	return Result;
}

/*static*/ int Utils::StringToInt32(std::string String)
{
	int Result = 0;
	char* Text = (char*) String.c_str();
	sscanf(Text, "%d", &Result);
		
	return Result;
}

/*static*/ std::string Utils::Int32ToString(int Value)
{
	std::string Result;
	char Buffer[15];
	snprintf(Buffer, sizeof(Buffer), "%d", Value);

	Result = Buffer;
	return Result;
}

/*static*/ char* Utils::CloneStr(const char* String)
{
	char* Result = (char*)malloc(strlen(String)+1);
	strcpy(Result, String);
	return Result;	
}

/*static*/ TColorDesc* Utils::StringToColorDesc(std::string Text)
{
	int Value;
	sscanf(Text.c_str(), "%x", &Value);
	unsigned char R, G, B;
	B = Value & 255;
	Value >>= 8;
	G = Value & 255;
	Value >>= 8;
	R = Value & 255;
	TColorDesc* Result = new TColorDesc(R, G, B);
	return Result;	
}

/*static*/ char* Utils::CopyStr(const char* String, int StartPos, int Length)
{
	int StrLen = (int)strlen(String);
	if ((String == NULL) || (StrLen <= StartPos))
	{
		char* Result = (char*) malloc(1);
		Result[0] = 0;
		return NULL;
	}

	if (StartPos + Length >= StrLen)
		Length = StrLen - StartPos;

	char* Result = (char*) malloc (Length + 1);

	for(int Count = 0; Count < Length; Count++)
		Result[Count] = String[StartPos + Count];
	Result[Length] = 0;

	return Result;
}

/*static*/ char* Utils::AllTrim(const char* String)
{
    int StartSkip, EndSkip, Length = (int)strlen(String);

	for(StartSkip = 0; StartSkip < Length; StartSkip++)
		if (String[StartSkip] != ' ')
			break;

	if (StartSkip == Length) {

	}

	for(EndSkip = Length - 1; EndSkip > 0; EndSkip--)
		if (String[EndSkip] != ' ')
			break;

	char* Result = Utils::CopyStr(String, StartSkip, EndSkip - StartSkip+1);

    return Result;
}

void wizSleep(int Miliseconds)
{
#ifndef WIN32
	usleep(Miliseconds*1000);
#else
	Sleep(Miliseconds);
#endif
}
