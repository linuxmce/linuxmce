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
 * @file BDCommandProcessor.h
 * @brief header file for the bidirectional command processor classs
 * @author
 * @todo notcommented
 *
 */

 
#ifndef BDCOMMANDPROCESSOR_H
#define BDCOMMANDPROCESSOR_H

//#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/MyStl.h"

/** @todo make sure the defines names are ok in the files acctually using them */

#define BD_PC_WHAT_DO_YOU_HAVE			1
#define BD_CP_HAVE_NOTHING				100


/**
 * @brief processes the command before sending it
 * @todo ask
 */
class BDCommandProcessor
{

public:

	bool m_bExit; /** < set if the BDCommandProcessor has exited for some reason (ex: has been disconnected). used to terminate scan loops */
	string m_sMacAddressPhone; /** < the mac address of the phone */

	pthread_t m_BDSockThreadID; /** < the bidirectional socket thread ID */

	bool m_bDead; /** < set to tell others having a pointer to the BDCoammandProcessor the processor is dead */
	char *m_pcReceiveCmdHeader; /** < pointer to the received command header, sent before the data */
	char *m_pcReceiveCmdData; /** < pointer to the received command data */
	char *m_pcReceiveAckHeader; /** < pointer to received command acknowledge header send before the ack data */
	char *m_pcReceiveAckData; /** < pointer to received command acknowledge data */
	class BDCommand *m_pCommand_Sent; /** < pointer to the command */

	pluto_pthread_mutex_t m_CommandMutex; /** < protect the list */
	pthread_cond_t m_PollingCond; /** < polling will be done every so often */
	pluto_pthread_mutex_t m_PollingMutex;

	/** 
	 * @brief included for Symbian comapatibility
	 * @see Symbian.txt for detailes 
	 */
	MYSTL_CREATE_LIST( m_listCommands, BDCommand );
	
	/**
	 * @brief constructs the processor starting from the mac address of the phone
	 */
	BDCommandProcessor( string sMacAddressPhone );
	
	/**
	 * @brief destructor, implemented by inheriting classes if neccessary
	 */
	virtual ~BDCommandProcessor() {}
	
	/**
	 * @brief send the command
	 */
	bool SendCommand( bool &bImmediateCallback );
	
	/**
	 * @brief recieve the command
	 */
	bool ReceiveCommand( unsigned long dwType, unsigned long dwSize, char *pcData );

	/**
	 * @brief sends a long using SendData
	 */
	bool SendLong( long l );
	
	/**
	 * @brief receives a long using ReceiveData
	 */
	long ReceiveLong();

	/**
	 * @brief sends data, needs to be overriden by inheriting classes
	 */
	virtual bool SendData( int iSize,const char *pcData)=0;
	
	/**
	 * @brief receives data, needs to be overriden by inheriting classes
	 */
	virtual char *ReceiveData( int iSize )=0;

	/**
	 * @brief adds the command to the command buffer
	 */
	virtual void AddCommand( class BDCommand *pCommand );

	/**
	 * @brief Returns a Pointer to BDCommand based on the given type.  Derived versions should override this for their specific types and call the base class to return standard types.
	 */
	virtual BDCommand *BuildCommandFromData( unsigned long dwType );
};

#endif
