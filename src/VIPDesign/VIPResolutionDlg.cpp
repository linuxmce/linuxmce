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
// VIPResolution.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "PlutoUtils/StringUtils.h"

#include "VIPDesign.h"
#include "VIPResolutionDlg.h"

#include "Pluto_Main/Define_Button.h"

// CVIPResolutionDlg dialog

IMPLEMENT_DYNAMIC(CVIPResolutionDlg, CDialog)
CVIPResolutionDlg::CVIPResolutionDlg(class VIPMenuResolution *pResolution)
	: CDialog(CVIPResolutionDlg::IDD, NULL)
	, m_Y(FALSE)
	, m_N(FALSE)
	, m_E(FALSE)
	, m_F(FALSE)
	, m_U(FALSE)
	, m_D(FALSE)
	, m_L(FALSE)
	, m_R(FALSE)
	, m_0(FALSE)
	, m_1(FALSE)
	, m_2(FALSE)
	, m_3(FALSE)
	, m_4(FALSE)
	, m_5(FALSE)
	, m_6(FALSE)
	, m_7(FALSE)
	, m_8(FALSE)
	, m_9(FALSE)
	, m_CriteriaValue(_T(""))
	, m_HideApp(FALSE)
	, m_ReportVariables(FALSE)
	, m_ActionValue(_T(""))
	, m_BasketToggle(FALSE)
	, m_Basket_ID(_T(""))
	, m_Basket_Description(_T(""))
	, m_Basket_Cost(_T(""))
	, m_C(FALSE)
	, m_sProgramName(_T(""))
{
	m_pResolution=pResolution;
}

CVIPResolutionDlg::~CVIPResolutionDlg()
{
}

void CVIPResolutionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_RES_Y, m_Y);
	DDX_Check(pDX, IDC_RES_N, m_N);
	DDX_Check(pDX, IDC_RES_E, m_E);
	DDX_Check(pDX, IDC_RES_F, m_F);
	DDX_Check(pDX, IDC_RES_UP, m_U);
	DDX_Check(pDX, IDC_RES_DOWN, m_D);
	DDX_Check(pDX, IDC_RES_LEFT, m_L);
	DDX_Check(pDX, IDC_RES_RIGHT, m_R);
	DDX_Check(pDX, IDC_RES_0, m_0);
	DDX_Check(pDX, IDC_RES_1, m_1);
	DDX_Check(pDX, IDC_RES_2, m_2);
	DDX_Check(pDX, IDC_RES_3, m_3);
	DDX_Check(pDX, IDC_RES_4, m_4);
	DDX_Check(pDX, IDC_RES_5, m_5);
	DDX_Check(pDX, IDC_RES_6, m_6);
	DDX_Check(pDX, IDC_RES_7, m_7);
	DDX_Check(pDX, IDC_RES_8, m_8);
	DDX_Check(pDX, IDC_RES_9, m_9);
	DDX_Control(pDX, IDC_RES_CRIT_VARIABLE, m_cbCriteriaVariable);
	DDX_Text(pDX, IDC_RES_CRIT_VAR_VALUE, m_CriteriaValue);
	DDX_Check(pDX, IDC_RES_HIDE_APP, m_HideApp);
	DDX_Check(pDX, IDC_RES_REPORT, m_ReportVariables);
	DDX_Control(pDX, IDC_RES_GOTO, m_cbGoto);
	DDX_Control(pDX, IDC_RES_ACTION_VARIABLE, m_cbActionVariable);
	DDX_Text(pDX, IDC_RES_VALUE, m_ActionValue);
	DDX_Control(pDX, IDC_BASKET_ACTION, m_BasketAction);
	DDX_Check(pDX, IDC_RES_BASKET_TOGGLE, m_BasketToggle);
	DDX_Text(pDX, IDC_RES_BASKET_ID, m_Basket_ID);
	DDX_Text(pDX, IDC_RES_BASKET_DESCRIPTION, m_Basket_Description);
	DDX_Text(pDX, IDC_RES_BASKET_COST, m_Basket_Cost);
	DDX_Control(pDX, IDC_RES_CLOSE_REQUEST, m_cbCloseRequest);
	DDX_Check(pDX, IDC_RES_CLEAR, m_C);
	DDX_Text(pDX, IDC_RES_LAUNCH_PROGRAM, m_sProgramName);
}


