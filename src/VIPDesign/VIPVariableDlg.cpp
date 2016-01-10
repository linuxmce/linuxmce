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
// VIPVariable.cpp : implementation file
//

#include "stdafx.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"

#include "VIPDesign.h"
#include "VIPVariableDlg.h"


// CVIPVariableDlg dialog

IMPLEMENT_DYNAMIC(CVIPVariableDlg, CDialog)
CVIPVariableDlg::CVIPVariableDlg(class VIPVariable *pVariable)
	: CDialog(CVIPVariableDlg::IDD, NULL)
	, m_CustomVariableDescription(_T(""))
	, m_Encrypt(FALSE)
	, m_PhoneSetsValue(FALSE)
	, m_sValue(_T(""))
{
	m_pVariable=pVariable;
}

CVIPVariableDlg::~CVIPVariableDlg()
{
}

void CVIPVariableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VAR_PREDEFINED, m_cbPredefinedVariables);
	DDX_Text(pDX, IDC_VAR_CUSTOM_VARIABLE, m_CustomVariableDescription);
	DDX_Check(pDX, IDC_VAR_ENCRYPT, m_Encrypt);
	DDX_Check(pDX, IDC_VAR_PHONE_SETS_VALUE, m_PhoneSetsValue);
	DDX_Control(pDX, IDC_VAR_FORMAT, m_Format);
	DDX_Text(pDX, IDC_VAR_VALUE, m_sValue);
}


BEGIN_MESSAGE_MAP(CVIPVariableDlg, CDialog)
	ON_EN_CHANGE(IDC_VAR_CUSTOM_VARIABLE, OnEnChangeVarCustomVariable)
	ON_CBN_SELCHANGE(IDC_VAR_PREDEFINED, OnCbnSelchangeVarPredefined)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CVIPVariableDlg message handlers

BOOL CVIPVariableDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	AddVariableIfNotThere("Payment Amount (user specified)",VIPVAR_PAYMENT_AMOUNT_USER);
	AddVariableIfNotThere("Payment Method",VIPVAR_PAYMENT_METHOD);
	AddVariableIfNotThere("PIN Code",VIPVAR_PIN_CODE);
	AddVariableIfNotThere("Invoice Number",VIPVAR_INVOICE_NUM);
	AddVariableIfNotThere("Establishment",VIPVAR_ESTABLISHMENT);
	AddVariableIfNotThere("Transaction Description",VIPVAR_TRANS_DESC);
	AddVariableIfNotThere("Invoice Detail",VIPVAR_INVOICE_DETAIL);
	AddVariableIfNotThere("Transaction Number",VIPVAR_VIP_TRANS_NUM);
	AddVariableIfNotThere("Available Payment Methods",VIPVAR_AVAIL_PAYMENT_METHODS);
	AddVariableIfNotThere("Payment Amount (requested)",VIPVAR_PAYMENT_AMOUNT_REQUESTED);

	m_CustomVariableDescription = m_pVariable->m_sDescription.c_str();
	m_Encrypt = m_pVariable->m_bEncryptBeforeSending;
	m_PhoneSetsValue = m_pVariable->m_iPhoneSetsValue;
	m_sValue = m_pVariable->m_sInitialValue.c_str();

	AddFormat("Numeric",MENU_VARFORMAT_NUMERIC);
	AddFormat("2 Decimal",MENU_VARFORMAT_2_DECIMAL);
	AddFormat("Text",MENU_VARFORMAT_TEXT);
	AddFormat("PIN",MENU_VARFORMAT_NUMERIC_PIN);

	UpdateData(FALSE);
	return TRUE;
}

void CVIPVariableDlg::AddFormat(string Description,int FormatID)
{
	int Value = m_Format.AddString(Description.c_str());
	m_Format.SetItemData(Value,FormatID);
	if( m_pVariable->m_iFormat==FormatID )
		m_Format.SetCurSel(Value);
}
void CVIPVariableDlg::AddVariableIfNotThere(string Description,int VariableID)
{
	if( m_pVariable->m_iVariableID==VariableID || m_pVariable->m_pCollection->m_mapVariables.find(VariableID)==m_pVariable->m_pCollection->m_mapVariables.end() )
	{
		int Value = m_cbPredefinedVariables.AddString(Description.c_str());
		m_cbPredefinedVariables.SetItemData(Value,VariableID);
		if( m_pVariable->m_iVariableID==VariableID )
			m_cbPredefinedVariables.SetCurSel(Value);
	}
}

void CVIPVariableDlg::OnEnChangeVarCustomVariable()
{
	UpdateData(TRUE);
	m_cbPredefinedVariables.SetCurSel(-1);
	UpdateData(FALSE);
}


void CVIPVariableDlg::OnCbnSelchangeVarPredefined()
{
	UpdateData(TRUE);
	CString str;
	m_cbPredefinedVariables.GetLBText(m_cbPredefinedVariables.GetCurSel(),str);
	m_CustomVariableDescription = str;
	UpdateData(FALSE);
}

void CVIPVariableDlg::OnBnClickedOk()
{
	OnOK();
	if( m_cbPredefinedVariables.GetCurSel()>=0 )
	{
		m_pVariable->m_iVariableID = (long) m_cbPredefinedVariables.GetItemData(m_cbPredefinedVariables.GetCurSel());
	}
	else
	{
		int NextID=1000;

		MYSTL_ITERATE_LONG_MAP(m_pVariable->m_pCollection->m_mapVariables,VIPVariable,pVariable,itVariables)
		{
			if( NextID<=pVariable->m_iVariableID )
				NextID = pVariable->m_iVariableID+1;
		}
		m_pVariable->m_iVariableID = NextID;
	}
	if( m_Format.GetCurSel()>=0 )
	{
		m_pVariable->m_iFormat=(unsigned short) m_Format.GetItemData(m_Format.GetCurSel());
	}
	else
		m_pVariable->m_iFormat=MENU_VARFORMAT_NUMERIC;

	m_pVariable->m_sDescription = m_CustomVariableDescription;
	m_pVariable->m_bEncryptBeforeSending = (m_Encrypt==TRUE);
	m_pVariable->m_iPhoneSetsValue = m_PhoneSetsValue;
	m_pVariable->m_sInitialValue = m_sValue;
}
