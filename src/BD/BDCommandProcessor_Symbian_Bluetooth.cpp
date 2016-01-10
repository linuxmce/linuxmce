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
#include "BD/BDCommandProcessor_Symbian_Bluetooth.h"

#include <bt_sock.h>

const TInt KPort = 19; //3460;

void BDCommandProcessor_Symbian_Bluetooth::Start() 
{
	User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iSdpSession.Connect());
    User::LeaveIfError(iSdpDatabase.Open(iSdpSession));

	// Begin listening for incoming pluto connections
	User::LeaveIfError( iListeningSocket.Open( iSocketServ, KServerTransportName ) );
	User::LeaveIfError( iSocket.Open( iSocketServ ) );

	TBTSockAddr listeningAddress;
	listeningAddress.SetPort( KPlutoMOPort );

	User::LeaveIfError( iListeningSocket.Bind( listeningAddress ) );
}

