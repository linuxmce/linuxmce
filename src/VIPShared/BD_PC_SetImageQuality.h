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
#ifndef BD_PC_SetImageQuality_h
#define BD_PC_SetImageQuality_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_SetImageQuality : public BDCommand 
{
public:

    unsigned long m_nImageQuality;

    BD_PC_SetImageQuality() {m_nImageQuality = 0;}
	BD_PC_SetImageQuality(unsigned long nImageQuality);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_SET_IMAGE_QUALITY; }
	virtual const char *Description() { return "Set Image Quality"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
