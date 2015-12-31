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
#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/DBHelper.h"
#include "VR_RequestPayment.h"
#include "VR_RequestSecureTransaction.h"
#include "VA_SendMenuToPhone.h"
#include "VIPShared/VIPMenu.h"

#include "RA/RA_Processor.h"
#include "DCE/DCEMySqlConfig.h"

#include <assert.h>
#include <iostream>
#include <sstream>

VR_RequestPayment::VR_RequestPayment(u_int64_t iMacAddress,long Amount,string InvoiceNumber,
		string Description,string CashierName,unsigned long FKID_PlutoId_Establishment,
		unsigned long FKID_PlutoId_User)
	: RA_Request()
{
	// Request is of the form:
	// AAAAIIIIdescription+null+CashierName+null
	m_iMacAddress=iMacAddress;
	m_iAmount=Amount;
	m_sInvoiceNumber=InvoiceNumber;
	m_sDescription=Description;
	m_sCashierName=CashierName;
	m_FKID_PlutoId_Establishment=FKID_PlutoId_Establishment;
	m_FKID_PlutoId_User=FKID_PlutoId_User;
}

bool VR_RequestPayment::ProcessRequest(RA_Processor *pRA_Processor)
{
#ifdef VIPSERVER

	DCEMySqlConfig *pDCEMySqlConfig = dynamic_cast<DCEMySqlConfig *>(pRA_Processor->m_pRA_Config);
	assert(NULL != pDCEMySqlConfig);

	std::ostringstream sql;

	sql << "INSERT INTO PmtTrans (FK_MasterUsers_Establishment,FK_MasterUsers_User,InvoiceNumber,Description,Cashier,Amount,DateTime) VALUES(" <<
		m_FKID_PlutoId_Establishment << "," << m_FKID_PlutoId_User << ",'" << StringUtils::SQLEscape(m_sInvoiceNumber) << "','" <<
		StringUtils::SQLEscape(m_sDescription) << "','" << StringUtils::SQLEscape(m_sCashierName) << "'," << m_iAmount << 
		",'" << StringUtils::SQLDateTime() << "');";

	cout << sql;

	int PKID_PmtTrans = pDCEMySqlConfig->ThreadedMysqlQueryWithID(sql.str());
	if( PKID_PmtTrans==0 )
	{
		cout << "FAILED: " << sql; 
		m_cProcessOutcome = INTERNAL_ERROR;
		return true;
	}

	MYSTL_ITERATE_LIST(m_listInvoiceDetail,InvoiceDetail,pInvoiceDetail,itListInvoiceDetail)
	{
		sql.str("");
		sql << "INSERT INTO PmtTrans_Detail (FKID_PmtTrans,LineID,ProductID,Description,Amount) VALUES(" <<
			PKID_PmtTrans << ",'" << StringUtils::SQLEscape(pInvoiceDetail->m_sLineID) << "','" <<
			StringUtils::SQLEscape(pInvoiceDetail->m_sProductID) << "','" << StringUtils::SQLEscape(pInvoiceDetail->m_sDescription) << "'," <<
			pInvoiceDetail->m_iAmount << ");";

		int PKID_PmtTrans_Detail = pDCEMySqlConfig->ThreadedMysqlQueryWithID(sql.str());
		if( PKID_PmtTrans_Detail==0 )
		{
			cout << "FAILED: " << sql; 
			// Ignore it since it's not needed anyway
		}
	}

	sql.str("");
	PlutoSqlResult rsEstablishment,rsPaymentMethods;
	MYSQL_ROW EstablishmentRow=NULL,PaymentMethodsRow=NULL;
	sql << "SELECT Name FROM Establishment WHERE FK_MasterUsers=" << m_FKID_PlutoId_Establishment;
	

	if( (rsEstablishment.r = pDCEMySqlConfig->MySqlQueryResult(sql.str()))==NULL || 
		(EstablishmentRow=mysql_fetch_row(rsEstablishment.r))==NULL )
	{
		cout << "UNKNOWN ESTALIBSHMENT: " << sql; 
		m_cProcessOutcome = INVALID_REQUEST_DATA;
		return true;
	}

	sql.str("");
	sql << "SELECT PKID_CC,Description FROM Users_CC WHERE FK_MasterUsers=" << m_FKID_PlutoId_User;
	if( (rsPaymentMethods.r = pDCEMySqlConfig->MySqlQueryResult(sql.str()))==NULL || 
		(PaymentMethodsRow=mysql_fetch_row(rsPaymentMethods.r))==NULL )
	{
		cout << "user Cannot pay: " << sql; 
		m_cProcessOutcome = USER_CANNOT_PAY_ONLINE;
		return true;
	}

	std::ostringstream pmtMethod;

	while( PaymentMethodsRow )
	{
		pmtMethod << PaymentMethodsRow[0] << "\t" << PaymentMethodsRow[1] << "\n";
		PaymentMethodsRow=mysql_fetch_row(rsPaymentMethods.r);
	}

	m_cProcessOutcome = SUCCESSFULLY_PROCESSED;
	/*
	VR_ShowMenu *pMenu = new VR_ShowMenu((pDCEMySqlConfig->m_sMenuPath + "payment.vmc").c_str());
	
	MYSTL_ADDTO_LIST(pMenu->m_listInputVariables, (new VIPVariable(VIPVAR_PAYMENT_AMOUNT_REQUESTED,0,StringUtils::itos(m_iAmount),0,0)));
	MYSTL_ADDTO_LIST(pMenu->m_listInputVariables, (new VIPVariable(VIPVAR_INVOICE_NUM,0,m_sInvoiceNumber.c_str(),0,0)));
	MYSTL_ADDTO_LIST(pMenu->m_listInputVariables, (new VIPVariable(VIPVAR_ESTABLISHMENT,0,EstablishmentRow[0],0,0)));
	MYSTL_ADDTO_LIST(pMenu->m_listInputVariables, (new VIPVariable(VIPVAR_TRANS_DESC,0,m_sDescription.c_str(),0,0)));
	MYSTL_ADDTO_LIST(pMenu->m_listInputVariables, (new VIPVariable(VIPVAR_VIP_TRANS_NUM,0,StringUtils::itos(PKID_PmtTrans),0,0)));
	MYSTL_ADDTO_LIST(pMenu->m_listInputVariables, (new VIPVariable(VIPVAR_AVAIL_PAYMENT_METHODS,0,pmtMethod.str(),0,0)));

	string sInvoiceDetail;
	MYSTL_ITERATE_LIST(m_listInvoiceDetail,InvoiceDetail,pID,itID)
	{
		sInvoiceDetail += pID->m_sDescription + "\t" + StringUtils::itos(pID->m_iAmount) + "\n";
	}

	pMenu->m_listInputVariables.push_back(new VIPVariable(VIPVAR_INVOICE_DETAIL,0,sInvoiceDetail.c_str(),0,0));

	VA_SendMenuToPhone *pbefore = new VA_SendMenuToPhone(pMenu,m_iMacAddress);
	*/

	VA_SendMenuToPhone *pbefore = new VA_SendMenuToPhone(pDCEMySqlConfig->m_sMenuPath + "payment.vmc", 
		m_iMacAddress);

	MYSTL_ADDTO_LIST(m_listActions,pbefore);
	m_cProcessOutcome = SUCCESSFULLY_PROCESSED;
	return true;

	m_iPaymentResponse=1;
	m_sApprovalCode="APP1234";
	m_sCardNumber="3715XXXXXX12345";

	VIPMenuCollection *pMenuCollection = new VIPMenuCollection(VIPMENUCOLL_SECURE_PAYMENT);

	VIPVariable *pTransVariable = new VIPVariable(VIPVAR_INVOICE_NUM,1,StringUtils::itos(pDCEMySqlConfig->m_iTransNumber++),false,0); // Number 1, phone sets value
	pMenuCollection->AddVariable(pTransVariable);

	VIPVariable *pAmountVariable = new VIPVariable(VIPVAR_PAYMENT_AMOUNT_USER,1,"",false,MENU_VARFORMAT_2_DECIMAL); // Number 1, phone sets value
	pMenuCollection->AddVariable(pAmountVariable);

	VIPVariable *pPaymentMethodVariable = new VIPVariable(VIPVAR_PAYMENT_METHOD,1,"",false,0); // Number 1, phone sets value
	pMenuCollection->AddVariable(pPaymentMethodVariable);

	VIPVariable *pPinCode = new VIPVariable(VIPVAR_PIN_CODE,1,"",true,MENU_VARFORMAT_NUMERIC_PIN); 
	pMenuCollection->AddVariable(pPinCode);

	// The user can continue by pressing Y or entering a value
	VIPMenuResolution *pGotoPaymentTypeMenu = new VIPMenuResolution("YE",-1,"", "", /*program name*/
		0 /* hide */,0 /* close */, 0 /* report*/,-1 /* goto */, -1 /* action var */,"");

	// The payment menu, which also has a text input if the user wants to override the amount
	VIPMenu *pInvoiceMenu = new VIPMenu(VIPMENU_INVOICE);
	pMenuCollection->m_pMenu_Starting = pInvoiceMenu;
	pInvoiceMenu->AddResolution(pGotoPaymentTypeMenu);
	pInvoiceMenu->AddElement(new VIPMenuElement_Graphic("background.png",0,0,-1,-1));
	pInvoiceMenu->AddElement(new VIPMenuElement_Text(12,"Arial",50,50,30,29,"The Matrix\nStar Wars\n",125,125,125));
	VIPMenuElement_Text *pAmountToPay_Text = new VIPMenuElement_Text(12,"Arial",50,150,30,29,"",125,125,125);
	pInvoiceMenu->AddElement(pAmountToPay_Text);
	pInvoiceMenu->AddInput(new VIPMenuInput(0 /*#ofchars*/,0 /*term*/,1 /*clear*/,pAmountVariable,pAmountToPay_Text)); // Yes = Don't hide, don't report, just go to menu 2
	pMenuCollection->AddMenu(pInvoiceMenu);

	// The user chooses the pmt method by pressing 1, 2, or 3
	VIPMenuResolution *pGotoPin = new VIPMenuResolution("123",-1,"","",
		0 /* hide */,0 /* close */, 0 /* report*/,-1 /* goto */, -1 /* action var */,"");

	// The payment choices Menu
	VIPMenu *pPaymentChoices = new VIPMenu(VIPMENU_PMTMETHODS);
	pPaymentChoices->AddResolution(pGotoPin);
	pPaymentChoices->AddElement(new VIPMenuElement_Graphic("background.png",0,0,-1,-1));
	pPaymentChoices->AddElement(new VIPMenuElement_Text(12,"Arial",50,50,30,29,"How do you want to pay?",125,125,125));
	pPaymentChoices->AddInput(new VIPMenuInput(1 /*#ofchars*/,1/*term*/,1 /* clear*/,pPaymentMethodVariable,NULL)); // Yes = Don't hide, don't report, just go to menu 2

	pMenuCollection->AddMenu(pPaymentChoices);
	pGotoPaymentTypeMenu->SetGotoMenu(pPaymentChoices);

	// The user enters the pin and finishes with E or Y
	VIPMenuResolution *pEnteredPinResolution = new VIPMenuResolution("EY",-1,"","",
		0 /* hide */,1 /* close */, 0 /* report*/,-1 /* goto */, -1 /* action var */,"");

	// The PIN code Menu
	VIPMenu *pPINCode = new VIPMenu(VIPMENU_PINNUMBER);
	pPINCode->AddResolution(pEnteredPinResolution);
	pPINCode->AddElement(new VIPMenuElement_Graphic("background.png",0,0,-1,-1));
	pPINCode->AddElement(new VIPMenuElement_Text(12,"Arial",50,50,30,29,"Enter your pin",125,125,125));
	VIPMenuElement_Text *pPin_Text = new VIPMenuElement_Text(12,"Arial",50,150,30,29,"",125,125,125);
	pPINCode->AddElement(pPin_Text);

	pPINCode->AddInput(new VIPMenuInput(1 /*#ofchars*/,1/*term*/,1 /* clear*/,pPinCode,pPin_Text)); // Yes = Report and go to processing menu

	pMenuCollection->AddMenu(pPINCode);
	pGotoPin->SetGotoMenu(pPINCode);


	// We are processing your request
	VIPMenu *pWaitForVerification = new VIPMenu(VIPMENU_WAITPMTAUTH);
	pWaitForVerification->AddElement(new VIPMenuElement_Graphic("background.png",0,0,-1,-1));
	pWaitForVerification->AddElement(new VIPMenuElement_Text(12,"Arial",50,50,30,29,"Wait for verification",125,125,125));
	pMenuCollection->AddMenu(pWaitForVerification);
	pEnteredPinResolution->SetGotoMenu(pWaitForVerification);
/*
	// Make a menu in response
	VR_ShowMenu *pMenu = new VR_ShowMenu(pMenuCollection);
	// And stick embed it within an action
	VA_SendMenuToPhone *pbefore = new VA_SendMenuToPhone(pMenu);
	listActions.push_back(pbefore);
	m_cProcessOutcome = SUCCESSFULLY_PROCESSED;
*/
#endif
	return true;
}


