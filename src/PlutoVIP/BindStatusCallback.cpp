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
// BindStatusCallback.cpp: implementation of the CBindStatusCallback class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindStatusCallback.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MyStl.h"
#include <shlwapi.h>                    // for StrFormatByteSize()

#include "PlutoVIPMain.h"
#include "UpdateDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern LONG g_fAbortDownload;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallback::CCallback() : m_bUseTimeout(FALSE), m_pDlg(NULL)
{
	m_HaveProgress=FALSE;
}

CCallback::~CCallback()
{
}

HRESULT CCallback::OnProgress ( ULONG ulProgress,   ULONG ulProgressMax,
                                ULONG ulStatusCode, LPCWSTR wszStatusText )
{
// Local variables are declared static so they don't have to be reallocated on
// the stack every time.  This is safe in this app since I know I'll only have
// one thread downloading.
static CString sIEStatusMsg;
static TCHAR   szCustomStatusMsg [256];
static TCHAR   szAmtDownloaded [256], szTotalSize [256];

    UNREFERENCED_PARAMETER(ulStatusCode);

    // Did the user hit the Stop button?
    if ( 0 != g_fAbortDownload )
       return E_ABORT;

    // Has the timeout period elapsed?
    if ( m_bUseTimeout  &&  CTime::GetCurrentTime() > m_timeToStop )
        return E_ABORT;

    // Use CString to convert IE's status message to a TCHAR string.
    if ( NULL != wszStatusText )
        {
        sIEStatusMsg = wszStatusText;
        }
    else
        {
        sIEStatusMsg.Empty();
        }

    // Make our own progress message - we'll show the amount downloaded and
    // the total file size (if known).

    StrFormatByteSize ( ulProgress, szAmtDownloaded, 256 );
    StrFormatByteSize ( ulProgressMax, szTotalSize, 256 );
    
    if ( 0 != ulProgressMax )
        {
        wsprintf ( szCustomStatusMsg, _T("Downloaded %s of %s"),
                   szAmtDownloaded, szTotalSize );
        }
    else
        {
        wsprintf ( szCustomStatusMsg, _T("Downloaded %s (total size unknown)"),
                   szAmtDownloaded );
        }

    // Report the progress back to the main window.

    if ( 0 != ulProgressMax && m_HaveProgress)
	{
		switch(m_DialogType) 
		{
			case DIALOG_TYPE_PLUTOVIPMAIN:
				((CPlutoVIPMain*) m_pDlg)->ProgressUpdate ( sIEStatusMsg, szCustomStatusMsg,
					int( 100.0 * ulProgress / ulProgressMax) );
				break;
			case DIALOG_TYPE_UPDATEDLG:
				((CUpdateDlg*) m_pDlg)->ProgressUpdate ( sIEStatusMsg, szCustomStatusMsg,
					int( 100.0 * ulProgress / ulProgressMax) );
				break;
		}
        
	}
    else
	{
		switch(m_DialogType) 
		{
		case DIALOG_TYPE_PLUTOVIPMAIN:
			((CPlutoVIPMain*) m_pDlg)->ProgressUpdate ( sIEStatusMsg, szCustomStatusMsg, 0 );
			break;
		case DIALOG_TYPE_UPDATEDLG:
			((CUpdateDlg*) m_pDlg)->ProgressUpdate ( sIEStatusMsg, szCustomStatusMsg, 0 );
			break;
		}
	}

    return S_OK;
}

void CCallback::HaveProgress(BOOL bProgress)
{
	m_HaveProgress=bProgress;
}

void CCallback::SetDialogType(UINT uiDialogType)
{
	m_DialogType=uiDialogType;
}	
