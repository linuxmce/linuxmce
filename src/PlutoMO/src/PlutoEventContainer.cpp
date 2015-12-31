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

#include "PlutoEventContainer.h"
#include "ImageLoader.h"
#include "MyString.h"
#include "PlutoMOAppUi.h"
#include <coecobs.h>

CPlutoEventContainer* CPlutoEventContainer::NewL(const TRect& aRect)
    {
    CPlutoEventContainer* self = CPlutoEventContainer::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

CPlutoEventContainer* CPlutoEventContainer::NewLC(const TRect& aRect)
    {
    CPlutoEventContainer* self = new (ELeave) CPlutoEventContainer;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

void CPlutoEventContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    }

TInt CPlutoEventContainer::CountComponentControls() const
    {
    return 0;
    }

void CPlutoEventContainer::Draw(const TRect& aRect) const
{    
	CPlutoMOAppUi* pApp = (CPlutoMOAppUi*)(CCoeEnv::Static()->AppUi());
	CWindowGc& gc = SystemGc();

	TRect rect(0, 0, -1, -1);
	CFbsBitmap *pBitmap = CImageLoader::LoadBitmap(pApp->m_sPlutoEventPng.Des());

	gc.BitBlt(rect.iTl, pBitmap);

	CGraphicsDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();
	CFont *pCurrentFont;
	
	TFontSpec myFontSpec(string("Arial").Des(), 10); 
	pCurrentFont = CCoeEnv::Static()->CreateScreenFontL(myFontSpec); 

	TRgb color(0, 0, 0);
	gc.SetPenColor(color);
	gc.UseFont(pCurrentFont);

	TPoint point(30, 40);
    gc.DrawText(pApp->iPlutoEventTypes[pApp->iPhoneTypes[pApp->iCurType].iWAP_EventType - 1], point);

	TPoint point2(30, 60);
    gc.DrawText(pApp->iPhoneTypes[pApp->iCurType].iMessage, point2);

	gc.DiscardFont();
	CCoeEnv::Static()->ReleaseScreenFont(pCurrentFont);

	pApp->m_bPlutoEventVisible = true;
}

CCoeControl* CPlutoEventContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }

TKeyResponse CPlutoEventContainer::OfferKeyEvent(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	CPlutoMOAppUi* pApp = (CPlutoMOAppUi*)(CCoeEnv::Static()->AppUi());
	
	if(aType == EEventKeyDown)
	{
		switch(aKeyEvent.iScanCode)		
		{
			case 0x31: //reset the alarm 
				break;
			case 0x32: //ignore the alarm 
				break;
			case 0x33: //fire an SOS event
				break;
			case 0x34: //notify the neibors
				break;
			case 0x35: //speak to the person in the house
				break;
			case 0x36: //go to the regular vmc remote control
				TFileName file_name;
				file_name.Append(pApp->m_sPlutoVMC.Des());
				pApp->OpenVMC(false, file_name, NULL);
				pApp->m_bPlutoEventVisible = false;
				MakeVisible(false);
				break;
			/*
			case 0x31:	 //go to pluto control panel (the browser)
			{
				pApp->LaunchBrowser();
				pApp->m_bPlutoEventVisible = false;
				MakeVisible(false);
				break;
			}
			
			case 0x32:  //view info about event
			{
				TFileName file_name;
				file_name.Append(pApp->m_sPlutoVMC.Des());
				pApp->OpenVMC(false, file_name, NULL);
				pApp->m_bPlutoEventVisible = false;
				MakeVisible(false);
				break;
			}

			case 0x33:  //ignore
			{
				MakeVisible(false);
				pApp->m_bPlutoEventVisible = false;
				MakeVisible(false);
				break;
			} 
			*/
		}
	}

	return EKeyWasNotConsumed;
}

