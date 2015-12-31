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
#ifndef PLUTO_VMC_VIEW_H
#define PLUTO_VMC_VIEW_H

// INCLUDES
#include <aknview.h>
#include "PlutoVMCContainer.h"


// CONSTANTS
// UID of view
const TUid KPlutoVMCViewId = {1};


// CLASS DECLARATION

/**
*  CPlutoVMCView view class.
* 
*/
class CPlutoVMCView : public CAknView
    {

    public: // Constructors and destructor
        /**
        * C++ default constructor
        */
		CPlutoVMCView();
	
        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CPlutoVMCView();


		void Refresh();

      CPlutoVMCContainer* iContainer;

    public: // Functions from base classes
        
        /**
        * From CAknView, Returns the ID of view.
        * @return Returns the ID of view.
        */
        TUid Id() const;

        /**
        * From CAknView, Handles the commands.
        * @pram aCommand Command to be handled.
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CAknView, Handles the clientrect.
        */
        void HandleClientRectChange();

   private:

        /**
        * From CAknView, Creates the Container class object.
        * @param aPrevViewId This is not used now.
        * @param aCustomMessage This is not used now.
        * @param aCustomMessage This is not used now.
        */
        void DoActivateL(const TVwsViewId& /*aPrevViewId*/,
                         TUid /*aCustomMessageId*/,
                         const TDesC8& /*aCustomMessage*/);

        /**
        * From AknView, Deletes the Container class object.
        */
        void DoDeactivate();

    private: // Data
  
		bool bReadyToRedraw;

    };

#endif  // PLUTO_VMC_VIEW_H

// End of File
