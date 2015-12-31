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
#ifndef _STATIC_PICTURE_H_
#define _STATIC_PICTURE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticPicture.h : header file
//

#include "Picture.h"
/////////////////////////////////////////////////////////////////////////////
// CStaticPicture window

class CStaticPicture : public CStatic
{
// Construction
public:
	CStaticPicture();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticPicture)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetPicture(CString szPictureFile);
	virtual ~CStaticPicture();

	// Generated message map functions
protected:
	CBitmap m_Picture;
	//{{AFX_MSG(CStaticPicture)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _STATIC_PICTURE_H_
