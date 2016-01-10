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
// SBDestination.h: interface for the CSBDestination class.
//
//////////////////////////////////////////////////////////////////////
//
// Copyright 1998 Scott D. Killen
//
//////////////////////////////////////////////////////////////////////

#ifndef __SBDESTINATION_H__
#define __SBDESTINATION_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BrowseForFolder.h"

class CSBDestination : public CBrowseForFolder  
{
public:
	CSBDestination(const HWND hParent , const int nTitleID = 0);
	CSBDestination(const HWND hParent = NULL, const CString Title = "");
	
	~CSBDestination();

	void SetInitialSelection(const CString& strPath);

	void OnInit() const;
	void OnSelChanged(const LPITEMIDLIST pidl) const;

private:
	CString m_strInitialSelection;
};

#endif // __SBDESTINATION_H__
