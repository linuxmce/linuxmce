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
/**************************************************************************************/
/* CXSBrowseFolder                                                                    */
/**************************************************************************************/
/* This is a simple class to wrap the SHBrowseForFolder function.                     */
/**************************************************************************************/
/* Written by Dana Holt, Xenos Software                                             */
/* http://www.xenossoftware.com                                                       */
/* This class is provided as-is, and carries no warranty or guarantee of any kind.    */
/* Use at your own risk.                                                              */
/**************************************************************************************/

#include "shlobj.h"

#pragma once

class CXSBrowseFolder
{
public:

	enum retCode {

		RET_CANCEL = 0,
		RET_NOPATH,
		RET_OK

	};

public:
	CXSBrowseFolder(void);
	~CXSBrowseFolder(void);
protected:
	// Holds the current style
	DWORD m_style;
public:
	// Modifies the current style
	DWORD ModifyStyle(DWORD add, DWORD remove = 0);
	// Returns the current style
	DWORD GetStyle(void);
	// Displays the dialog
	CXSBrowseFolder::retCode Show(HWND parent, LPSTR pathBuffer);
	// Set the title of the dialog
	void SetTitle(LPSTR title);
protected:
	// Buffer to receieve the display name of the selected object
	char m_displayName[MAX_PATH];
	// Root item to start browsing at
	LPITEMIDLIST m_root;
	// Text to display above the tree view control
	char m_title[MAX_PATH];
		
};
