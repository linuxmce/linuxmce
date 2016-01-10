/*
 Logger
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "Logger.h"
//----------------------------------------------------------------------------------------------
SymbianLogger::SymbianLogger(string FileName, TUid aUid, TScope scop/*=EThread*/) : CCoeStatic(aUid, scop)
{
#ifdef ENABLE_LOG

	aFs.Connect();

	if(KErrNotFound == file.Open(aFs, FileName.Des(), EFileStream | EFileWrite))
	{
		file.Create(aFs, FileName.Des(), EFileStream | EFileWrite);
	}
	else
	{
		int aPos = 0;
		file.Seek(ESeekEnd, aPos);
	}

	Write("\n", false);
	Write("Starting log...\n");

#endif
}
//----------------------------------------------------------------------------------------------
SymbianLogger::~SymbianLogger()
{
#ifdef ENABLE_LOG
	file.Close();
	aFs.Close();
#endif
}
//----------------------------------------------------------------------------------------------
void SymbianLogger::Write(int Value, bool bAddTime /*= true*/)
{
	string s = string::IntegerToString(Value);
	Write(s, bAddTime);
}
//----------------------------------------------------------------------------------------------
void SymbianLogger::Write(char *log, bool bAddTime /*= true*/)
{
	Write(string(log), bAddTime);
}
//----------------------------------------------------------------------------------------------
void SymbianLogger::Write(string log, bool bAddTime /*= true*/)
{
#ifdef ENABLE_LOG

	if(bAddTime)
	{
		//string time = string::IntegerToString(User::TickCount());
		//log = time + string(": \t") + log;
	}

#ifdef __WINS__
	HBufC8 *pBuf = HBufC8::NewL(log.length());
	TDes8 *log8 = &pBuf->Des();

	for(int i = 0; i < log.length(); i++)
		log8->Append(log[i]);
	
	file.Write(*log8);

	delete pBuf;
	pBuf = NULL;
#else
	file.Write(_L8(log.c_str()));
#endif //__WINS__

#endif //ENABLE_LOG
}
//----------------------------------------------------------------------------------------------