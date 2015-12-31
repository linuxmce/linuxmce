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

#ifndef GUIWizardUtilsH
#define GUIWizardUtilsH
//---------------------------------------------------------------------------
#include <string>
#include "ColorDesc.h"

#ifdef WIN32
#define snprintf _snprintf 
#define snscanf _snscanf
#endif

class Utils
{
	public:
		static int StringToInt32(const char* String);
		static int StringToInt32(std::string String);
		static std::string Int32ToString(int Value);
		static char* CloneStr(const char* String);

		static char* CopyStr(const char* String, int StartPos, int Length);
		static char* AllTrim(const char* String);

		static TColorDesc* StringToColorDesc(std::string String);
};

void wizSleep(int Miliseconds);
//---------------------------------------------------------------------------
#endif
