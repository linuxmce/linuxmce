/*
 GetCallerID
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "GetCallerID.h"
#include "PlutoMOAppUi.h"
#include <aknnotewrappers.h>


CGetCallerId::CGetCallerId(CPlutoMOAppUi* aAppView): CActive(EPriorityStandard)
{
	iAppView=aAppView;
	CActiveScheduler::Add(this); 
}

CGetCallerId::~CGetCallerId()
{
	CActive::Cancel(); // Causes call to DoCancel()
}

void CGetCallerId::RunL()
{
	if( iStatus == KErrNone)
	{
		iAppView->NotifyIncomingNumber(iRecentLogView->Event().Number());
	} 
}

void CGetCallerId::GetLatest()
{
	// Connect to the Log Server
	iFs.Connect();

	iLogClient = CLogClient::NewL(iFs);
	iRecentLogView = CLogViewRecent::NewL(*iLogClient);

	// Cancel any outstanding requests.
	iRecentLogView->Cancel();

	// Make a call into the Log server to get the latest call event.
	if( iRecentLogView->SetRecentListL(KLogNullRecentList,iStatus))
	{
		// Notify the Log server that we're ready
		SetActive();
	} 
}

void CGetCallerId::DoCancel()
{
	CActive::Cancel();
	// Cancel any outstanding requests.
	iRecentLogView->Cancel();
}

