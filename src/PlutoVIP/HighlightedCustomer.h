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
#if !defined(AFX_HIGHLIGHTEDCUSTOMER_H__3AEA70C2_98D9_434F_9970_9CFF0286F29B__INCLUDED_)
#define AFX_HIGHLIGHTEDCUSTOMER_H__3AEA70C2_98D9_434F_9970_9CFF0286F29B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HighlightedCustomer.h : header file
//


#include "Picture.h"
#include "Customer.h"
#include "Seperator.h"
#include "StaticPicture.h"
#include "PPHtmlStatic.h"
#include "MyHyperlink.h"
//#include "VIPShared/PlutoConfig.h"

#define DEFAULT_TIMER			0
#define APPEAR_TIMER			1
#define APPEAR_TIMER_DESTROY	400

/////////////////////////////////////////////////////////////////////////////
// CHighlightedCustomer dialog
typedef struct tagPosition
{
	int iDirection;
	POINT pPosition;		//upper left corner of selected item in list control
	POINT WidthHeight;		//width and height of CControlDlg
} Position;

class CHighlightedCustomer : public CDialog
{
// Construction
public:
	void ReadCustomerComment();
	void AdjustDialog();
	void SetPositionInfo(Position PosInfo);
	void SetCustomer(Customer* pCustomer);
	void KillMe(int iTimer);
	void CommitChanges();
	void ResetTimer() { KillTimer(DEFAULT_TIMER);}
	CHighlightedCustomer(CWnd* pParent = NULL);   // standard constructor

	const Customer *GetCustomer() const { return m_pCustomer; }

	CButton m_ChangeState;
	CButton m_Task;

	// Dialog Data
	//{{AFX_DATA(CHighlightedCustomer)
	enum { IDD = IDD_HIGHLIGHT };
	CMyHyperLink	m_email;
	CPPHtmlStatic	m_Purchase;
	CPPHtmlStatic	m_Visit;
	CStaticPicture	m_Picture;
	CSeperator		m_NickName;
	CString			m_Address;
	CString			m_Comment;
	CString			m_FullName;
	CString			m_City;
	CString			m_State;
	CString			m_Zip;
	int		m_StoreCustomerID;
	CString	m_Bdate_Gender;
	CString	m_Extra;
	CString	m_Phones;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHighlightedCustomer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL CommentsIsChanged;
	Position PositionInfo;
	int m_Timer;
	Customer *m_pCustomer;

	// Generated message map functions
	//{{AFX_MSG(CHighlightedCustomer)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTask();
	afx_msg void OnChangeState();
	afx_msg void OnCustomerNew();
	afx_msg void OnCustomerRecent();
	afx_msg void OnCustomerActive();
	afx_msg void OnTaskChargeCustomer();
	afx_msg void OnTaskSendImage();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAction();
	afx_msg void OnActionAddVisit();
	afx_msg void OnActionRedeemPurchase();
	afx_msg void OnActionAddPurchase();
	afx_msg void OnActionRedeemVisit();
	afx_msg void OnChangeComment();
	afx_msg void OnDestroy();
	afx_msg void OnChangeStoreCustomerId();
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIGHLIGHTEDCUSTOMER_H__3AEA70C2_98D9_434F_9970_9CFF0286F29B__INCLUDED_)