BEGIN_MESSAGE_MAP(CVIPResolutionDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CVIPResolutionDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

	m_Y=m_pResolution->m_sTerminatingKey.find(BUTTON_Phone_Talk_CONST)!=string::npos;
	m_N=m_pResolution->m_sTerminatingKey.find(BUTTON_Phone_End_CONST)!=string::npos;
	m_E=m_pResolution->m_sTerminatingKey.find(BUTTON_Enter_CONST)!=string::npos;
	m_F=m_pResolution->m_sTerminatingKey.find(BUTTON_Asterisk_CONST)!=string::npos;//F
	m_C=m_pResolution->m_sTerminatingKey.find(BUTTON_Phone_C_CONST)!=string::npos;
	m_U=m_pResolution->m_sTerminatingKey.find(BUTTON_Up_Arrow_CONST)!=string::npos;
	m_D=m_pResolution->m_sTerminatingKey.find(BUTTON_Down_Arrow_CONST)!=string::npos;
	m_L=m_pResolution->m_sTerminatingKey.find(BUTTON_Left_Arrow_CONST)!=string::npos;
	m_R=m_pResolution->m_sTerminatingKey.find(BUTTON_Right_Arrow_CONST)!=string::npos;
	m_0=m_pResolution->m_sTerminatingKey.find(BUTTON_0_CONST)!=string::npos;
	m_1=m_pResolution->m_sTerminatingKey.find(BUTTON_1_CONST)!=string::npos;
	m_2=m_pResolution->m_sTerminatingKey.find(BUTTON_2_CONST)!=string::npos;
	m_3=m_pResolution->m_sTerminatingKey.find(BUTTON_3_CONST)!=string::npos;
	m_4=m_pResolution->m_sTerminatingKey.find(BUTTON_4_CONST)!=string::npos;
	m_5=m_pResolution->m_sTerminatingKey.find(BUTTON_5_CONST)!=string::npos;
	m_6=m_pResolution->m_sTerminatingKey.find(BUTTON_6_CONST)!=string::npos;
	m_7=m_pResolution->m_sTerminatingKey.find(BUTTON_7_CONST)!=string::npos;
	m_8=m_pResolution->m_sTerminatingKey.find(BUTTON_8_CONST)!=string::npos;
	m_9=m_pResolution->m_sTerminatingKey.find(BUTTON_9_CONST)!=string::npos;

	m_HideApp=m_pResolution->m_iHideApp;
	m_ReportVariables=m_pResolution->m_iReportToServer;
	m_ActionValue=m_pResolution->m_sActionVariableValue.c_str();
	m_CriteriaValue=m_pResolution->m_sCriteriaVariableValue.c_str();

	m_sProgramName = m_pResolution->m_sProgramName.c_str();

	m_cbCloseRequest.AddString("Don't close request");
	m_cbCloseRequest.AddString("Close with Cancel");
	m_cbCloseRequest.AddString("Close with OK");
	m_cbCloseRequest.AddString("Close with checkout");
	m_cbCloseRequest.AddString("Close with switch to 1");
	m_cbCloseRequest.AddString("Close with switch to 2");
	m_cbCloseRequest.AddString("Close with switch to 3");
	m_cbCloseRequest.AddString("Close with switch to 4");
	m_cbCloseRequest.AddString("Close with switch to 5");

	m_cbCloseRequest.SetCurSel(m_pResolution->m_iCloseRequest);

	VIPMenuCollection *pMenuCollection = m_pResolution->m_pMenu->m_pCollection;

	MYSTL_ITERATE_LONG_MAP(pMenuCollection->m_mapMenus,VIPMenu,pMenu,itm)
	{
		int Value=m_cbGoto.AddString(pMenu->m_sDescription.c_str());
		m_cbGoto.SetItemData(Value,pMenu->m_iMenuNumber);
		if( m_pResolution->m_iMenuNumber_Goto==pMenu->m_iMenuNumber )
			m_cbGoto.SetCurSel(Value);
	}

	MYSTL_ITERATE_LONG_MAP(pMenuCollection->m_mapVariables,VIPVariable,pVariable,iv)
	{
		int Value=m_cbCriteriaVariable.AddString(pVariable->m_sDescription.c_str());
		m_cbCriteriaVariable.SetItemData(Value,pVariable->m_iVariableID);
		if( pVariable->m_iVariableID==m_pResolution->m_iCriteriaVariableID )
			m_cbCriteriaVariable.SetCurSel(Value);

		Value=m_cbActionVariable.AddString(pVariable->m_sDescription.c_str());
		m_cbActionVariable.SetItemData(Value,pVariable->m_iVariableID);
		if( pVariable->m_iVariableID==m_pResolution->m_iActionVariableID )
			m_cbActionVariable.SetCurSel(Value);
	}

	m_BasketToggle = m_pResolution->m_iBasketToggle;
	m_Basket_ID = m_pResolution->m_sBasketID.c_str();
	m_Basket_Description = m_pResolution->m_sBasketDescription.c_str();
	m_Basket_Cost = StringUtils::itos(m_pResolution->m_iBasketCost).c_str();

	m_BasketAction.AddString("n/a");
	m_BasketAction.AddString("Add to Basket");
	m_BasketAction.AddString("Remove From Basket");
	m_BasketAction.AddString("Add Option");
	m_BasketAction.AddString("Clear Basket");
	m_BasketAction.SetCurSel(m_pResolution->m_iBasketAction);

	UpdateData(FALSE);
	return TRUE;
}


