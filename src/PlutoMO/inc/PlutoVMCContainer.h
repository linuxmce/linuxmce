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
#ifndef PLUTO_VMC_CONTAINER_H
#define PLUTO_VMC_CONTAINER_H

// INCLUDES
#include <aknview.h>

#include "PlutoUtils/MyString.h"

#include "VIPShared/RenderMenu.h"
//#include "VIPShared/PlutoConfig.h"
#include "PlutoVMCUtil.h"
//------------------------------------------------------------------------------------------------------------------
// CLASS DECLARATION

/**
*  CPlutoVMCContainer  container control class.
*  
*/
class CPlutoVMCContainer : public CCoeControl, public MCoeControlObserver
    {

    public: // Constructors and destructor
        /**
        * C++ default constructor.
        */
        CPlutoVMCContainer();

        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        virtual ~CPlutoVMCContainer();

		CWindowGc& GetSystemGc() { return SystemGc(); }

		bool m_bScrollUp;

        /**
         * From CCoeControl, Handles key event.
         * @param aKeyEvent The key event.
         * @param aType The type of the event.
         * @return Indicates whether or not the key event was
         *           used by this control.
         */
         TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);

         TKeyResponse OfferKeyEvent(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);

		 void SimulateEvent(long eventType, long key);

		 bool m_bDataKeys;

    public: // New functions
        /**
        * Change focussed control using specified command.
        * @param aCommand Command to be used.
        */
        void FocusTo(TInt aCommand);

    private: // Functions from base classes

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        * @param aIndex of control
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

       /**
        * From CCoeControl, Draw client rect.
        * @param aRect Frame rectangle for container.
        */
        void Draw(const TRect& aRect) const;

		 int NumberKeyIndex(int KeyCode);
		 char GetKeyChar(int KeyCode);
		 bool IsNumberKey(int KeyCode);
		 bool IsNumberKey(const TKeyEvent& aKeyEvent);
		 int GetKeyCode(const TKeyEvent& aKeyEvent, TEventCode aType);
		 void AutomaticallyScrollDataGrid(bool bUp);

		 bool HandleDataGridKeys(const TKeyEvent& aKeyEvent, TEventCode aType);		 
		 bool HandleAutomaticDataGridScrolling(const TKeyEvent& aKeyEvent, TEventCode aType);
		 bool HandleCommonKeys(const TKeyEvent& aKeyEvent, TEventCode aType);
		 bool HandleCaptureKeyboardKeys(const TKeyEvent& aKeyEvent, TEventCode aType);

		 int GetSymbianKeyEventFromKeyCode(TKeyEvent& aKeyEvent, TEventCode& aType, long key);

    private:
       /**
        * From MCoeControlObserver, Handles an event of type aEventType
        * @param aControl Pointer to component control
        * @param aEventType Event Code
        */
        void HandleControlEventL(CCoeControl* /*aControl*/,
                                    TCoeEvent /*aEventType*/);


		bool m_bRepeated;

		CPeriodic *iScrollTimer;
		CPeriodic *iKeyTimer;

		static TInt ScrollTimerCallBack(TAny* aKeyEvent);
		static TInt KeyTimerCallBack(TAny* aKeyEvent);

		bool m_bNeedRefresh;
		bool m_bDeleteLastKey;

		int m_iRepeatStep;
		int m_uLastKeyCode;
		int m_iLastTick;
    };

#endif // PLUTO_VMC_CONTAINER_H

// End of File
