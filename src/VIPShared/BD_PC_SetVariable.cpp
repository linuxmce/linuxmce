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
#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_PC_SetVariable.h"

#ifdef BLUETOOTH_DONGLE
#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

BD_PC_SetVariable::BD_PC_SetVariable(int iVariable, string sVariableValue) 
	
{
	m_iVariable = iVariable;
	m_sVariableValue = sVariableValue;

#ifdef SYMBIAN
	LOG("#	Sending 'SetVariable' command  #\n");
#endif 
}

void BD_PC_SetVariable::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_iVariable);
	Write_string(m_sVariableValue);
}

void BD_PC_SetVariable::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_iVariable = Read_long();
	Read_string(m_sVariableValue);
}

bool BD_PC_SetVariable::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef BLUETOOTH_DONGLE
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pBluetooth_Dongle->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter)
	{
		//TODO: log this
		return false;
	}


	pOrbiter->m_pOrbiter->CaptureKeyboard_UpdateVariableAndText(m_iVariable, m_sVariableValue);

	/*
	pOrbiter->m_pOrbiter->m_mapVariable[m_iVariable] = m_sVariableValue;
    pOrbiter->m_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_CaptureKeyboard_pText->m_pObject);

	pOrbiter->m_pOrbiter->RenderScreen();
	*/

#endif

	return true;
}