// CVIPResolutionDlg message handlers

void CVIPResolutionDlg::OnBnClickedOk()
{
	OnOK();
/*
	m_pResolution->m_sTerminatingKey="";
	if( m_Y )
		m_pResolution->m_sTerminatingKey+="Y";
	if( m_N )
		m_pResolution->m_sTerminatingKey+="N";
	if( m_E )
		m_pResolution->m_sTerminatingKey+="E";
	if( m_F )
		m_pResolution->m_sTerminatingKey+="F";
	if( m_C )
		m_pResolution->m_sTerminatingKey+="C";
	if( m_U )
		m_pResolution->m_sTerminatingKey+="U";
	if( m_D )
		m_pResolution->m_sTerminatingKey+="D";
	if( m_L )
		m_pResolution->m_sTerminatingKey+="L";
	if( m_R )
		m_pResolution->m_sTerminatingKey+="R";
	if( m_0 )
		m_pResolution->m_sTerminatingKey+="0";
	if( m_1 )
		m_pResolution->m_sTerminatingKey+="1";
	if( m_2 )
		m_pResolution->m_sTerminatingKey+="2";
	if( m_3 )
		m_pResolution->m_sTerminatingKey+="3";
	if( m_4 )
		m_pResolution->m_sTerminatingKey+="4";
	if( m_5 )
		m_pResolution->m_sTerminatingKey+="5";
	if( m_6 )
		m_pResolution->m_sTerminatingKey+="6";
	if( m_7 )
		m_pResolution->m_sTerminatingKey+="7";
	if( m_8 )
		m_pResolution->m_sTerminatingKey+="8";
	if( m_9 )
		m_pResolution->m_sTerminatingKey+="9";
*/

	m_pResolution->m_sTerminatingKey="";
	if( m_Y )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Phone_Talk_CONST);
	if( m_N )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Phone_End_CONST);
	if( m_E )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Enter_CONST);
	if( m_F )
		m_pResolution->m_sTerminatingKey+="F";
	if( m_C )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Phone_C_CONST);
	if( m_U )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Up_Arrow_CONST);
	if( m_D )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Down_Arrow_CONST);
	if( m_L )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Left_Arrow_CONST);
	if( m_R )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_Right_Arrow_CONST);
	if( m_0 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_0_CONST);
	if( m_1 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_1_CONST);
	if( m_2 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_2_CONST);
	if( m_3 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_3_CONST);
	if( m_4 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_4_CONST);
	if( m_5 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_5_CONST);
	if( m_6 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_6_CONST);
	if( m_7 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_7_CONST);
	if( m_8 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_8_CONST);
	if( m_9 )
		m_pResolution->m_sTerminatingKey+=char(BUTTON_9_CONST);


	m_pResolution->m_sCriteriaVariableValue = m_CriteriaValue;
	m_pResolution->m_iHideApp=m_HideApp;
	m_pResolution->m_iCloseRequest=m_cbCloseRequest.GetCurSel();
	m_pResolution->m_iReportToServer=m_ReportVariables;
	m_pResolution->m_sActionVariableValue=m_ActionValue;
	m_pResolution->m_sProgramName = m_sProgramName;

	m_pResolution->m_iCriteriaVariableID=m_cbCriteriaVariable.GetItemData(m_cbCriteriaVariable.GetCurSel());
	m_pResolution->m_iActionVariableID=m_cbActionVariable.GetItemData(m_cbActionVariable.GetCurSel());
	m_pResolution->m_iMenuNumber_Goto=m_cbGoto.GetItemData(m_cbGoto.GetCurSel());

	m_pResolution->m_iBasketToggle = m_BasketToggle;
	m_pResolution->m_sBasketID = m_Basket_ID;
	m_pResolution->m_sBasketDescription = m_Basket_Description;
	m_pResolution->m_iBasketCost = atoi(m_Basket_Cost);
	m_pResolution->m_iBasketAction = m_BasketAction.GetCurSel();
}
