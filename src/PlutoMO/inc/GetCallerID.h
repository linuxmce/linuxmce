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
#ifndef __GetCallerID_H
#define __GetCallerID_H

#include <e32base.h>
#include <logcli.h>
#include <logview.h>
#include <f32file.h>
#include <logwrap.h>
#include <e32svr.h>
#include <aknnotewrappers.h>

class CPlutoMOAppUi;

class CGetCallerId : public CActive
{
public:
	CLogViewRecent* iRecentLogView;
	RFs iFs;
	CLogClient* iLogClient;
	CPlutoMOAppUi* iAppView;

	CGetCallerId(CPlutoMOAppUi* aAppView);
	~CGetCallerId();
	virtual void RunL();
	virtual void DoCancel();
	void GetLatest(); 
};

#endif