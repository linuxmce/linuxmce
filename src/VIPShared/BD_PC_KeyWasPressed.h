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
#ifndef BD_PC_KeyWasPressed_h
#define BD_PC_KeyWasPressed_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

//enum EventType { eButtonDown, eButtonUp };

class BD_PC_KeyWasPressed : public BDCommand 
{
	int m_Key;
    int m_EventType; // 0 - ButtonDown, 1 - ButtonUp, 2 - Execute

public:
	BD_PC_KeyWasPressed()  {}
	BD_PC_KeyWasPressed(int Key, int EventType);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_KEY_WAS_PRESSED; }
	virtual const char *Description() { return "Key pressed"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
