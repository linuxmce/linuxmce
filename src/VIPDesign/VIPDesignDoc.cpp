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
// VIPDesignDoc.cpp : implementation of the CVIPDesignDoc class
//
#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/StringUtils.h"

#include "VIPDesign.h"

#include "VIPDesignDoc.h"
#include "VIPTextDlg.h"
#include "VIPListDlg.h"
#include "VIPGraphicDlg.h"
#include "VIPInputDlg.h"
#include "VIPResolutionDlg.h"
#include "VIPMenuDlg.h"
#include "VIPVariableDlg.h"
#include "VIPEditDlg.h"
#include "VIPPropertiesDlg.h"
#include "ResetEmulatorDlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVIPDesignDoc

IMPLEMENT_DYNCREATE(CVIPDesignDoc, CDocument)

BEGIN_MESSAGE_MAP(CVIPDesignDoc, CDocument)
	ON_COMMAND(ID_ADD_TEXT, OnAddText)
	ON_COMMAND(ID_ADD_GRAPHIC, OnAddGraphic)
	ON_COMMAND(ID_EDIT_TEXT, OnEditText)
	ON_COMMAND(ID_EDIT_GRAPHIC, OnEditGraphic)
	ON_COMMAND(ID_EDIT_INPUT, OnEditInput)
	ON_COMMAND(ID_EDIT_RESOLUTION, OnEditResolution)
	ON_COMMAND(ID_EDIT_MENU, OnEditMenu)
	ON_COMMAND(ID_EDIT_VARIABLE, OnEditVariable)
	ON_COMMAND(ID_ADD_INPUT, OnAddInput)
	ON_COMMAND(ID_ADD_RESOLUTION, OnAddResolution)
	ON_COMMAND(ID_ADD_MENU, OnAddMenu)
	ON_COMMAND(ID_ADD_VARIABLE, OnAddVariable)
	ON_COMMAND(ID_VIEW_GOTOMENU, OnViewGotomenu)
	ON_COMMAND(ID_GOTO_LISTMENUS, OnGotoListmenus)
	ON_COMMAND(ID_MODIFY_PROPERTIES, OnModifyProperties)
	ON_COMMAND(ID_ADD_LIST, OnAddList)
	ON_COMMAND(ID_MODIFY_LIST, OnModifyList)
	ON_COMMAND(ID_POS_MEMBERIDENTIFIED, OnPosMemberidentified)
END_MESSAGE_MAP()

// CVIPDesignDoc construction/destruction


CVIPDesignDoc::CVIPDesignDoc()
{
	g_pPlutoConfig->m_pDoc = this;
	m_pMenuCollection = new VIPMenuCollection(0);
	m_pMenu = new VIPMenu(0);
	m_pMenu->m_sDescription="Default";
	m_pMenuCollection->AddMenu(m_pMenu);
	m_pMenuInput=NULL;
	m_pImageStatic_Type=0;
	m_pImageStatic_Size=0;
	m_pImageStatic_Data=NULL;
}

CVIPDesignDoc::~CVIPDesignDoc()
{
	delete m_pMenuCollection;
}

BOOL CVIPDesignDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


// CVIPDesignDoc serialization

void CVIPDesignDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		string BinaryFileName = ar.m_strFileName.Mid(0,ar.m_strFileName.GetLength()-4) + ".vmc";
		CString x = BinaryFileName.c_str();
		ar << x;
		m_pMenuCollection->ConvertToBinary();
		FILE *file = fopen(BinaryFileName.c_str(),"wb");
		fwrite(m_pMenuCollection->m_pBinary,1,m_pMenuCollection->m_iBinarySize,file);
		fclose(file);
	}
	else
	{
		CString x;
		ar >> x;
		string BinaryFileName = ar.m_strFileName.Mid(0,ar.m_strFileName.GetLength()-4) + ".vmc";
		FILE *file = fopen(BinaryFileName.c_str(),"rb");
		fseek(file, 0L, SEEK_END);
		int total_size=ftell(file);
		if( total_size==0 )
		{
			fclose(file);
			return;
		}
		fseek(file, 0L, SEEK_SET);
		char *data_block=(char *)malloc(total_size+1);
		size_t bytesread=fread(data_block,1,total_size,file);
		fclose(file);

		delete m_pMenuCollection;
		m_pMenuCollection = new VIPMenuCollection(total_size,data_block);
		delete[] data_block;

		delete m_pMenu;

		UpdateListOfMenus();
		SwitchToMenu(m_pMenuCollection->m_pMenu_Starting);
	}
}


// CVIPDesignDoc diagnostics

#ifdef _DEBUG
void CVIPDesignDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVIPDesignDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVIPDesignDoc commands

void CVIPDesignDoc::OnAddText()
{
	VIPMenuElement_Text *pText = new VIPMenuElement_Text(0,"",0,0,0,0,"",0,0,0);
	CVIPTextDlg vipTextDlg(pText,this);
	if( vipTextDlg.DoModal()==1 )
	{
		// Add it
		m_pMenu->AddElement(pText);
		UpdateAllViews(NULL);
		SetModifiedFlag(TRUE);
	}
	else
		delete pText;
}

