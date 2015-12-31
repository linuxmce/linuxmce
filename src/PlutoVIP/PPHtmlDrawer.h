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
#ifndef _PPHTMLDRAWER_H
#define _PPHTMLDRAWER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPHtmlDrawer.h : header file
//

// disable warning C4786: symbol greater than 255 character, okay to ignore
#pragma warning(disable : 4786)

#include "PPDrawManager.h"
#include <vector>
#include <map>

//Uncomments next line for MFC program
#define _USE_MFC

#ifdef _USE_MFC
	#define CPPString	CString //MFC program
#else
	#include "StdString.h"
	#ifdef _UNICODE
	#define CPPString	CStdStringW	//non-MFC program UNICODE
	#else
	#define CPPString	CStdStringA	//non-MFC program ANSI
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// CPPHtmlDrawer window

class CPPHtmlDrawer
{
// Construction
public:
	CPPHtmlDrawer();

// Attributes
public:

// Operations
public:
	//Drawing methods
	void  Draw(HDC hDC, LPCTSTR lpszHtml, LPPOINT lpPoint);
	void  PrepareOutput(HDC hDC, LPCTSTR lpszHtml, LPSIZE lpSize); //Prepares to draw the HTML string
	void  DrawPreparedOutput(HDC hDC, LPCRECT lpRect);

	void  EnableEscapeSequences(BOOL bEnable = TRUE);
	void  EnableOutput(BOOL bEnable = TRUE);
	void  SetDisabledColor(COLORREF color);
	
	//Shadow of the image
	void SetImageShadow(int nOffsetX, int nOffsetY, BYTE nDarkenPercent = 50, BOOL bGradient = TRUE, int nDepthX = 7, int nDepthY = 7);

	CPPString GetResCommandPrompt(UINT nID, UINT nNumParam = 0);

	//Functions for the styles
	void SetTextStyle(LPCTSTR lpszStyleName, LPCTSTR lpszStyleValue);
	void SetCssStyles(LPCTSTR lpszCssString = NULL); //Sets the CSS styles
	void SetCssStyles(DWORD dwIdCssString, LPCTSTR lpszPathDll = NULL); //Sets the CSS styles
	LPCTSTR GetCssStyles(); //Returns the current CSS styles

	void OnLButtonDown(LPPOINT lpClient);
	BOOL OnSetCursor(LPPOINT lpClient);
	BOOL OnTimer(); //New timer count

	void SetHyperlinkCursor(HCURSOR hCursor = NULL); //Sets the cursor to be displayed when moving the mouse over a link. Specifying NULL will cause the control to display its default 'hand' cursor.
	HCURSOR GetHyperlinkCursor() const; //Returns the current link cursor.

	void SetCallbackHyperlink(HWND hWnd, UINT nMessage, LPARAM lParam = 0); //Sets the callback message: "Mouse over the link".
	void SetCallbackRepaint(HWND hWnd, UINT nMessage, LPARAM lParam = 0); //Sets the callback message: "Please repaint me".

	//Functions for images
	void SetImageList(UINT nIdBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255));
	void SetImageList(HBITMAP hBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255));

	void LoadResourceDll(LPCTSTR lpszPathDll, DWORD dwFlags = 0); //Sets the path to the resource's DLL
	void SetResourceDll(HINSTANCE hInstDll = NULL); //Sets the handle of the loaded resource's DLL

	CPPDrawManager * GetDrawManager();

	static short GetVersionI()		{return 0x10;}
	static LPCTSTR GetVersionC()	{return (LPCTSTR)_T("1.0");}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPPHtmlDrawer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPPHtmlDrawer();

