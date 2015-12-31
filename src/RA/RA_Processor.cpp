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
/**
 *
 * @file RA_Processor.cpp
 * @brief source file for the request processor class
 * @author
 * @todo notcommented
 *
 */

 /**
  *
  * Copyright Notice goes here
  *
  */



#ifndef SYMBIAN

#include <iostream>
#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"
#include "DCE/Socket.h"
#include "PlutoUtils/MyStl.h"

#else

#include "VIPShared/VIPIncludes.h"

#endif


#include "RA_Processor.h"
#include "RA_Request.h"
#include "RA_Action.h"

#define REQHEADER "PVIPVVEEEENN"
#define HEADER "ZZZZCCCC"
#define RESPHEADER "----VVZZZZCCCC"
#define PVPR 1380996688
#define PVIP 1346983504
#define TENMEGA 10000000
#define RESPONSE "PVPRVVZZZZCCCC"

#include "RAClientSocket.h"

bool RA_Processor::SendRequests( string sServerAddress, DCE::Socket **ppSocket )
{
	if( NULL != ppSocket && NULL != *ppSocket )
		return SendRequests(*ppSocket);  // We were already passed in a socket

	RAClientSocket *pSocket;
	int iAttemptsCount=0;
	while(true)
	{
		iAttemptsCount++;
		
		// Connecting to the server
		pSocket = new RAClientSocket( 1, sServerAddress, "foo" ); // Freed before the endif or any of the returns
		if( !pSocket->Connect() )
		{
    		/** @todo check comment */
    		//cout << "Could not connect.\n"; }
			return false; // Connection could not be established
		}

		string sResult;
		bool bResult = pSocket->ReceiveString(sResult);
		if( sResult=="OK" )
			break; // The server is ready
		if( sResult=="BUSY_RETRY" )
		{
			cout << "The server is busy processing another request.";
			
			if (m_iMaxConnectionAttempts!=-1 && iAttemptsCount>=m_iMaxConnectionAttempts)
			{
				cerr << "Maximal count of connection attempts (" << m_iMaxConnectionAttempts << ") reached, giving up" << endl;
				cout << endl << "Maximal count of connection attempts (" << m_iMaxConnectionAttempts << ") reached, giving up" << endl;
				delete pSocket;
				return false;
			}
			
			cout << "  Waiting 5 seconds." << endl;
			delete pSocket;
			Sleep(5000);
			continue;
		}
		cerr << "The server cannot process now.  Result was: " << bResult << " " << sResult << endl;
		cout << "The server cannot process now.  Result was: " << bResult << " " << sResult << endl;
		delete pSocket;
		return false;
	}

	bool bResult = SendRequests(pSocket);
	if( ppSocket )
		*ppSocket = pSocket;
	else
	    delete pSocket;
    return bResult;
}

