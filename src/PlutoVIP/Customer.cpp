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
#include "stdafx.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD/PhoneDevice.h"
#include "Customer.h"
#include "Picture.h"
#include "ImageDefs.h"

#ifdef BT_SOCKET_X
#include "BD/BDCommandProcessor_Windows_Socket.h"
#else
#include "BD/BDCommandProcessor_Windows_Bluetooth.h"
#endif

BDCommandProcessor *Customer::GetCommandProcessor()
{
	if( m_pBDCommandProcessor )
		return m_pBDCommandProcessor;

	if( !m_pPhoneDevice )
		throw "Customer with no phone device";

#ifdef BT_SOCKET_X
	EstablishmentSocket *pSocket = new EstablishmentSocket(1,"localhost:3461","foo");
	if( !pSocket->Connect() )
	{
		MessageBox(NULL,"Could not connect to phone emulator","*ERROR*",MB_OK);
		return;
	}
	m_pBDCommandProcessor = new BDCommandProcessor_Windows_Socket("Mac",pSocket);
#else
	m_pBDCommandProcessor = new BDCommandProcessor_Windows_Bluetooth(m_pPhoneDevice->m_sMacAddress,"");
#endif

	return m_pBDCommandProcessor;
}
