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
// LogMsg.h: interface for the CLogMessage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LOGMSG_H__
#define __LOGMSG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CLogMessage  
{
public:
	BOOL WriteError (const CString& strMessage);
	CLogMessage ();
	virtual ~CLogMessage();

private:
    CStdioFile m_LogFile;
    BOOL  m_bSuccess;
};

#endif // !defined(AFX_LOGMSG_H__AB5CEE07_A955_11D1_B258_006097960BB7__INCLUDED_)