bool RA_Processor::SendRequests(DCE::Socket *pSocket)
{
	m_pSocket = pSocket;
    // Merge in the new requests
    MYSTL_ITERATE_LIST( m_listNewRequests, RA_Request, pRequest, itList )
    {
        MYSTL_ADDTO_LIST( m_listRequests, pRequest );
    }

    m_listNewRequests.clear();
    if( m_listRequests.size()==0 )
        return false; // No request added

    // Building the request header
    char acRequestHeader[13] = REQHEADER;
    uint16_t *usi = (uint16_t *)(acRequestHeader + 4);
    *usi = (uint16_t) m_dwSoftwareVersion;
    uint32_t *ui = (uint32_t *)(acRequestHeader + 6);
    *ui = m_dwEstablishmentID;
    usi = (uint16_t *) (acRequestHeader + 10);
    *usi = (uint16_t) m_listRequests.size();

    // Sending the header data
    pSocket->SendData( 12, acRequestHeader );

    // Iterating through the requests list
    for( itList=m_listRequests.begin(); itList != m_listRequests.end(); ++itList )
    {
        RA_Request *pRequest = (*itList);
        pRequest->ConvertRequestToBinary();

        // Building the request header
        char acRequestHeader[13] = HEADER;
        ui = (uint32_t *)(acRequestHeader);
        *ui = pRequest->RequestSize();
        ui = (uint32_t *)(acRequestHeader+4);
        *ui = pRequest->RequestChecksum();
        ui = (uint32_t *)(acRequestHeader+8);
        *ui = pRequest->ID();

        // Sending the header data
        pSocket->SendData( 12, acRequestHeader );
        pSocket->SendData( pRequest->RequestSize(),(const char *)pRequest->Request() );

        // Get the response
		// RESPHEADER "----VVZZZZCCCC"
        // The header should be of the format PVPRVVZZZZCCCC---NNXXZZZZCCCC where PVIP is required
        // VV is a 16 bit integer indicating the version, or formatting of the message (for future use) - always same as incoming
        // ZZZZ is the 32-bit size of the response, CCCC is the 32-bit checksum of the response
        // --- is the data for the response
        // where N is a 2 byte, 16 integer indicating the number of actions coming in
        // repeat: where XX is a 2 byte, 16 bit integer representating the type of action
        char acResponseHeader[15] = RESPHEADER;

        if ( !pSocket->ReceiveData( 14, acResponseHeader ) )  // Not a valid message
        {
            fprintf(stderr,"invalid response 1 %s\n",acResponseHeader);
            return false;
        }

        // Parsing the response header
        uint32_t *pdwSanityCheck = (uint32_t *)acResponseHeader;
        uint16_t *piVersion = (uint16_t *)(acResponseHeader + 4);
        uint32_t *pdwSize = (uint32_t *)(acResponseHeader + 6);
        uint16_t *piCheckSum = (uint16_t *)(acResponseHeader + 10);

        if( *pdwSanityCheck != PVPR )  // PVPR
        {
            fprintf(stderr,"invalid response 1b %d\n",(*pdwSanityCheck));
            return false;
        }

        // Testing for errors

        char *pcData = new char[*pdwSize]; // Freed inside the for before one of the returns
        if( !pSocket->ReceiveData( *pdwSize, pcData ) )
        {
            delete pSocket;
            delete[] pcData;
            fprintf( stderr, "invalid response 2\n" );
            return false;
        }

        pRequest->ParseResponse( *pdwSize, pcData );

        char caNumActions[3];
        if ( !pSocket->ReceiveData( 2, caNumActions ) )  // Not a valid message
        {
            delete pSocket;
            delete[] pcData;
            fprintf( stderr, "invalid response 3\n" );
            return false;
        }

        uint16_t *iNumActions = (uint16_t *) caNumActions;
        for( long lActionCount=0; lActionCount < *iNumActions; ++lActionCount )
        {
            char acActionHeader[13] = HEADER;
            if ( !pSocket->ReceiveData( 12, acActionHeader ) )  // Not a valid message
            {
                delete pSocket;
                delete[] pcData;
                fprintf( stderr, "invalid response 4\n" );
                return false;
            }

            uint32_t *plActionSize = (uint32_t *)acActionHeader;
            uint32_t *plActionChecksum = (uint32_t *)(acActionHeader + 4);
            uint32_t *plActionID = (uint32_t *)(acActionHeader + 8);

            char *pcActionData = new char[*plActionSize];
            if( !pSocket->ReceiveData( *plActionSize, pcActionData ) )
            {
                delete pSocket;
                delete[] pcActionData;
                delete[] pcData;
                fprintf( stderr, "invalid response 5\n" );
                return false;
            }

            RA_Action *pAction = BuildActionFromData( *plActionSize, pcActionData, *plActionID );
            pAction->ProcessAction(pRequest,this);
            MYSTL_ADDTO_LIST( m_listActions, pAction );
            delete[] pcActionData;
        }
        delete[] pcData;
    }

    m_listRequests.clear();
	return true;
}

bool RA_Processor::SendRequest(RA_Request *pRequest,DCE::Socket *pSocket)
{
	AddRequest(pRequest);
	while( SendRequests(pSocket) );
	return true;
}

bool RA_Processor::ReceiveRequests(DCE::Socket *pSocket)
{
	m_pSocket = pSocket;

    char acRequestHeader[13] = HEADER;

    if (!pSocket->ReceiveData( 12, acRequestHeader ) )  // Not a valid message
        return false;

    // The header should be of the format PVIPVVEEEENNXXZZZZCCCC
    // Where PVIP is required
    // VV is a 16 bit integer indicating the version, or formatting of the message (for future use)
    // Where EEEE is a 32 bit integer with the establishment ID
    // where N is a 2 byte, 16 integer indicating the number of requests coming in (normally 01)
    // where XX is a 2 byte, 16 bit integer representating the type of request
    // ZZZZ is the 32-bit size of the request, CCCC is the 32-bit checksum of the request
    uint32_t *pdwSanityCheck = (uint32_t *)acRequestHeader;
    uint16_t *piNumberOfRequests = (uint16_t *)(acRequestHeader + 10);

    if( *pdwSanityCheck != PVIP )  // PVIP
        return false;

    cout << "Data valid " << (*piNumberOfRequests) << " new requests\n";

    for( uint16_t iCount=0; iCount < *piNumberOfRequests; ++iCount)
    {
        char acRequestItem[13] = HEADER;
        if ( !pSocket->ReceiveData( 12, acRequestItem ) )  // Not a valid message
            return false;

        uint32_t *pdwSize = (uint32_t *)(acRequestItem);
        uint32_t *RequestID = (uint32_t *)(acRequestItem+8);

        if( *pdwSize > TENMEGA )  // Can't be more than 10MB
            return false;

        char *acData = new char[*pdwSize]; // freed at the end of the procedure or before any of the returns
        if( !pSocket->ReceiveData( *pdwSize, acData ) )
        {
	    /** @todo check comment */
	    // cout << "Got bogus data: sc " << SanityCheck << " size " << Size " checksum " << CheckSum << " est " << Establishment; }
            delete[] acData;
            return false;
        }


        if( !true )
        {
	    /** @todo check comment */
	    //cout << "Checksum does not match: sc " << SanityCheck << " size " << Size << " checksum " << CheckSum << " estab " << Establishment;
            delete[] acData;
            return false;
        }

        RA_Request *pRA_Request = BuildRequestFromData( *pdwSize, acData, *RequestID );

		if( !pRA_Request )
		{
			cerr << "Cannot build incoming request" << endl;
			return false;
		}
        cout << "Built request ID " << pRA_Request->ID() << "\n";

        pRA_Request->ProcessRequest(this);
        pRA_Request->ConvertResponseToBinary();

        char acResponse[15] = RESPONSE;
        char *pcPtr = (acResponse + 4);
        pcPtr += sizeof(uint16_t);

        pdwSize = (uint32_t *) pcPtr;
        *pdwSize = pRA_Request->ResponseSize();
        pcPtr += sizeof(uint16_t);

        /** \todo calc checksum */

        // Sendin response data
        pSocket->SendData(14,acResponse);
        pSocket->SendData(*pdwSize,(const char *)pRA_Request->Response());
        uint16_t iNumActions = (uint16_t)pRA_Request->m_listActions.size();
        pSocket->SendData(2,(const char *) &iNumActions);

        cout << "Returning " << iNumActions << " actions\n";

        MYSTL_ITERATE_LIST( pRA_Request->m_listActions, RA_Action, pAction, itList)
        {
            pAction->ConvertActionToBinary();
            char acResponse[13] = HEADER;
            uint32_t *dwUi = (uint32_t *) (acResponse);
            *dwUi = pAction->ActionSize();
            dwUi = (uint32_t *) (acResponse+4);
            *dwUi = pAction->ActionChecksum();
            dwUi = (uint32_t *) (acResponse+8);
            *dwUi = pAction->ID();
            pSocket->SendData(12,acResponse);
            pSocket->SendData(pAction->ActionSize(),(const char *)pAction->Action());
        }

        delete[] acData;
        delete pRA_Request;
    }

    return true;
}

void RA_Processor::RemoveRequest(RA_Request *pRequest_Del)
{
    MYSTL_ITERATE_LIST( m_listNewRequests, RA_Request, pNewRequest, itNewList )
    {
        if( pNewRequest==pRequest_Del )
			m_listNewRequests.erase( itNewList );
		break;
    }

    MYSTL_ITERATE_LIST( m_listRequests, RA_Request, pRequest, itList )
    {
        if( pRequest==pRequest_Del )
			m_listNewRequests.erase( itList );
		break;
    }
}

