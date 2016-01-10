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
// VIPDesign.h : main header file for the VIPDesign application
//
#pragma once

#include "resource.h"       // main symbols

// CVIPDesignApp:
// See VIPDesign.cpp for the implementation of this class
//

class CVIPDesignApp : public CWinApp
{
public:
	CVIPDesignApp();

	pthread_t m_pthread_emulator;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CVIPDesignApp theApp;