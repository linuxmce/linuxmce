/*
 PlutoVMCView
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>  // for Fonts. 

#include <aknviewappui.h>
#include <avkon.hrh>

#include "PlutoMO.hrh"
#include "PlutoVMCView.h"
#include "PlutoVMCUtil.h"
#include "PlutoVMCContainer.h" 
#include "PlutoMOAppUi.h"
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPlutoVMCView::CPlutoVMCView()
{
	bReadyToRedraw = false;

	//the one and only ...
	CPlutoVMCUtil *pVCMUtil = new CPlutoVMCUtil(KCPlutoVMCUtilId, CCoeStatic::EApp);
}

// EPOC default constructor can leave.
void CPlutoVMCView::ConstructL()
    {
    BaseConstructL();
    }

// Destructor
CPlutoVMCView::~CPlutoVMCView()
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	delete pVMCUtil;
	pVMCUtil = NULL;

	
    if (iContainer)
	{
		AppUi()->RemoveFromStack(iContainer);
	    delete iContainer;
		iContainer = NULL;
    }
	
}

void CPlutoVMCView::Refresh() 
{ 
	if(bReadyToRedraw)
		iContainer->DrawDeferred(); 
}

// ---------------------------------------------------------
// TUid CAknExEditorView1::Id()
// Returns Id of the view.
// ---------------------------------------------------------
TUid CPlutoVMCView::Id() const
    {
    return KPlutoVMCViewId;
    }

// ---------------------------------------------------------
// CPlutoVMCView::HandleCommandL(TInt aCommand)
// Handles commands
// ---------------------------------------------------------
void CPlutoVMCView::HandleCommandL(TInt aCommand)
    {   
    switch (aCommand)
        {
		
        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// ---------------------------------------------------------
// CPlutoVMCView::HandleClientRectChange()
// Handles client rect change.
// ---------------------------------------------------------
void CPlutoVMCView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

// ---------------------------------------------------------
// CPlutoVMCView::DoActivateL(...)
// Creates the Container class object.
// ---------------------------------------------------------
void CPlutoVMCView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
                                    TUid /*aCustomMessageId*/,
                                    const TDesC8& /*aCustomMessage*/)
{
    iContainer = new (ELeave) CPlutoVMCContainer;
    iContainer->SetMopParent(this);
    iContainer->ConstructL(ClientRect());
    AppUi()->AddToStackL(*this, iContainer);

	TRect xrect = ClientRect();
	TRect rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iContainer->SetRect(rect);

	bReadyToRedraw = true;

	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVMCUtil->Setup(iContainer, this);
}


// ---------------------------------------------------------
// CPlutoVMCView::DoDeactivate()
// Deletes the Container class object.
// ---------------------------------------------------------
void CPlutoVMCView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        }
    
    delete iContainer;
    iContainer = NULL;
    }

// End of File


