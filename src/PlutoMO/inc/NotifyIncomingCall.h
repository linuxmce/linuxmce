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
#ifndef __NotifyIncomingCall_H
#define __NotifyIncomingCall_H

#include <etel.h>
#include <e32base.h>
#include <logcli.h>
#include <logview.h>
#include <f32file.h>
#include <logwrap.h>
#include <e32svr.h>
#include <aknnotewrappers.h>

class CPlutoMOAppUi;

class CIncomingCallWatcher : public CActive
{
public:
	~CIncomingCallWatcher();
	CIncomingCallWatcher(RLine& aLine, CPlutoMOAppUi *aAppView);
	void ConstructL();

protected:	// from CActive
	void RunL();
	void DoCancel();

private:	// implementation
	void IssueRequestL();

private:
	RLine&					iLine;
	CPlutoMOAppUi			*iAppView;
	TName					iName;
};

#endif