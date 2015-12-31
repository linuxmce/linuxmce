/*
 sqlCVSrequests

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

/**
 *
 * @file sqlCVSrequests.cpp
 * @brief source file    @todo ask
 *
 */


#include "sqlCVSrequests.h"
#include "RA/RA_Processor.h"
#include "R_CommitChanges.h"
#include "R_CommitTable.h"
#include "R_CommitRow.h"
#include "R_GetRow.h"
#include "R_GetAll_psc_id.h"
#include "R_UpdateRepository.h"
#include "R_UpdateTable.h"
#include "R_CloseTransaction.h"
#include "R_ApproveBatch.h"
#include "R_GetHashedTableStats.h"
#include "R_GetConditional_psc_id.h"

#include "A_UpdateRow.h"
#include "A_UpdateSchema.h"
#include "sqlCVSprocessor.h"
#include "RA/RAServerSocket.h"

RA_Request *RA_Processor::BuildRequestFromData(uint32_t dwSize, const char *pcData, uint32_t dwRequestID )
{
	RA_Request *pRequest=NULL;
	switch( dwRequestID )
	{
	case R_COMMIT_CHANGES:
		pRequest=new R_CommitChanges( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_COMMIT_TABLE:
		pRequest=new R_CommitTable( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_COMMIT_ROW:
		pRequest=new R_CommitRow( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_GET_ROW:
		pRequest=new R_GetRow( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_GET_ALL_PSC_ID:
		pRequest=new R_GetAll_psc_id( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_UPDATE_REPOSITORY:
		pRequest=new R_UpdateRepository( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_UPDATE_TABLE:
		pRequest=new R_UpdateTable( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_CLOSE_TRANSACTION:
		pRequest=new R_CloseTransaction( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_APPROVE_BATCH:
		pRequest=new R_ApproveBatch( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_GET_HASHED_TABLE_STATS:
		pRequest=new R_GetHashedTableStats( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	case R_GET_CONDITIONAL_PSC_ID:
		pRequest=new R_GetConditional_psc_id( );
		pRequest->CreateRequest( dwSize, pcData );
		return pRequest;
	};

	return pRequest;
}

RA_Action *RA_Processor::BuildActionFromData( uint32_t dwSize, const char *pcData, uint32_t dwActionID )
{
	RA_Action *pRA_Action=NULL;
	switch( dwActionID )
	{
	case A_UPDATE_ROW:
		pRA_Action=new A_UpdateRow( );
		pRA_Action->CreateAction( dwSize, pcData );
		return pRA_Action;
	case A_UPDATE_SCHEMA:
		pRA_Action=new A_UpdateSchema( );
		pRA_Action->CreateAction( dwSize, pcData );
		return pRA_Action;
	}
	return pRA_Action;
}

RA_Processor *RA_Processor::CreateRA_Processor(class RAServerSocket *pRAServerSocket,class RA_Config *pRA_Config/* = NULL*/)
{
	if( g_psqlCVSprocessor )
	{
		cout << "There is still another processor open for " << (int) (time(NULL) - g_psqlCVSprocessor->tTime_Creation) <<
			" seconds for IP: " << g_psqlCVSprocessor->m_pRAServerSocket->m_sIPAddress << endl;
		
		int iInactivityTime = (int) (time(NULL) - g_psqlCVSprocessor->tTime_LastActivity);
		
		cout << "It is inactive " << iInactivityTime << " seconds. ";
		if (iInactivityTime<g_GlobalConfig.m_iServerProcessorTimeout)
		{
			cout << "Remote client has to wait, telling him we are busy" << endl;
			pRAServerSocket->SendString("BUSY_RETRY");
			return NULL;
		}
		else
		{
			cout << "Closing inactive processor, creating new one for the new client" << endl;
			delete g_psqlCVSprocessor;
			g_psqlCVSprocessor = NULL;
		}
	}
	else
	{
			cout << "Creating new processor for the new client" << endl;
	}

	g_psqlCVSprocessor = new sqlCVSprocessor( pRAServerSocket );
	printf("Setting g_psqlCVSprocessor %p\n",g_psqlCVSprocessor);
	return g_psqlCVSprocessor;
}

