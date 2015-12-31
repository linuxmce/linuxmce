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
// VIPDesignView.cpp : implementation of the CVIPDesignView class
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"
//#include "PlutoVIP/VR_ShowMenu.h"
#include "PlutoUtils/StringUtils.h"

#include "VIPDesign.h"

#include "VIPDesignDoc.h"
#include "VIPDesignView.h"

#include "Picture.h"
#include "ResetEmulatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVIPDesignView

IMPLEMENT_DYNCREATE(CVIPDesignView, CView)

BEGIN_MESSAGE_MAP(CVIPDesignView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_PAINT()
	ON_WM_KEYUP()
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CVIPDesignView construction/destruction

CVIPDesignView::CVIPDesignView() : RenderMenu()
{
	m_dc=NULL;
	m_hOriginalFont=0;
}

CVIPDesignView::~CVIPDesignView()
{
}

BOOL CVIPDesignView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CVIPDesignView drawing

void CVIPDesignView::OnDraw(CDC* /*pDC*/)
{
	CVIPDesignDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}


// CVIPDesignView printing

BOOL CVIPDesignView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVIPDesignView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVIPDesignView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CVIPDesignView diagnostics

#ifdef _DEBUG
void CVIPDesignView::AssertValid() const
{
	CView::AssertValid();
}

void CVIPDesignView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVIPDesignDoc* CVIPDesignView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVIPDesignDoc)));
	return (CVIPDesignDoc*)m_pDocument;
}
#endif //_DEBUG


// CVIPDesignView message handlers

void CVIPDesignView::OnPaint()
{
	CVIPDesignDoc *pDoc = GetDocument();
	SetCurrent(pDoc->m_pMenu,pDoc->m_pMenuCollection,pDoc->m_pMenuInput);
	if( pDoc->m_pImageStatic_Size && pDoc->m_pImageStatic_Data )
	{
		SetStatic(pDoc->m_pImageStatic_Type,pDoc->m_pImageStatic_Size,pDoc->m_pImageStatic_Data);
	}

	m_dc = new CPaintDC(this); // device context for painting
	DoRender();

	if( m_hOriginalFont )
	{
		HGDIOBJ hOldFont = m_dc->SelectObject(m_hOriginalFont);
		::DeleteObject(hOldFont);
		m_hOriginalFont=0;
	}

	delete m_dc;
}

void CVIPDesignView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
/*
#define BUTTON_Enter_CONST 5
#define BUTTON_Scroll_Up_CONST 6
#define BUTTON_Scroll_Down_CONST 7
#define BUTTON_Scroll_Enter_CONST 8
#define BUTTON_F1_CONST 10
#define BUTTON_F2_CONST 11
#define BUTTON_F3_CONST 12
#define BUTTON_F4_CONST 13
#define BUTTON_F5_CONST 14
#define BUTTON_Record_CONST 15
#define BUTTON_Phone_C_CONST 26
#define BUTTON_Phone_Pencil_CONST 27
#define BUTTON_Phone_Talk_CONST 28
#define BUTTON_Phone_End_CONST 29
#define BUTTON_Phone_Soft_left_CONST 30
#define BUTTON_Phone_Soft_right_CONST 31
#define BUTTON_Rept_Pound_CONST 106
#define BUTTON_Rept_Asterisk_CONST 105
#define BUTTON_Rept_Phone_Soft_right_CONST 104
#define BUTTON_Rept_Phone_Soft_left_CONST 103
#define BUTTON_Rept_Phone_End_CONST 102
#define BUTTON_Rept_Phone_Talk_CONST 101
#define BUTTON_Rept_Phone_Pencil_CONST 100
#define BUTTON_Rept_Phone_C_CONST 99
#define BUTTON_Rept_0_CONST 98
#define BUTTON_Rept_9_CONST 97
#define BUTTON_Rept_8_CONST 96
#define BUTTON_Rept_7_CONST 95
#define BUTTON_Rept_6_CONST 94
#define BUTTON_Rept_5_CONST 93
#define BUTTON_Rept_4_CONST 92
#define BUTTON_Rept_3_CONST 91
#define BUTTON_Rept_2_CONST 90
#define BUTTON_Rept_1_CONST 89 
*/

	unsigned char KeyCode = 0;

	switch(nChar)
	{
		case VK_UP:			KeyCode=BUTTON_Up_Arrow_CONST;			break;
		case VK_DOWN:		KeyCode=BUTTON_Down_Arrow_CONST;		break;
		case VK_RIGHT:		KeyCode=BUTTON_Right_Arrow_CONST;		break;
		case VK_LEFT:		KeyCode=BUTTON_Left_Arrow_CONST;		break;

		//'Y'
		case 0x59:			KeyCode=BUTTON_Phone_Talk_CONST;		break;

		//'N'
		case 0x4E:			KeyCode=BUTTON_Phone_End_CONST;			break;

		//'E'
		case 0x45: 
		case VK_RETURN:		KeyCode=BUTTON_Enter_CONST;				break;

		//'C'
		case 0x43:			KeyCode=BUTTON_Phone_C_CONST;			break;

		//'*'
		case 0x0B:			KeyCode=BUTTON_Asterisk_CONST;			break;

		//'#'
		case 0x0C:			KeyCode=BUTTON_Pound_CONST;				break;

		case 0x30:
		case VK_NUMPAD0:	KeyCode=BUTTON_0_CONST;					break;
		case 0x31:
		case VK_NUMPAD1:	KeyCode=BUTTON_1_CONST;					break;
		case 0x32:
		case VK_NUMPAD2:	KeyCode=BUTTON_2_CONST;					break;
		case 0x33:
		case VK_NUMPAD3:	KeyCode=BUTTON_3_CONST;					break;
		case 0x34:
		case VK_NUMPAD4:	KeyCode=BUTTON_4_CONST;					break;
		case 0x35:
		case VK_NUMPAD5:	KeyCode=BUTTON_5_CONST;					break;
		case 0x36:
		case VK_NUMPAD6:	KeyCode=BUTTON_6_CONST;					break;
		case 0x37:
		case VK_NUMPAD7:	KeyCode=BUTTON_7_CONST;					break;
		case 0x38:
		case VK_NUMPAD8:	KeyCode=BUTTON_8_CONST;					break;
		case 0x39:
		case VK_NUMPAD9:	KeyCode=BUTTON_9_CONST;					break;
	}

	KeyPressed(KeyCode);
}