protected:
	enum{	ALIGN_LEFT = 0,
			ALIGN_CENTER,
			ALIGN_RIGHT
		};
	
	enum{	ALIGN_TOP = 0,
			ALIGN_VCENTER,
			ALIGN_BOTTOM,
			ALIGN_BASELINE
		};

	enum{	LINK_NONE = 0,
			LINK_HREF,
			LINK_MESSAGE
		};

	enum{	TEXT_TRANSFORM_NONE = 0,
			TEXT_TRANSFORM_UPPERCASE,
			TEXT_TRANSFORM_LOWERCASE,
			TEXT_TRANSFORM_CAPITALIZE
		};

	enum{	BORDER_STYLE_NONE = 0,
			BORDER_STYLE_SOLID,
			BORDER_STYLE_DOTTED,
			BORDER_STYLE_DASHED,
			BORDER_STYLE_DOUBLE,
		};

	enum {	TAG_NONE = 0,
			TAG_BOLD,
			TAG_ITALIC,
			TAG_UNDERLINE,
			TAG_STRIKEOUT,
			TAG_FONT,
			TAG_HLINE,
			TAG_NEWLINE,
			TAG_TABULATION,
			TAG_LEFT,
			TAG_CENTER,
			TAG_RIGHT,
			TAG_BASELINE,
			TAG_TOP,
			TAG_VCENTER,
			TAG_BOTTOM,
			TAG_BITMAP,
			TAG_ICON,
			TAG_IMAGELIST,
			TAG_STRING,
			TAG_NEWSTYLE,
			TAG_SPAN,
			TAG_HYPERLINK
		};

#pragma pack(1)
	typedef struct _STRUCT_TAGPROP
	{
		DWORD dwTagIndex;	// The hot rect of the hyperlink
		CPPString strTagName;	// The type of the hyperlink
	} STRUCT_TAGPROP;
#pragma pack()

#pragma pack(1)
	typedef struct _STRUCT_ANIMATION
	{
		int nIndex;		//The current index of the image
		int nMaxImages; //The max images in the bitmap
		int nTimerCount;//The current time position
		int nSpeed;		//The speed of animation
	} STRUCT_ANIMATION;
#pragma pack()

#pragma pack(1)
	typedef struct _STRUCT_HYPERLINK
	{
		CRect rcArea;		// The hot rect of the hyperlink
		int nTypeLink;		// The type of the hyperlink
		int nIndexLink;		// The index of the hyperlink
		CPPString sHyperlink; // The hyperlink
	} STRUCT_HYPERLINK;
#pragma pack()
	
#pragma pack(1)
	typedef struct _STRUCT_CHANGESTYLE 
	{
		CPPString strTag;		//The name of the last opened tag
		
		//Font
		int  nSizeFont;		//The height of the logic font
		int	 nWeightFont;	//The weight of the logic font
		BOOL bItalicFont;	//Is italic logic font?
		BOOL bUnderlineFont;//Is underline logic font?
		BOOL bStrikeOutFont;//Is strikeout logic font?
		BOOL bOverlineFont; //Is overline logic font?
		CPPString sFaceFont;  //The face name of the logic font
		
		//Color		
		COLORREF crText;	//The foreground color 
		COLORREF crBkgnd;	//The background color (also begin for the gradient)
		COLORREF crBorderLight;	//The border color
		COLORREF crBorderDark;	//The border color
		COLORREF crMidBkgnd;//The middle background color
		COLORREF crEndBkgnd;//The end background color

		//Fill
		int  nBkMode;		//The background mode for the text (TRANSPARENT, OPAQUE)
		int  nFillBkgnd;	//The fill effect of the background
		CPPString strNameResBk;

		//Align
		int  nHorzAlign;	//The horizontal align
		int  nVertAlign;	//The vertical align
		
		//Border
		int  nBorderStyle;	//The border style
		int  nBorderWidth;	//The width of the border

		//Text
		int  nTextTransform;//Transformation of the text (NONE, UPPERCASE, LOWERCASE, CAPITALIZE)

		//Margins
		int nMarginLeft;
		int nMarginRight;
		int nMarginTop;
		int nMarginBottom;
		
		//Padding
		int nPaddingLeft;
		int nPaddingRight;
		int nPaddingTop;
		int nPaddingBottom;
		
		//Hyperlink
		int  nTypeLink;		//The type of the link (NONE, HREF, MESSAGE)
		CPPString sHyperlink; //The additional parameter for the link
	} STRUCT_CHANGESTYLE; 
#pragma pack()
	
#pragma pack(1)
	typedef struct _STRUCT_IMAGE
	{
		int			nIndexImageList;//image's index of the image list
		int			nIdRes;			//ID resource from app
		int			nIdDll;			//ID resource from dll
		int			nHandle;		//handle of the resource
		int			cx;				//horizontal size of image
		int			cy;				//vertical size of image
		int			nWidth;			//width of image
		int			nHeight;		//height of image
		int         nSpeed;			//speed for animation
		UINT		nStyles;		//styles of image
		UINT		nHotStyles;		//hot styles of image
		BOOL        bUseMask;		//
		BOOL		bPercentWidth;
		BOOL		bPercentHeight;
		COLORREF	crMask;			//color of mask
		CPPString	strSrcFile;		//path on the source file
		CPPString   strPathDll;		//path on the resource dll
	} STRUCT_IMAGE;
#pragma pack()
	
#pragma pack(1)
	typedef struct _STRUCT_CALLBACK
	{
		HWND		hWnd;			/* Дескриптор окна, принимающего сообщение */
		UINT		nMessage;		// Message identifier
		WPARAM		wParam;
		LPARAM		lParam;
	} STRUCT_CALLBACK;
#pragma pack()

#pragma pack(1)
	typedef struct _STRUCT_CELLPARAM
	{
//		int  rowspan;
//		int  colspan;
		UINT uBorder;
		int  iFillBkgnd;
		UINT uMarginCx;
		UINT uMarginCy;
		UINT uBorderStyle;
		UINT uVAlign;
		UINT uAlign;
		COLORREF clrBorder;
		COLORREF clrBkgnd;
		COLORREF clrMidBkgnd;
		COLORREF clrEndBkgnd;
	} STRUCT_CELLPARAM;
#pragma pack()

#pragma pack(1)
	typedef struct _STRUCT_HTMLLINE
	{
		int  nWidthLine;
		int  nHeightLine;
		int  nDescentLine;
		int  nAddPercentWidth;
		int  nHorzAlign;
	} STRUCT_HTMLLINE;
#pragma pack()

#pragma pack(1)
	typedef struct _STRUCT_CELL
	{
		int   nColSpan; //-1 = Cell isn't used, >0 - How much columns was spaned
		int   nRowSpan; //-1 = Cell isn't used, >0 - How much rows was spaned
		CSize szText;
		CSize szCell;
	} STRUCT_CELL;
#pragma pack()

	STRUCT_CALLBACK	m_csCallbackRepaint; //Callback for repaint HTML drawer
	STRUCT_CALLBACK	m_csCallbackLink; //Callback for hyperlink message
	STRUCT_CHANGESTYLE m_defStyle;
	STRUCT_HTMLLINE m_hline;

	CPPDrawManager m_drawmanager;
	
	//Values of the system context
	CImageList m_ImageList;
	CSize m_szImageList;

	HINSTANCE m_hInstDll;
	BOOL m_bFreeInstDll;

	HCURSOR m_hLinkCursor;
	HFONT m_hOldFont;
	int m_nOldBkMode;
	COLORREF m_crOldText;
	COLORREF m_crOldBk;

	BOOL m_bIsCalculate; //TRUE for a prepare mode, FALSE for an output mode
	BOOL m_bIsEnable; //TRUE for fullcolor output, FALSE for disabled output
	COLORREF m_crDisabled;
//	SIZE m_szOutput; // Output size
	RECT m_rcOutput; //Output rectangle
//	POINT m_ptOutput; //Output coordinates
	HDC m_hDC; //Device context to output or to prepare
	CPPString m_csHtmlText; //String to output

//	COLORREF m_clrShadow;

	int   m_nCurLine;   // The current drawing line
	int   m_nCurTableIndex; //The current index of the table
	int   m_nNumCurTable; //The number of the current table
