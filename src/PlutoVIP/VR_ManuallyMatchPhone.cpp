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
#include "VR_ManuallyMatchPhone.h"
#include "VIPShared/VIPMenu.h"
#include "PlutoUtils/DBHelper.h"

#include "RA/RA_Processor.h"
#include "DCE/DCEMySqlConfig.h"

#include <assert.h>
#include <sstream>
#include <iostream>
using namespace std;

VR_ManuallyMatchPhone::VR_ManuallyMatchPhone(unsigned long EstablishmentID,
		string sBluetooth,unsigned long long MacAddress,unsigned long IdentifiedPlutoId,string IdentiedPlutoIdPin) 
	: RA_Request()
{
	// Request is of the form:
	// AAAAIIIIdescription+null+CashierName+null

	m_iEstablishmentID=EstablishmentID;
	m_sBluetoothID=sBluetooth;
	m_iMacAddress=MacAddress;
	m_iIdentifiedPlutoId=IdentifiedPlutoId;
	m_sIdentifiedPlutoIdPin=IdentiedPlutoIdPin;
}

bool VR_ManuallyMatchPhone::ProcessRequest(RA_Processor *pRA_Processor)
{
#ifdef VIPSERVER

	DCEMySqlConfig *pDCEMySqlConfig = dynamic_cast<DCEMySqlConfig *>(pRA_Processor->m_pRA_Config);
	assert(NULL != pDCEMySqlConfig);

	if( !m_iMacAddress )
	{
		cout << "Request from " << m_iEstablishmentID << " had empty MAC"; 
		m_cProcessOutcome=INVALID_REQUEST_DATA;
		return true;
	}

	PlutoSqlResult rsPlutoId;
	MYSQL_ROW PlutoIdRow=NULL;

	cout << "Manual Match from " << m_iEstablishmentID << 
		" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin; 

	ostringstream s;
	s << "SELECT RecordVersion FROM PlutoId WHERE PKID_PlutoId=" << m_iIdentifiedPlutoId << " AND PIN='" << m_sIdentifiedPlutoIdPin << "'";
	if( 
		(rsPlutoId.r = pDCEMySqlConfig->MySqlQueryResult(s.str())) && 
		(PlutoIdRow=mysql_fetch_row(rsPlutoId.r)) 
	)
	{
		cout << "Match OK " << m_iEstablishmentID << 
			" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin << " adding to database"; 
		// Add it to the MacAddress table
//		sprintf(sql,"INSERT INTO MacAddress(PKID_MacAddress,FK_MasterUsers,FKID_C_PhoneStatus) "
//			"VALUES(%I64d,%d,%d)",m_iMacAddress,m_iIdentifiedPlutoId,C_PHONESTATUS_NEW_PHONE_CONST);

		s.str("");
		s << "INSERT INTO MacAddress(PKID_MacAddress,FK_MasterUsers,FKID_C_PhoneStatus) " <<
			"VALUES(" << m_iMacAddress << "," << m_iIdentifiedPlutoId << "," << C_PHONESTATUS_NEW_PHONE_CONST << ")";


		cout << "Executing: " << s << "\n";
	
		pDCEMySqlConfig->ThreadedMysqlQuery(s.str());
		m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}
	else
		m_cProcessOutcome=INVALID_REQUEST_DATA;

#endif
	return true;
}
