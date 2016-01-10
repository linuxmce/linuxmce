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
#ifndef __LOGGER_H__
#define __LOGGER_H__
//----------------------------------------------------------------------------------------------
#include <utf.h>
#include <coeccntx.h>
#include <coemain.h>

#include "VIPShared/VIPIncludes.h"
//----------------------------------------------------------------------------------------------
//#define ENABLE_LOG
//----------------------------------------------------------------------------------------------
// CONSTANTS
// UID of view
const TUid KCPlutoLoggerId = {102};
//----------------------------------------------------------------------------------------------
#define LOG(s) { ((SymbianLogger *)CCoeEnv::Static(KCPlutoLoggerId))->Write(s); }
#define LOGN(s) { ((SymbianLogger *)CCoeEnv::Static(KCPlutoLoggerId))->Write(s, false); }

class SymbianLogger : public CCoeStatic
{
	RFile file;
	RFs   aFs;

public: 

	SymbianLogger(string FileName, TUid aUid, TScope scop = EThread);
	~SymbianLogger();

	void Write(int Value,  bool bAddTime = true);
	void Write(char *log,  bool bAddTime = true);
	void Write(string log, bool bAddTime = true);
};
//----------------------------------------------------------------------------------------------
#endif __LOGGER_H__