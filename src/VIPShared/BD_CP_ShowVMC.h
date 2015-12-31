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
#ifndef BD_CP_ShowVMC_h
#define BD_CP_ShowVMC_h

#include "BD/BDCommand.h"
#include "PlutoPhoneCommands.h"

class BD_CP_ShowVMC : public BDCommand
{
	char *m_pVMC;
	unsigned char m_iStore;
	unsigned long m_iVMCSize;
	unsigned long m_iVMCFileNameSize;
	char *m_pVMCFileName;

public:
	BD_CP_ShowVMC()  {}
	BD_CP_ShowVMC(unsigned char Store,unsigned long VMCSize,const char *pVMC,
		unsigned long iVMCFileNameSize, char *pVMCFileName);
	~BD_CP_ShowVMC(); 

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SHOW_VMC; }
	virtual const char *Description() { return "Show VMC"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};
#endif
