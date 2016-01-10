/*
 BD_CP_ShowVMC
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#include "VIPShared/PlutoConfig.h"
#endif
#else
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "PlutoUtils/PlutoDefs.h"
#include "VIPShared/VIPMenu.h"
#include "BD_CP_ShowVMC.h"

BD_CP_ShowVMC::BD_CP_ShowVMC(unsigned char Store,unsigned long VMCSize,const char *pVMC,
							 unsigned long iVMCFileNameSize, char *pVMCFileName) 
	
{
	m_iStore = Store;
	m_iVMCSize=VMCSize;
	m_pVMC=(char *)malloc(m_iVMCSize);
	memcpy(m_pVMC,pVMC,m_iVMCSize);

	m_iVMCFileNameSize = iVMCFileNameSize;
	m_pVMCFileName=(char *)malloc(m_iVMCFileNameSize);	
	memcpy(m_pVMCFileName, pVMCFileName, m_iVMCFileNameSize);
}	

BD_CP_ShowVMC::~BD_CP_ShowVMC()
{
	PLUTO_SAFE_DELETE(m_pVMC);
	PLUTO_SAFE_DELETE(m_pVMCFileName);
}

void BD_CP_ShowVMC::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_iStore);
	Write_long(m_iVMCSize);
	Write_block(m_pVMC,m_iVMCSize);
	Write_long(m_iVMCFileNameSize);
	Write_block(m_pVMCFileName, m_iVMCFileNameSize);
}

void BD_CP_ShowVMC::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size, data);

	m_iStore = Read_unsigned_char();
	m_iVMCSize = Read_long();
	m_pVMC = Read_block(m_iVMCSize);
	m_iVMCFileNameSize = Read_long();
	m_pVMCFileName = Read_block(m_iVMCFileNameSize);
}

bool BD_CP_ShowVMC::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

#ifdef SYMBIAN
	LOG("#	Received 'ShowVMC' command  #\n");
	
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->UpdateScreen(m_iStore, m_iVMCSize, m_pVMC, 
		m_iVMCFileNameSize, m_pVMCFileName);
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Show();
#endif

#ifdef VIPDESIGN
	VIPMenuCollection *pVMC = new VIPMenuCollection(VMCSize, m_pcCurrentPosition);
	g_pPlutoConfig->m_pDoc->m_pMenuCollection=pVMC;
	g_pPlutoConfig->m_pDoc->m_pMenu=pVMC->m_pMenu_Starting;
	g_pPlutoConfig->m_pDoc->InvalidateAllViews();
#endif

#endif

	return true;
}
