/*
 BD_CP_MenuData
 
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
#endif
#endif

#ifndef SYMBIAN
#include "BD/BDCommandProcessor.h"
#else
#include "PlutoMOAppUi.h"
#include "Logger.h" 
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_MenuData.h"

#ifdef SMARTPHONE
#include "Orbiter/CENet_Smartphone/OrbiterApp.h"
#endif

BD_CP_MenuData::BD_CP_MenuData( MenuData& data ) 
{
	m_Data = data;
}

BD_CP_MenuData::~BD_CP_MenuData()
{
}

void BD_CP_MenuData::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();

#if defined(SMARTPHONE) || defined(BLUETOOTH_DONGLE)
	m_Data.Write( *this );
#endif
}

void BD_CP_MenuData::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size, data);
	
#if defined(SMARTPHONE) || defined(BLUETOOTH_DONGLE)
	m_Data.Read( *this );
#endif
}

bool BD_CP_MenuData::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef SMARTPHONE
	OrbiterApp::GetInstance()->SetMenuData( m_Data );
#endif //SMARTPHONE

	return true;
}
