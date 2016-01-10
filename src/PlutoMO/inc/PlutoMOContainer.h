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
#ifndef PLUTOMOCONTAINER_H
#define PLUTOMOCONTAINER_H

#include <coecntrl.h>
#include <aknlists.h>
#include "eiklbx.h"         // For Listbox
#include "PlutoMOengine.h"




class CPlutoMOContainer : public CCoeControl, MCoeControlObserver
    {
    public: 
        void ConstructL(const TRect& aRect);

        ~CPlutoMOContainer();

    public: 
        void SetPlutoMO(TInt aDirectory, TInt aSizeDate);
		
		// Handles open from option menu
		void CallLaunch();		

		// Handles file delete
		void CallDelete();

		// Find number of files
		void FindNumberFiles();
    
		// Make grid
		void MakeGridL();

	public: 

    TKeyResponse    OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);



    private: // Functions from base classes
        void SizeChanged();

        TInt CountComponentControls() const;

        CCoeControl* ComponentControl(TInt aIndex) const;

        void Draw(const TRect& aRect) const;

        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    public:
        CPlutoMOEngine* iAppEngine;
	
    private: //data
        // Listbox
        CAknDoubleNumberStyleListBox* iListBox;
		//CPlutoMOGridEngine* iGamesGrid; // the grid

    };

#endif

// End of File