//	RECT  m_rect; //
//	int m_nLineHeight; //The height of the current line
//	int m_nLineDescent;
	int m_nHoverIndexLink; //The index of the link under the mouse
	int m_nCurIndexLink;
	int m_nCurIndexAni; //The index of the animation
	BOOL m_bLastValueIsPercent;
	BOOL m_bEnableEscapeSequences; // 

	//Shadow of the image
	BOOL m_bGradientShadow;
	SIZE m_szOffsetShadow;
	SIZE m_szDepthShadow;
	BYTE m_nDarkenShadow;
	COLORREF m_crShadow;

	TEXTMETRIC m_tm;

	LOGFONT m_lfDefault; //Default font
	HFONT m_hFont;

	//Wrapper string
	CPPString m_strPrefix; //Prefix string 
	CPPString m_strPostfix; //Postfix string
	CPPString m_strCssStyles;

	//Vectors
	typedef std::vector<STRUCT_HTMLLINE> vecHtmlLine;
	vecHtmlLine m_arrHtmlLine;

	//Vector of the stack
	typedef std::vector<STRUCT_CHANGESTYLE> arrStack;
	arrStack m_arrStack;

	//Vector of the hyperlinks
	typedef std::vector<STRUCT_HYPERLINK> arrLink;
	arrLink m_arrLinks;

	typedef std::vector<STRUCT_ANIMATION> arrAni;
	arrAni m_arrAni;

	//Map of the colors by name
	typedef std::map<CPPString, COLORREF> mapColors;
	typedef std::map<CPPString, COLORREF>::iterator iterMapColors;
	mapColors m_mapColors;

	//Map of the styles
	typedef std::map<CPPString, CPPString> mapStyles;
	typedef std::map<CPPString, CPPString>::iterator iter_mapStyles;
	mapStyles m_mapStyles;

	//Cells of Table
	typedef std::vector<STRUCT_CELL> vecCol;
	typedef std::vector<vecCol> vecTable;
	vecTable m_arrTable;

	//Dimensions of the columns and rows
	typedef std::vector<int> vecSize;
	typedef std::vector<vecSize> vecRowCol;
	vecRowCol m_arrTableSizes;

	//Map of the colors by name
	typedef std::map<CPPString, STRUCT_TAGPROP> mapTags;
	typedef mapTags::iterator iterMapTags;
	mapTags m_mapTags;

protected:
	void SetListOfTags(); //Fill a map of tags
	void AddTagToList(LPCTSTR lpszName, DWORD dwTagIndex, LPCTSTR lpszFullName); //Add tag to the list of tags
	DWORD GetTagFromList(LPCTSTR lpszName, CPPString & strFullName, BOOL & bCloseTag); //Get tag from the list

	//The resource's methods
	HICON GetIconFromResources(DWORD dwID, int nWidth = 0, int nHeight = 0) const; //Load an icon from the app resources
	HICON GetIconFromFile(LPCTSTR lpszPath, int nWidth = 0, int nHeight = 0) const; //Load an icon from the file
	HICON GetIconFromDll(DWORD dwID, int nWidth = 0, int nHeight = 0, LPCTSTR lpszPathDll = NULL) const; //Load an icon from the dll resources
	HBITMAP GetBitmapFromResources(DWORD dwID) const; //Load a bitmap from the app resources
	HBITMAP GetBitmapFromFile(LPCTSTR lpszPath) const; //Load a bitmap from the file
	HBITMAP GetBitmapFromDll(DWORD dwID, LPCTSTR lpszPathDll = NULL) const; //Load a bitmap from the dll resources
	CPPString GetStringFromResource(DWORD dwID) const; //Load a string from the app resources
	CPPString GetStringFromDll(DWORD dwID, LPCTSTR lpszPathDll = NULL) const; //Load a string from the dll resources

	//The drawing methods
	void DrawHtml (LPSIZE lpSize, LPCRECT lpRect); //Draws the HTML text on device context or gets the size of the output area.
	void DrawHtmlTable (LPCTSTR lpszHtml, LPCRECT lpRect, LPSIZE lpSize); //Draws the HTML table on device context or gets the size of the output area.
	void DrawHtmlRow(LPCTSTR lpszHtml, LPCRECT lpRect, LPSIZE lpSize); //Draws the HTML row of the table
	void DrawHtmlCell(LPCTSTR lpszHtml, LPCRECT lpRect, LPSIZE lpSize); //Draws the HTML cell of the table
	void DrawHtmlString (LPCTSTR lpszHtml, LPCRECT lpRect, LPSIZE lpSize); //Draws the HTML string on device context or gets the size of the output area.

