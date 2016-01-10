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
// LogMsg.cpp: implementation of the CLogMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogMessage::CLogMessage()
{
    if (m_LogFile.Open ("LOG.TXT", CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {   
        m_bSuccess = TRUE;
    }
    else
    {
        m_bSuccess = FALSE;
    }
}

CLogMessage::~CLogMessage()
{
    m_LogFile.Close ();
}

BOOL CLogMessage::WriteError(const CString & strMessage)
{
    TRY
    {
        if (m_bSuccess)
        {
            m_LogFile.WriteString (strMessage);
        }
        else
        {
            throw;
        }
    }
    CATCH (CFileException, e)
    {
        return FALSE;
    }
    END_CATCH

    return TRUE;
}
