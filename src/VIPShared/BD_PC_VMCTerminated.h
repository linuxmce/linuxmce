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
#ifndef BD_PC_VMCTerminated_h
#define BD_PC_VMCTerminated_h

#include "BD/BDCommand.h"
#include "PlutoPhoneCommands.h"

class BD_PC_VMCTerminated : public BDCommand 
{
	char m_iTerminatingResolution;
	unsigned long m_iMenuCollectionID;
	MYSTL_CREATE_LONG_MAP(m_mapVariables,VIPVariable);

public:
	BD_PC_VMCTerminated()  {}
	BD_PC_VMCTerminated(char TerminatingResolution,unsigned long MenuCollectionID);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_VMC_TERMINATED; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
	virtual const char *Description() { return "VMC Terminated"; };
};

#endif
