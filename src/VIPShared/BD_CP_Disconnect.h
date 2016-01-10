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
#ifndef BD_CP_Disconnect_h
#define BD_CP_Disconnect_h

#include "BD/BDCommand.h"
#include "PlutoPhoneCommands.h"

class BD_CP_Disconnect : public BDCommand 
{
public:
	BD_CP_Disconnect()  {}
	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_DISCONNECT; }
	virtual const char *Description() { return "Disconnect"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor) { return true; }
};


#endif
