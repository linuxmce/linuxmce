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
#ifndef Tribuneprocessor_h
#define Tribuneprocessor_h

#include "RA/RA_Processor.h"
#include "CommonFunctions.h"
#include "Database.h"


using namespace Tribune;
class Tribuneprocessor;
extern Tribuneprocessor *g_pTribuneprocessor;

/**
 * @brief request action processor class
 */
class Tribuneprocessor : public RA_Processor
{
public:
	time_t tTime_Creation;
	time_t tTime_LastActivity;
	class RAServerSocket *m_pRAServerSocket;
	
	/**< This will create a transaction for every processor that will rollback automatically if the processor goes away with an R_CloseTransaction */
	SafetyTransaction st;

	void LogActivityTime()
	{
		tTime_LastActivity = time(NULL);
		
	}

	/** @brief constructor assigning null to all the pointers */
	Tribuneprocessor( RAServerSocket *pRAServerSocket ) : 
		RA_Processor( 0, 1, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount ), 
		st(g_GlobalConfig.m_pDatabase) 
	{ 
		//m_pTable=NULL; m_pvectFields=NULL; m_pRepository=NULL; 
		tTime_LastActivity = tTime_Creation = time(NULL);  m_pRAServerSocket=pRAServerSocket;
	}
	
	virtual ~Tribuneprocessor( ) 
	{ 
		if( st.m_bIsOpen_get() ) 
			st.Rollback();

		if( g_pTribuneprocessor==this )
		{
			g_pTribuneprocessor=NULL; 
			printf("Clearing g_pTribuneprocessor %p\n",this);
		}
	}

};

#endif
