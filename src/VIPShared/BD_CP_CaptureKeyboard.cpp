/*
 BD_CP_CaptureKeyboard
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#ifdef SMARTPHONE
#include "Orbiter/CENet_Smartphone/OrbiterApp.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_CaptureKeyboard.h"


BD_CP_CaptureKeyboard::BD_CP_CaptureKeyboard(
	bool bOnOff, bool bDataGrid, bool bReset, int iEditType, int iVariable, string sText) 
{
	m_bOnOff = bOnOff;
	m_bReset = bReset;
	m_iEditType = iEditType;
	m_bDataGrid = bDataGrid;
	m_iVariable = iVariable;
	m_sText = sText;
}

void BD_CP_CaptureKeyboard::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_bOnOff);
	Write_unsigned_char(m_bReset);
	Write_long(m_iEditType);
	Write_unsigned_char(m_bDataGrid);
	Write_long(m_iVariable);
	Write_string(m_sText);
}

void BD_CP_CaptureKeyboard::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);

	m_bOnOff = Read_unsigned_char()==1;
	m_bReset = Read_unsigned_char()==1;
	m_iEditType = Read_long();
	m_bDataGrid = Read_unsigned_char()==1;
	
	m_iVariable = Read_long();
	Read_string(m_sText);
}


bool BD_CP_CaptureKeyboard::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

#ifdef SYMBIAN 
	LOG("#	Received 'CaptureKeyboard' command  #\n");
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->SetCaptureKeyboardCommand(m_bOnOff, m_bDataGrid, m_bReset, m_iEditType, m_iVariable, m_sText);
#endif

#ifdef SMARTPHONE
	OrbiterApp::GetInstance()->SetCaptureKeyboard(m_bOnOff, m_bDataGrid, m_bReset, m_iEditType, m_iVariable, m_sText);
#endif

#endif
	return true;
}

