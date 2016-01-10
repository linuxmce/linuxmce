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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/DBHelper.h"
#include "VR_PhoneRespondedToRequest.h"
#include "VA_UpdateTransaction.h"
#include "VIPShared/VIPMenu.h"
#include "RA/RA_Request.h"

#include "RA/RA_Processor.h"
#include "DCE/DCEMySqlConfig.h"

#include <assert.h>
#include <iostream>
#include <sstream>

VR_PhoneRespondedToRequest::VR_PhoneRespondedToRequest(unsigned long RequestID,unsigned long RequestSize,const char *RequestData,unsigned long ResponseSize,const char *ResponseData)
	: RA_Request()
{
	m_iRequestID=RequestID;
	m_pdbOriginalRequest.m_dwSize=RequestSize;
	m_pdbOriginalRequest.m_pBlock = new char[RequestSize];
	memcpy(m_pdbOriginalRequest.m_pBlock,RequestData,RequestSize);

	m_pdbOriginalResponse.m_dwSize=ResponseSize;
	m_pdbOriginalResponse.m_pBlock = new char[ResponseSize];
	memcpy(m_pdbOriginalResponse.m_pBlock,ResponseData,ResponseSize);

}

bool VR_PhoneRespondedToRequest::ProcessRequest(RA_Processor *pRA_Processor)
{
#ifdef VIPSERVER

	DCEMySqlConfig *pDCEMySqlConfig = dynamic_cast<DCEMySqlConfig *>(pRA_Processor->m_pRA_Config);
	assert(NULL != pDCEMySqlConfig);

	RA_Request *pRequest;
	if( m_pdbOriginalRequest.m_dwSize )
		pRequest = pRA_Processor->BuildRequestFromData(m_pdbOriginalRequest.m_dwSize,m_pdbOriginalRequest.m_pBlock, m_iRequestID);
	else
		pRequest = pRA_Processor->BuildRequestFromData(0,NULL,m_iRequestID);

	pRequest->ParseResponse(m_pdbOriginalResponse.m_dwSize,m_pdbOriginalResponse.m_pBlock);
	if( m_iRequestID==VRP_REQUEST_SHOW_MENU )
	{
		/*
		VR_ShowMenu *pShowMenu = (VR_ShowMenu *) pRequest;
		if( pShowMenu->m_iMenuCollectionID==VIPMENUCOLL_SECURE_PAYMENT )
		{
			long Amount=-1;
			long PaymentMethod=-1;
			string PinCode;
			long TransNumber=-1;

			MYSTL_ITERATE_LIST(pShowMenu->m_listReturnedVariables,VIPVariable,pVariable,itListVariables)
			{
				if( pVariable->m_iVariableID==VIPVAR_PAYMENT_AMOUNT_USER )
					Amount=atoi(pVariable->m_sCurrentValue.c_str());
				else if( pVariable->m_iVariableID==VIPVAR_PAYMENT_METHOD )
					PaymentMethod=atoi(pVariable->m_sCurrentValue.c_str());
				else if( pVariable->m_iVariableID==VIPVAR_PIN_CODE )
					PinCode=pVariable->m_sCurrentValue;
				else if( pVariable->m_iVariableID==VIPVAR_VIP_TRANS_NUM )
					TransNumber=atoi(pVariable->m_sCurrentValue.c_str());
			}

			std::ostringstream sql;
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);

			// Confirm we have a record of the transaction
			PlutoSqlResult rsTrans;
			MYSQL_ROW TransRow=NULL;
			sql << "SELECT Amount FROM PmtTrans WHERE PKID_PmtTrans=" << TransNumber;
			if( (rsTrans.r = pDCEMySqlConfig->MySqlQueryResult(sql.str()))==NULL || 
				(TransRow=mysql_fetch_row(rsTrans.r))==NULL )
			{
				cout << "CANNOT FIND TRANSACTION: " << sql; 
				m_cProcessOutcome = INVALID_REQUEST_DATA;
				return true;
			}

			if( Amount==-1 )
				Amount = atoi(TransRow[0]);

			if( pShowMenu->m_iMenuTermination!=CLOSE_WITH_OK )
			{
				sql.str("");
				sql << "UPDATE PmtTrans SET Result=-3 WHERE PKID_PmtTrans=" << TransNumber;
				pDCEMySqlConfig->ThreadedMysqlQuery(sql.str());

				VA_UpdateTransaction *pUP = new VA_UpdateTransaction(TransNumber,-3,"cancelled");
				m_listActions.push_back(pUP);
				m_cProcessOutcome = SUCCESSFULLY_PROCESSED;
			}

			// 0=Not processed, 1=approved, -1=declined, -2=referred, -3=user cancelled
			int Result=1; 
			string ResultStr="28228";

			// Process the transaction
			sql << "INSERT INTO PmtTrans_Auth(FKID_PmtTrans,DateTime,Result,ResultStr) VALUES("
				<< TransNumber << ",'" << StringUtils::SQLDateTime() << "'," << Result << ",'"
				<< StringUtils::SQLEscape(ResultStr) << "');";

			int PKID_PmtTrans_Auth = pDCEMySqlConfig->ThreadedMysqlQueryWithID(sql.str());
			if( PKID_PmtTrans_Auth==0 )
			{
				cout << "FAILED: " << sql; 
				m_cProcessOutcome = INTERNAL_ERROR;
				return true;
			}

			sql.str("");
			sql << "UPDATE PmtTrans SET Result=" << Result << " WHERE PKID_PmtTrans=" << TransNumber;
			pDCEMySqlConfig->ThreadedMysqlQuery(sql.str());


			VA_UpdateTransaction *pUP = new VA_UpdateTransaction(TransNumber,Result,ResultStr);
			m_listActions.push_back(pUP);
			m_cProcessOutcome = SUCCESSFULLY_PROCESSED;
		}
		*/
	}
#endif
	return true;
}
