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
// SBDestination.cpp: implementation of the CSBDestination class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SBDestination.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSBDestination::CSBDestination(const HWND hParent, const int nTitleID)
	: CBrowseForFolder(hParent, NULL, nTitleID)
{

}

CSBDestination::CSBDestination(const HWND hParent, const CString Title)
	: CBrowseForFolder(hParent,NULL,Title)
{

}

CSBDestination::~CSBDestination()
{

}

void CSBDestination::SetInitialSelection(const CString & strPath)
{
	m_strInitialSelection = strPath;
}

void CSBDestination::OnInit() const
{
	SetSelection(m_strInitialSelection);
	SetStatusText(m_strInitialSelection);
}

void CSBDestination::OnSelChanged(const LPITEMIDLIST pidl) const
{
	CString strBuffer;
	if (SHGetPathFromIDList(pidl, strBuffer.GetBuffer(MAX_PATH)))
		strBuffer.ReleaseBuffer();
	else
		strBuffer.ReleaseBuffer(0);
	SetStatusText(strBuffer);
}
