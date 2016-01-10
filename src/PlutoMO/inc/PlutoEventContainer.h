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

#ifndef __PLUTOEVENT_CONTAINER_H__
#define __PLUTOEVENT_CONTAINER_H__

#include <aknview.h>
#include <coecobs.h>

/*! 
  @class CPlutoEventContainer
  
  @discussion An instance of CPlutoEventContainer contains the view drawn to the screen
  for CMultiViewsView1
*/
class CPlutoEventContainer : public CCoeControl
    {
public: 

/*!
  @function NewL
   
  @discussion Create a CPlutoEventContainer object, which will draw itself to aRect
  @param aRect the rectangle this view will be drawn to
  @result a pointer to the created instance of CPlutoEventContainer
*/
    static CPlutoEventContainer* NewL(const TRect& aRect);

/*!
  @function NewLC
   
  @discussion Create a CPlutoEventContainer object, which will draw itself to aRect
  @param aRect the rectangle this view will be drawn to
  @result a pointer to the created instance of CPlutoEventContainer
*/
    static CPlutoEventContainer* NewLC(const TRect& aRect);

/*!
  @function ConstructL
  
  @discussion Perform the second phase construction of a CPlutoEventContainer object
  @param aRect Frame rectangle for container.
*/
    void ConstructL(const TRect& aRect);

public: // from CoeControl
/*!
  @function CountComponentControls
  
  @discussion Get the number of controls contained in this compound control
  @return Number of component controls 
  @result the number of controls
*/
    TInt CountComponentControls() const;

/*!
  @function ComponentControl.

  @discussion Get the specified component
  @param aIndex specification for component pointer
  @return Pointer to component control
*/
    CCoeControl* ComponentControl(TInt aIndex) const;

/*!
  @function Draw
  
  @discussion Draw this CPlutoEventContainer to the screen
  @param aRect the rectangle of this view that needs updating
*/
void Draw(const TRect& aRect) const;

/**
 * From CCoeControl, Handles key event.
 * @param aKeyEvent The key event.
 * @param aType The type of the event.
 * @return Indicates whether or not the key event was
 *           used by this control.
 */
TKeyResponse OfferKeyEvent(const TKeyEvent& aKeyEvent,
                            TEventCode aType);

};

#endif // __PLUTOEVENT_CONTAINER_H__
