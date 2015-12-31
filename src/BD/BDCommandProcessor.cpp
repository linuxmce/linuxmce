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
 * @file BDCommandProcessor.cpp
 * @brief source file for the bidirec
 * @author
 * @todo notcommented
 *
 */

 /**
  *
  * Copyright Notice goes here
  *
  */

#include "PlutoUtils/MyStl.h"
#include "DCE/Logger.h"

#include "BD/BDCommandProcessor.h"
#include "BD/BDCommand.h"
#include "BD/BD_HaveNothing.h"
#include "BD/BD_WhatDoYouHave.h"

#include "VIPShared/PlutoPhoneCommands.h"

using namespace DCE;

BDCommandProcessor::BDCommandProcessor( string sMacAddressPhone ) 
	: m_CommandMutex("command"), m_PollingMutex("Polling")
{
	m_bDead = false;
	m_bExit = false;
	m_pcReceiveCmdData = NULL;
	m_pcReceiveAckData = NULL;
	m_pcReceiveCmdHeader = NULL;
	m_pcReceiveAckHeader = NULL;
	m_pCommand_Sent = NULL;
	m_sMacAddressPhone = sMacAddressPhone;

	m_CommandMutex.Init( NULL );
	pthread_cond_init( &m_PollingCond, NULL );
	m_PollingMutex.Init( NULL, &m_PollingCond );
}

bool BDCommandProcessor::SendCommand( bool &bImmediateCallback )
{
	bImmediateCallback = false;
	
	// This also sends the Command, and starts receiving the acknowledge
	PLUTO_SAFETY_LOCK( cm, m_CommandMutex );
	if( MYSTL_SIZEOF_LIST( m_listCommands ) == 0 )
	{
#ifdef VIPPHONE
		m_pCommand_Sent = new BD_WhatDoYouHave();
#else
		m_pCommand_Sent = new BD_HaveNothing();
#endif
	}
	else
	{
		MYSTL_EXTRACT_FIRST( m_listCommands, BDCommand, pC );
		m_pCommand_Sent = pC;
	}

	cm.Release();

	m_pCommand_Sent->ConvertCommandToBinary();
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending %s command", m_pCommand_Sent->Description());

	if(!SendLong( m_pCommand_Sent->ID() ))
	{
		delete m_pCommand_Sent;
		m_pCommand_Sent = NULL;
		return false;
	}
	

	if(!SendLong( m_pCommand_Sent->GetCommandOrAckSize() ))
	{
		delete m_pCommand_Sent;
		m_pCommand_Sent = NULL;
		return false;
	}

	if(!SendData( m_pCommand_Sent->GetCommandOrAckSize(), m_pCommand_Sent->GetCommandOrAckData() ))
	{
		delete m_pCommand_Sent;
		m_pCommand_Sent = NULL;
		return false;
	}

	m_pCommand_Sent->FreeSerializeMemory();

	// If we sent a "What do you have", we're going to get a command in return - not a normal acknowledge
	if( NULL != m_pcReceiveAckHeader ) // we didn't send a WhatDoYouHave
	{
		delete m_pcReceiveAckHeader;
		m_pcReceiveAckHeader = NULL;
	}

	if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )  // sent the WhatDoYouHave
		m_pcReceiveAckHeader = ReceiveData(8);
	else
		m_pcReceiveAckHeader = ReceiveData(4);

	/** @todo check comment */
	if( !m_pcReceiveAckHeader ) // got nothing
	{
		delete m_pCommand_Sent;
		m_pCommand_Sent = NULL;

		return false;
	}

	// got something
	
	int *pdwSize;
	if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )
		pdwSize = (int *)(m_pcReceiveAckHeader + sizeof(int)); // header starts from +4
	else
		pdwSize = (int *)(m_pcReceiveAckHeader); // normal header

	delete m_pcReceiveAckData;
	m_pcReceiveAckData = NULL;

	if( *pdwSize )
	{
		if ( NULL != m_pcReceiveAckData ) // got some ack data, clearing it
		{
			delete m_pcReceiveAckData;
			m_pcReceiveAckData = NULL;
		}

		m_pcReceiveAckData = ReceiveData( *pdwSize ); // getting the data for this header
		if( !m_pcReceiveAckData ) // got some
		{
			delete m_pCommand_Sent; // no need for it anymore
			m_pCommand_Sent = NULL;
			return false;
		}
	}

	if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE ) // the ack will be a command that's parsed directly
	{
		int *pdwType = (int *)m_pcReceiveAckHeader;
		int *pdwSize = (int *)(m_pcReceiveAckHeader + sizeof(int));
		if(!ReceiveCommand( *pdwType, *pdwSize, m_pcReceiveAckData ))
		{
			delete m_pCommand_Sent;
			m_pCommand_Sent = NULL;
			return false;
		}
		
		if( *pdwType != BD_CP_HAVE_NOTHING )
			bImmediateCallback = true; // that's when I set it on true
	}
	else
	{
		int *pdwSize = (int *)(m_pcReceiveAckHeader);
		m_pCommand_Sent->ParseAck( *pdwSize, m_pcReceiveAckData );
	}
	delete m_pCommand_Sent;
	m_pCommand_Sent=NULL;
	return true;
}

bool BDCommandProcessor::ReceiveCommand( unsigned long dwType, unsigned long dwSize, char *pcData )
{

	// if we're passed in Data, then we explicitly parse it, otherwise we do a three
	// state thing to handle Symbian's lack of multi-threading
	
	if( dwType )
	{
		BDCommand *pCommand = BuildCommandFromData( dwType ); // creates a new command to parse the data

		if(NULL == pCommand)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"# Received unknown command, type %d", dwType);
			return false;
		}

LoggerWrapper::GetInstance()->Write(LV_WARNING,"# Received %s command #", pCommand->Description());

		pCommand->ParseCommand( dwSize, pcData );
		pCommand->ProcessCommand( this ); // takes the command processor and processes it
		pCommand->ConvertAckToBinary();

		if(!SendLong( pCommand->GetCommandOrAckSize() ))
		{
			delete pCommand;
			pCommand = NULL;
			return false;
		}
	
		if(!SendData( pCommand->GetCommandOrAckSize(), pCommand->GetCommandOrAckData() ))
		{
			delete pCommand;
			pCommand = NULL;
			return false;
		}

		pCommand->FreeSerializeMemory();

		if( NULL != pCommand )
		{
			delete pCommand;
			pCommand = NULL;
		}

		return true;
	}
	else
	{
		/** @todo check comment */
		if( NULL != m_pcReceiveCmdHeader )
		{
			delete m_pcReceiveCmdHeader;
			m_pcReceiveCmdHeader = NULL;
		}

		m_pcReceiveCmdHeader = ReceiveData( 8 );

		/** @todo check comment */
		if( !m_pcReceiveCmdHeader ) // no data received
			return false;

		int *dwSize = (int *)(m_pcReceiveCmdHeader + sizeof(int));

		if( *dwSize )
		{
			if(NULL != m_pcReceiveCmdData)
			{
				delete m_pcReceiveCmdData;
				m_pcReceiveCmdData = NULL;
			}

			m_pcReceiveCmdData = ReceiveData(*dwSize);
			if( !m_pcReceiveCmdData )
				return false;
		}
		
		/** @todo check comment */
		int *dwType = (int *) m_pcReceiveCmdHeader;
		dwSize = (int *)(m_pcReceiveCmdHeader + sizeof(int));
		BDCommand *pCommand = BuildCommandFromData( *dwType );

		if(!pCommand)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"# Command %d not implemented! #", *dwType);
			return false;
		}

		if( pCommand->ID() == BD_PC_WHAT_DO_YOU_HAVE ) // received a WhatDoYouHave
		{
			bool bDummy;
			if(!SendCommand( bDummy )) // response to WhatDoYouHave
				return false;
		}
		else // a normal command received
		{
			pCommand->ParseCommand( *dwSize, m_pcReceiveCmdData );
			pCommand->ProcessCommand( this );
			pCommand->ConvertAckToBinary();
			if(!SendLong( pCommand->GetCommandOrAckSize() ))
			{
				delete pCommand;
				pCommand = NULL;
				return false;
			}

			if(!SendData( pCommand->GetCommandOrAckSize(), pCommand->GetCommandOrAckData() ))
			{
				delete pCommand;
				pCommand = NULL;
				return false;
			}

			pCommand->FreeSerializeMemory();
		}

		return true;
	}
	return false; // never get here
}

bool BDCommandProcessor::SendLong(long l)
{
	//serialize it as int (we are talking with a 32bit cellphone ;) )
	int value = static_cast<int>(l);
	return SendData(sizeof(int), (const char *)&value );
}

long BDCommandProcessor::ReceiveLong()
{
	//deserialize it as int (we are talking with a 32bit cellphone ;) )
	const char *pcData = ReceiveData(sizeof(int));
	int *l = (int *)pcData;
	return static_cast<long>(*l);
}

void BDCommandProcessor::AddCommand( class BDCommand *pCommand )
{
	PLUTO_SAFETY_LOCK( cm, m_CommandMutex );
	
	if(m_bDead)
		return;

#ifndef SYMBIAN
    if(pCommand->ID() == BD_CP_SHOW_IMAGE)
    {
        list<BDCommand *>::iterator it;
        for(it = m_listCommands.begin(); it != m_listCommands.end();)
        {
            BDCommand *p = *it;

            if(p->ID() == BD_CP_SHOW_IMAGE)
                m_listCommands.erase(it++);
            else
                it++;
        }
    }
#endif

    MYSTL_ADDTO_LIST( m_listCommands, pCommand );

	// If we're sitting waiting for a keypress, be sure to wake up that thread now
	pthread_cond_broadcast( &m_PollingCond );
}

// Whatever project uses this must implement this function
extern class BDCommand *BuildCommandFromData( unsigned long dwType );

BDCommand *BDCommandProcessor::BuildCommandFromData( unsigned long dwType )
{
	switch( dwType )
	{
	case BD_PC_WHAT_DO_YOU_HAVE:
		return new BD_WhatDoYouHave();

	case BD_CP_HAVE_NOTHING:
		return new BD_HaveNothing();
	default:
		// Log Error
		return ::BuildCommandFromData(dwType);
	};
}

