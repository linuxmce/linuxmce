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
// transp_bth.h : main header file for the TRANSP_BTH DLL
//

#if !defined(AFX_TRANSP_BTH_H__9E79BBA1_FCE3_439C_B8E4_91C8EB48A6A4__INCLUDED_)
#define AFX_TRANSP_BTH_H__9E79BBA1_FCE3_439C_B8E4_91C8EB48A6A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTransp_bthApp
// See transp_bth.cpp for the implementation of this class
//

class CTransp_bthApp : public CWinApp
{
public:
	CTransp_bthApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransp_bthApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTransp_bthApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSP_BTH_H__9E79BBA1_FCE3_439C_B8E4_91C8EB48A6A4__INCLUDED_)
