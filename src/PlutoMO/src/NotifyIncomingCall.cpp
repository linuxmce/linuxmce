/*
 NotifyIncomingCall
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "NotifyIncomingCall.h"
#include "PlutoMOAppUi.h"

CIncomingCallWatcher::~CIncomingCallWatcher()
{
	Cancel();
}

CIncomingCallWatcher::CIncomingCallWatcher(RLine& aLine, CPlutoMOAppUi *aAppView) 
	: CActive(EPriorityStandard), iLine(aLine), iAppView(aAppView)
{
	CActiveScheduler::Add(this);
}

void CIncomingCallWatcher::ConstructL()
{
	IssueRequestL();
}

void CIncomingCallWatcher::IssueRequestL()
{
	iLine.NotifyIncomingCall(iStatus, iName);
	SetActive();
}

void CIncomingCallWatcher::RunL()
{
	// if successful, notify observer and re-issue request
	if (iStatus.Int() == KErrNone)
		iAppView->NotifyIncomingCall(iName);
	IssueRequestL();
}

void CIncomingCallWatcher::DoCancel()
{
	iLine.NotifyIncomingCallCancel();
}

