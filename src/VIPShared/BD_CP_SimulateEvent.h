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
#if !defined(__BD_CP_SIMULATEEVENT_H__)
#define __BD_CP_SIMULATEEVENT_H__

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_SimulateEvent : public BDCommand
{
	long m_EventType; // reserved
	long m_key;

public:
	BD_CP_SimulateEvent()  
	{
		m_EventType = 0;
	}

	BD_CP_SimulateEvent(
		long EventType,
		long key
	);

	~BD_CP_SimulateEvent();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SIMULATE_EVENT; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Simulate event"; };
};

#endif //__BD_CP_SIMULATEEVENT_H__