void CVIPDesignView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	GetDocument()->UpdateListOfMenus();
}

void CVIPDesignView::Redraw()
{
	Invalidate();
}

void CVIPDesignView::MessageBox(const char *Message,const char *Title)
{
	CView::MessageBox(Message,Title);
}

bool CVIPDesignView::DrawImage(const char *Filename,int X,int Y,int Width,int Height)
{
	CPicture pic;
	if( !pic.Load(Filename) )
		return false;

	bool bResult = pic.Draw(m_dc,CPoint(X,Y),CSize(Width,Height));
	return bResult;
}

bool CVIPDesignView::DrawImage(unsigned char ImageType,void *pGraphic,int GraphicSize,int X,int Y,int Width,int Height)
{
	FILE *f = fopen("C:\\temp.jpg","wb");
	fwrite(pGraphic,1,GraphicSize,f);
	fclose(f);

	CPicture pic;
	if( pic.Load("C:\\temp.jpg") )
	{
		if( Width==0 )
			Width=pic.GetOriginalWidth();
		if( Height==0 )
			Height=pic.GetOriginalHeight();
Width=176;  // TODO - TEMP HACK -- the above are returning wrong values!
Height=208;
		bool bResult = pic.Draw(m_dc,CPoint(X,Y),CSize(Width,Height));
		return bResult;
	}
	return false;

}

bool CVIPDesignView::DrawText(const char *Text, MyRect &r)
{
	CRect r2(r.left,r.top,r.right,r.bottom);
	m_dc->DrawText(Text, strlen(Text), r2, 0);
	return true;
}

void CVIPDesignView::SetTextProperties(int Size,const char *Font,int R, int G, int B)
{
	LOGFONT lf;
	memset( &lf, 0, sizeof(lf) );
	lf.lfHeight     = Size;
	lf.lfQuality    = NONANTIALIASED_QUALITY;
	_tcscpy( lf.lfFaceName, Font );
	HFONT hFont = ::CreateFontIndirect( &lf );

	HGDIOBJ hOldFont = m_dc->SelectObject(hFont);
	m_dc->SetTextColor(RGB(R,G,B) ); 
	lf.lfCharSet = SYMBOL_CHARSET;
	m_dc->SetBkMode(TRANSPARENT);

	if( !m_hOriginalFont )
		m_hOriginalFont=hOldFont;
	else
		::DeleteObject(hOldFont);
}

void CVIPDesignView::SwitchToMenu(VIPMenu *pMenu)
{
	CVIPDesignDoc *pDoc = GetDocument();
	pDoc->SwitchToMenu(pMenu);
}

bool CVIPDesignView::Draw3dRect(MyRect &r)
{
	CRect r2(r.left,r.top,r.right,r.bottom);
	m_dc->Draw3dRect(r2,RGB(125,125,125),RGB(175,175,175));
	return true;
}
