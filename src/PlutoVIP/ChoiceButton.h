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
/******************************************************************************/
/**
 * @file ChoiceButton.h
 *
 * @brief Header for CChoiceButton
 *
 * @version 1.2
 *
 * @date 02.06.2004
 *
 * Provides a Button Object with a Picture on it, and with a DropDown menu with Choices.
 * The Picture & DropDown menu are optional. Unless a Picture or Menu Item is added, the
 * Button shows up as normal button.
 *
 * The Button can handle default status and default keyboard handling (space,return)
 *
 * If menu items are added, the actual chosen one becomes the buttons text.
 *
 ******************************************************************************/

#if !defined(AFX_CHOICEBUTTON_H__A709F674_6EA8_4C55_B89D_ACEDB96F2797__INCLUDED_)
#define AFX_CHOICEBUTTON_H__A709F674_6EA8_4C55_B89D_ACEDB96F2797__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// size pre-defines
#define CHOICE_DROP_WIDTH 1 //verschiebung des textes/bild/menupfeil beim drücken
#define CHOICE_MENU_WIDTH 20 //breite des meun buttons
#define CHOICE_BORDER_SPACING 4 //abstand des bildes/textes von den rändern des buttons
#define CHOICE_PICTEXT_SPACING 4 //abstandd des button textes vom bild
#define CHOICE_SIZEDEFAULT_SPACING 10 //default spacing, added to the width calculated by SizeToContents

/////////////////////////////////////////////////////////////////////////////
// menu command id base
#define CHOICE_MENUID WM_USER+0x2000

/////////////////////////////////////////////////////////////////////////////
// CChoiceButton window


/******************************************************************************/
/**
* @class CChoiceButton
*
* @brief Provides a Button with a Picture and/or DropDown menu
*
* @version (optional)
*
* see file comment
*
*******************************************************************************/
class CChoiceButton : public CButton
{
// Construction
public:
	CChoiceButton();

// Attributes
public:

// Operations
public:
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle,	const RECT& rect, CWnd* pParentWnd, UINT nCmdID);
	BOOL Create(UINT nStrID, DWORD dwStyle,	const RECT& rect, CWnd* pParentWnd, UINT nCmdID);
	BOOL Create(DWORD dwStyle,	const RECT& rect, CWnd* pParentWnd, UINT nCmdID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChoiceButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SizeToContents(UINT uSpacing=CHOICE_SIZEDEFAULT_SPACING);
	SIZE GetSize();
	bool IsPushed();
	BOOL DeleteMenuItem(UINT nItem);
	void SetPicTextSpacing(UINT nSpacing);
	void SetBorderSpacing(UINT nSpacing);
	void SetMenuButtonWidth(UINT nWidth);
	void SetDropWidth(UINT nDropWidth);
	bool SetPush(bool bPushed=true);
	UINT GetChoice();
	void SetChoice(UINT nMenuItem);
	bool EnableMenuItem(UINT nItem, bool bEnable);
	void AddMenuSeparator();
	int AddMenuItem(LPCTSTR  lpszNewItem);
	BOOL SetImage(UINT nResID, COLORREF crTrans, bool bResize=true);
	virtual ~CChoiceButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChoiceButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnSetStyle(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnSetState(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()

// Attributes
private:
	BOOL	m_bPushed;			///< internal flag for drawing the main button as pushed/not pushed
	BOOL	m_bMenuPushed;		///< internal flag for drawing the menu button as pushed/not pushed
	BOOL	m_bMenuEnabled;		///< internal flag if menu is enabled, false until at least one menu item is added
	BOOL	m_bDefault;			///< internal flag for drawing the button as default button or not (black border)
	BOOL	m_bFocus;			///< internal flag for drawing the ficus rectangle or not (dotted gray rect around text)

	CBitmap m_bmpImage;			///< the CBitmap object for the normal state picture
	CBitmap m_bmpImageDisabled;	///< the CBitmap object for the disabled state picture
	BITMAP  m_bmpInfo;			///< BITMAP info structure holding the size of the pictures
	BOOL	m_bImageLoaded;		///< internal flag for drawing the picture or not (if not loaded)

	UINT	m_nDropWidth;		///< contains the actual drop width
	UINT	m_nMenuButtonWidth;	///< contains the actual width of the menu button
	UINT	m_nBorderSpacing;	///< contains the actual spacing of the contents from the buttons border
	UINT	m_nPicTextSpacing;	///< contains the actual Picture-Text spacing
	CRect	m_rcMainRect;		///< contains the actual rectangle of the main button, set in DrawItem
	CRect	m_rcMenuRect;		///< contains the actual rectangle of the menu button, set in DrawItem

	CMenu	m_menuChoice;		///< the menu object for the dropdown menu
	UINT	m_nMenuChoice;		///< contains the actual chosen choice (the checked menu item, the buttons text)

// Operations
private:
	void DrawArrow(CDC* pDC,CPoint ptArrow, COLORREF crCol);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOICEBUTTON_H__A709F674_6EA8_4C55_B89D_ACEDB96F2797__INCLUDED_)