bool VR_RequestPayment::UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition )
{
	m_pcDataBlock = pcDataBlock; m_dwAllocatedSize = dwAllocatedSize; m_pcCurrentPosition = pcCurrentPosition;
	
	if( bWriting ) // writing
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_LIST_INV_DETAIL:
			{
				list<InvoiceDetail *> *p_listInvoiceDetail = (list<InvoiceDetail *> *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) p_listInvoiceDetail->size() );
				for( list<InvoiceDetail *>::iterator it = p_listInvoiceDetail->begin(); it != p_listInvoiceDetail->end(); ++it )
				{
					InvoiceDetail *pInvoiceDetail = *it;
					pInvoiceDetail->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				return true; // We handled it
			}
			break;
		};
	}
	else // reading
	{
		switch( pItem->m_iSerializeDataType )
		{
		
		case SERIALIZE_DATA_TYPE_LIST_INV_DETAIL:
			{
				list<InvoiceDetail *> *p_listInvoiceDetail = (list<InvoiceDetail *> *) pItem->m_pItem;
				long Count=Read_unsigned_long();
				for(long c=0;c<Count;++c)
				{
					InvoiceDetail *pInvoiceDetail = new InvoiceDetail();
					pInvoiceDetail->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					p_listInvoiceDetail->push_back(pInvoiceDetail);
				}
				return true; // We handled it
			}
			break;
		};
	}

	pcDataBlock = m_pcDataBlock; dwAllocatedSize = m_dwAllocatedSize; pcCurrentPosition = m_pcCurrentPosition;
	return true;
}