//public:
	//The methods
	void SetDefaultCssStyles();
	void SetDefaultCursor();
	LPLOGFONT GetSystemToolTipFont() const; //Gets the system logfont

	CPPString SearchBodyOfTag(CPPString & str, CPPString & strTag, int & nIndex); //Search a tag's body
	CPPString SearchPropertyOfTag(CPPString & str, int & nIndex); //Search a name or a property of a tag
	CSize AnalyseCellParam(CPPString & strTag, _STRUCT_CHANGESTYLE & cs);
	void  AnalyseImageParam(CPPString & strTag, _STRUCT_IMAGE & si);
	BOOL  IsImageWithShadow(_STRUCT_IMAGE & si);

	//Functions for hyperlink
	int PtInHyperlink(LPPOINT lpPoint);
	void JumpToHyperlink(int nLink);
	void StoreHyperlinkArea(int left, int top, int right, int bottom);
	HINSTANCE GotoURL(LPCTSTR url, int showcmd = SW_SHOW);
	LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);

	//Functions for notify
	void CallbackOnClickHyperlink(LPCTSTR sLink);
	void CallbackOnRepaint(int nIndexLink);

	//Running tag
	int  HorizontalAlign(int x);
	int  VerticalAlignText(int y, int nHeight);
	int  VerticalAlignImage(int y, int nHeight);
	void UpdateContext();
	BOOL StoreRestoreStyle(BOOL bRestore);
	void Tag_NewLine(LPPOINT lpPoint, int nNum, LPSIZE lpSize);
	void Tag_Tabulation(LPPOINT lpPoint, int nNum);
	int  InitNewLine(int x);

	void SelectNewHtmlStyle(LPCTSTR lpszNameStyle, STRUCT_CHANGESTYLE & cs);

	CSize GetTableDimensions(CPPString str); //Gets dimensions of the table
	void  SearchEndOfTable(CPPString & str, int & nIndex); //Searching end of the table

	//Functions for the map of the styles
	void SetTableOfColors();
	void SetColorName(LPCTSTR lpszColorName, COLORREF color);
	COLORREF GetColorByName(LPCTSTR lpszColorName, COLORREF crDefColor = RGB(0, 0, 0));

	BOOL SearchTag(CPPString & str, int & nIndex, CPPString strTag);
	BOOL GetIndexNextAlphaNum(CPPString & str, int & nIndex, BOOL bArithmetic = FALSE);
	BOOL GetBeginParameter(CPPString & str, int & nIndex, TCHAR chSeparator = _T(':'));
	TCHAR GetIndexNextChars(CPPString & str, int & nIndex, CPPString strChars);
	TCHAR GetIndexNextNoChars(CPPString & str, int & nIndex, CPPString strChars);
	CPPString GetParameterString(CPPString & str, int & nIndex, TCHAR chBeginParam = _T(':'), CPPString strSeparators = _T(";"));
	CPPString GetNameOfTag(CPPString & str, int & nIndex);

	//Functions for the map of the styles
	LPCTSTR GetTextStyle(LPCTSTR lpszStyleName);
	void RemoveTextStyle(LPCTSTR lpszStyleName);
	void AddToTextStyle(LPCTSTR lpszStyleName, LPCTSTR lpszAddStyle);
	void UnpackTextStyle(CPPString strStyle, _STRUCT_CHANGESTYLE & cs);

	//Functions for analyzing parameters
	void SetDefaultStyles(_STRUCT_CHANGESTYLE & cs);
	BOOL GetStyleFontStyle(CPPString & str, BOOL bDefault);
	int  GetStyleFontWeight(CPPString & str, int nDefault);
	int  GetStyleHorzAlign(CPPString & str, int nDefault);
	int  GetStyleVertAlign(CPPString & str, int nDefault);
	COLORREF GetStyleColor(CPPString & str, COLORREF crDefault);
	int  GetStyleTextTransform(CPPString & str, int nDefault);
	CPPString GetStyleString(CPPString str, CPPString strDefault);
	void GetStyleFontShortForm(CPPString & str);
	UINT GetStyleImageShortForm(CPPString & str);
	int GetStyleBkgndEffect(CPPString & str, int nDefault);
	
	void StyleTextDecoration(CPPString & str, _STRUCT_CHANGESTYLE & cs);
	int StyleBorderWidth(CPPString & str, int Default);
	int StyleBorder(CPPString & str, int nDefault);

	//Get
	int GetLengthUnit(CPPString & str, int nDefault, BOOL bFont = FALSE);
	BOOL IsPercentableValue(CPPString & str);
	int GetTableWidth(CPPString & str, int nClientWidth, int nMinWidth, BOOL bSet = FALSE);

	//Drawing
	void DrawBackgroundImage(HDC hDC, int nDestX, int nDestY, int nWidth, int nHeight, CPPString strNameImage);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
