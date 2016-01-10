#if !defined(AFX_DLGOPTIONS_H__218AAA01_FAC3_11D5_8DD7_00E07D8144D0__INCLUDED_)
#define AFX_DLGOPTIONS_H__218AAA01_FAC3_11D5_8DD7_00E07D8144D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOptions.h : header file
//

#include "xTargetButton.h"

/////////////////////////////////////////////////////////////////////////////
// DlgOptions dialog
class DlgOptions : public CDialog
{
// Construction
public:
	DlgOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgOptions)
	enum { IDD = IDD_OPTIONS };
	CxTargetButton	m_ok;
	CxTargetButton	m_canc;
	BYTE	m_quality;
	long	m_xres;
	DWORD	m_yres;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgOptions)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPTIONS_H__218AAA01_FAC3_11D5_8DD7_00E07D8144D0__INCLUDED_)
