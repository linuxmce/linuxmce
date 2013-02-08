/*
 PlutoMOContainer
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "PlutoMOAppUi.h"
#include "PlutoMOContainer.h"
#include "PlutoMOengine.h"
#include "PlutoMO.hrh"
#include "Logger.h"

// For Grid

#include <PlutoMO.rsg> // R_PlutoMO_GAMES_GRID, R_ICON_FILE_NAME


//#include "PlutoMOGridEngine.h"//CPlutoMOGridEngine

void CPlutoMOContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
  
    iListBox = new (ELeave) CAknDoubleNumberStyleListBox;

	iListBox->SetContainerWindowL( *this );
	iListBox->ConstructL( this, EAknListBoxMarkableList);


	// Create the scroll indicator
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iListBox->ActivateL();

    // Create the PlutoMOEngine
    iAppEngine = new (ELeave) CPlutoMOEngine;  

    SetPlutoMO(EPlutoMOPictures, EPlutoMODate);
	
	SetRect(aRect);
    ActivateL();
    }

CPlutoMOContainer::~CPlutoMOContainer()
    {
    delete iAppEngine;
    delete iListBox;
    }

void CPlutoMOContainer::SetPlutoMO(TInt aDirectory, TInt aSizeDate)
    {

    CDesCArray* items = static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray());

    if (iAppEngine->RemoveItems(items)) 
        {
        iListBox->HandleItemRemovalL();  
        }

	iAppEngine->SetDirectory(aDirectory);  
	iAppEngine->SetSizeDate(aSizeDate);     
    
	if(iAppEngine->StartPlutoMO() == KErrNone)
        {        
        iAppEngine->GetPlutoMOItems(items);       
        }
    iAppEngine->EndPlutoMO();                  

	iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndex(0);
    iListBox->DrawNow();
    }

void CPlutoMOContainer::SizeChanged()
    {
    iListBox->SetExtent(TPoint(0,0),TSize(176,160));
    }


TKeyResponse CPlutoMOContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	//LOG("key");
	if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bApplicationIsMinimized)
	{
		//LOG("minimized is true");
		if(aType == EEventKeyUp)
		{
			//LOG("key up");
			((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bApplicationIsMinimized = false;
			//LOG("minimized set to false");
		}
	}
	else
	{
		//LOG("minimized is false");

		if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bVMCViewerVisible)
		{
			//LOG("m_bVMCViewerVisible is true");
			return EKeyWasNotConsumed;
		}

		if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bPlutoEventVisible)
		{
			//LOG("m_bPlutoEventVisible is true");
			return EKeyWasNotConsumed;
		}
	}

    TInt code = aKeyEvent.iCode;
    switch(code)
    {
		case EKeyOK:
			iAppEngine->LaunchCurrent(iListBox->CurrentItemIndex());
			return (EKeyWasConsumed);

		default:
		{
			//LOG("event to list box");
			return iListBox->OfferKeyEventL(aKeyEvent, aType);
		}
    }
}

TInt CPlutoMOContainer::CountComponentControls() const
    {
    return 1; 
    }

CCoeControl* CPlutoMOContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

void CPlutoMOContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

void CPlutoMOContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
		int a = 0;
    }

void CPlutoMOContainer::CallLaunch() 
{
	iAppEngine->LaunchCurrent(iListBox->CurrentItemIndex());	
}

void CPlutoMOContainer::CallDelete() 
{
	iAppEngine->RemoveFile(iListBox->CurrentItemIndex());	
}

void CPlutoMOContainer::FindNumberFiles() 
{
	iAppEngine->FindFiles();	
}

void CPlutoMOContainer::MakeGridL()
{
/*
	iGamesGrid = new (ELeave)CPlutoMOGridEngine;

  CreateWindowL();

	
	SetRect(aRect);
	ActivateL();
*/
}

// End of File  