void CVIPDesignDoc::OnAddList()
{
	VIPMenuElement_List *pList = new VIPMenuElement_List(0,0,"",0,"",0,0,0,0,"",0,0,0);
	CVIPListDlg vipListDlg(pList,this);
	if( vipListDlg.DoModal()==1 )
	{
		// Add it
		m_pMenu->AddElement(pList);
		UpdateAllViews(NULL);
		SetModifiedFlag(TRUE);
	}
	else
		delete pList;
}

void CVIPDesignDoc::OnAddGraphic()
{
	VIPMenuElement_Graphic *pGraphic = new VIPMenuElement_Graphic("",0,0,0,0);
	CVIPGraphicDlg vipGraphicDlg(pGraphic,this);
	if( vipGraphicDlg.DoModal()==1 )
	{
		// Add it
		m_pMenu->AddElement(pGraphic);
		UpdateAllViews(NULL);
		SetModifiedFlag(TRUE);
	}
	else
		delete pGraphic;
}

void CVIPDesignDoc::OnAddInput()
{
	VIPMenuInput *pInput = new VIPMenuInput(0,0,0,NULL,NULL);
	pInput->m_pMenu=m_pMenu;
	CVIPInputDlg vipInputDlg(pInput);
	if( vipInputDlg.DoModal()==1 )
	{
		// Add it
		m_pMenu->AddInput(pInput);
		UpdateAllViews(NULL);
		SetModifiedFlag(TRUE);
	}
	else
		delete pInput;
}

void CVIPDesignDoc::OnAddResolution()
{
	VIPMenuResolution *pResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
	pResolution->m_pMenu = m_pMenu;
	CVIPResolutionDlg vipResolutionDlg(pResolution);
	if( vipResolutionDlg.DoModal()==1 )
	{
		// Add it
		m_pMenu->AddResolution(pResolution);
		UpdateAllViews(NULL);
		SetModifiedFlag(TRUE);
	}
	else
		delete pResolution;
}

void CVIPDesignDoc::OnAddMenu()
{
	VIPMenu *pMenu = new VIPMenu(0);
	CVIPMenuDlg vipMenuDlg(pMenu);
	if( vipMenuDlg.DoModal()==1 )
	{
		// Add it
		m_pMenuCollection->AddMenu(pMenu);
		UpdateAllViews(NULL);
		SetModifiedFlag(TRUE);
	}
	else
		delete pMenu;
	UpdateListOfMenus();
}

void CVIPDesignDoc::OnAddVariable()
{
	VIPVariable *pVariable = new VIPVariable(-1,0,"",FALSE,0);
	pVariable->m_pCollection=m_pMenuCollection;
	CVIPVariableDlg vipVariableDlg(pVariable);
	if( vipVariableDlg.DoModal()==1 )
	{
		// Add it
		m_pMenuCollection->AddVariable(pVariable);
		UpdateAllViews(NULL);
		SetModifiedFlag(TRUE);
	}
	else
		delete pVariable;
}

void CVIPDesignDoc::OnEditText()
{
	CVIPEditDlg dlg(&m_pMenu->m_mapMenuElements,NULL,NULL,
		NULL,NULL,
		NULL,NULL,
		NULL,this);

	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void CVIPDesignDoc::OnModifyList()
{
	CVIPEditDlg dlg(NULL,&m_pMenu->m_mapMenuElements,NULL,
		NULL,NULL,
		NULL,NULL,
		NULL,this);

	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void CVIPDesignDoc::OnEditGraphic()
{
	CVIPEditDlg dlg(NULL,NULL,&m_pMenu->m_mapMenuElements,
		NULL,NULL,
		NULL,NULL,
		NULL,this);

	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void CVIPDesignDoc::OnEditInput()
{
	CVIPEditDlg dlg(NULL,NULL,NULL,
		&m_pMenu->m_mapMenuInputs,NULL,
		NULL,NULL,
		NULL,this);

	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void CVIPDesignDoc::OnEditResolution()
{
	CVIPEditDlg dlg(NULL,NULL,NULL,
		NULL,NULL,
		&m_pMenu->m_mapMenuResolutions,NULL,
		NULL,this);

	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void CVIPDesignDoc::OnEditMenu()
{
	CVIPEditDlg dlg(NULL,NULL,NULL,
		NULL,NULL,
		NULL,&m_pMenu->m_pCollection->m_mapMenus,
		NULL,this);

	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
	UpdateListOfMenus();
}

void CVIPDesignDoc::OnEditVariable()
{
	CVIPEditDlg dlg(NULL,NULL,NULL,
		NULL,NULL,
		NULL,NULL,
		&m_pMenu->m_pCollection->m_mapVariables,this);

	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void CVIPDesignDoc::OnViewGotomenu()
{
	// TODO: Add your command handler code here
}

/////////////////////////////////////////////////////////////////////////////
// CVIPDesignDoc implementation

//***********************************************************************
// Function: CVIPDesignDoc::OnCmdMsg()
//
// Purpose:
//    OnCmdMsg() is called by the framework to route and dispatch
//    command messages and to handle the update of command
//    user-interface objects.
//
//    Here we extend the standard command routing to intercept command
//    messages with variable command IDs.  In this sample, the
//    currently defined text color options are stored in
//    CVIPDesignDoc::m_aColorDef.  Each element of this array contains
//    the command ID associated with that color.  When a command ID
//    is passed to this function, we search m_aColorDef for a matching
//    command ID.  If one is found, we process the message.  Otherwise,
//    we route the command to the standard OnCmdMsg processing.
//
// Parameters:
//    nID           -- contains the command ID
//    nCode         -- identifies the command notification code
//    pExtra        -- used according to the value of nCode
//    pHandlerInfo  -- if not NULL, filled in with pTarget and pmf
//                     members of CMDHANDLERINFO structure, instead
//                     of dispatching the command.  Typically NULL.
//
// Returns:
//    nonzero if message handled, otherwise 0.
//
// Comments:
//    See the documentation for CCmdTarget::OnCmdMsg() for further
//    information.  Command routing is also discussed in tech note #21.
//
//***********************************************************************
BOOL CVIPDesignDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra,
							AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// If pHandlerInfo is NULL, then handle the message
	if (pHandlerInfo == NULL)
	{
		// Filter the commands sent to a text color menu option
		MYSTL_ITERATE_LONG_MAP(m_pMenuCollection->m_mapMenus,VIPMenu,pMenu,i)
		{
			if (nID >= 60000 && nID <= 61000) // There must be a better way!
			{
				if (nCode == CN_COMMAND)
				{
					// Handle WM_COMMAND message
					SwitchToMenu(m_pMenuCollection->m_mapMenus_Find(nID-60000));
				}
				else if (nCode == CN_UPDATE_COMMAND_UI)
				{
					CCmdUI* pCmdUI = (CCmdUI*)pExtra;
					pCmdUI->SetCheck(m_pMenu->m_iMenuNumber==nID-60000);
					pCmdUI->Enable(TRUE);
				}
				return TRUE;
			}
		}
	}

	// If we didn't process the command, call the base class
	// version of OnCmdMsg so the message-map can handle the message
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CVIPDesignDoc::UpdateListOfMenus()
{
	CMenu* pGotoMenu = NULL;
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	int iPos;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
	{
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_GOTO_LISTMENUS)
		{
			pGotoMenu = pMenu;
			break;
		}
	}
	if( !pGotoMenu )
		return;

	// First, delete all items ..  Thank you MS, if I delete all of them I can't get the menu pad again!
	for (iPos = pGotoMenu->GetMenuItemCount()-1; iPos > 0; iPos--)
		pGotoMenu->DeleteMenu(iPos, MF_BYPOSITION);

	// Then, add a separator and an item for each available text color
	MYSTL_ITERATE_LONG_MAP(m_pMenuCollection->m_mapMenus,VIPMenu,pMenu,i)
	{
		CString str = pMenu->m_sDescription.c_str();

		// I can't figure a clean way to get this information
		pGotoMenu->AppendMenu(MF_ENABLED | MF_STRING,60000 + pMenu->m_iMenuNumber, str);
	}
}

void CVIPDesignDoc::OnGotoListmenus()
{
	CResetEmulatorDlg c(m_pMenuCollection);
	c.DoModal();
	SwitchToMenu(m_pMenuCollection->m_pMenu_Starting);
}


void CVIPDesignDoc::OnModifyProperties()
{
	CVIPPropertiesDlg dlg(m_pMenuCollection);
	dlg.DoModal();
	SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
}

void CVIPDesignDoc::SwitchToMenu(VIPMenu *pMenu,bool bUpdateViews)
{
	if( !pMenu )
		return;

    static long LastMenu = pMenu->m_iMenuNumber;
    
	m_pMenu = pMenu;
	// Just get the first one
	MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuInputs,VIPMenuInput,m_pMenuInput,itm);

	// Go through all of them
	MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuInputs,VIPMenuInput,pInput,itm2)
	{
        if(LastMenu != pMenu->m_iMenuNumber && pInput->m_iClearVariable )
		{
			VIPVariable *pVariable = m_pMenuCollection->m_mapVariables_Find(pInput->m_iVariableID);
			if( pVariable )
				pVariable->m_sCurrentValue="";
		}
	}

    LastMenu = pMenu->m_iMenuNumber;

	if( bUpdateViews )
		UpdateAllViews(NULL);
}
void CVIPDesignDoc::OnPosMemberidentified()
{
	// TODO: Add your command handler code here
}

void CVIPDesignDoc::InvalidateAllViews()
	// walk through all views
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		pView->Invalidate(TRUE);
	}
}
