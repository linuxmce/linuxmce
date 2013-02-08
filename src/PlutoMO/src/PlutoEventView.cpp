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
/* Copyright (c) 2003, Nokia. All rights reserved */

#include <aknviewappui.h>
#include <aknconsts.h>
//#include <MultiViews.rsg>

#include "PlutoMOAppUi.h"
#include "PlutoEventView.h"
#include "PlutoEventContainer.h"
//#include "MultiViews.hrh"

CPlutoEventView::CPlutoEventView()
    {
    // No implementation required
    }

CPlutoEventView::~CPlutoEventView()
    {
    // No implementation required
    }

void CPlutoEventView::ConstructL()
    {
#ifdef __SERIES60_30__
    // BaseConstructL(EAknEnableSkin);
    BaseConstructL();
#else
    BaseConstructL();
#endif
    }


TUid CPlutoEventView::Id() const
    {
    return KPlutoEventViewId;
    }

void CPlutoEventView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
                                    TUid /*aCustomMessageId*/,
                                    const TDesC8& /*aCustomMessage*/)
    {
    ASSERT(!(iContainer));
    iContainer = CPlutoEventContainer::NewL(ClientRect());

	TRect xrect = ClientRect();
	TRect rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iContainer->SetRect(rect);
    }

void CPlutoEventView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    }

void CPlutoEventView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
		
        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }
