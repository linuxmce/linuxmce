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

#ifndef __PLUTOEVENT_VIEW_H__
#define __PLUTOEVENT_VIEW_H__


#include <aknview.h>

const TUid KPlutoEventViewId = {43};

class CPlutoEventContainer;

/*! 
  @class CPlutoEventView
  
  @discussion An instance of the Application View object for the MultiViews 
  example application
*/
class CPlutoEventView: public CAknView
    {
public:

/*!
  @function ~CPlutoEventView
  
  @discussion Destroy the object 
*/
    ~CPlutoEventView();

/*!
  @function Id

  @discussion Identity of this view to the system
  @result the ID of view
*/
    TUid Id() const;

/*!
  @function HandleCommandL
  
  @discussion Handle commands
  @param aCommand Command to be handled
*/
    void HandleCommandL(TInt aCommand);

/*!
  @function DoActivateL
  
  @discussion Create the Container class object
  @param aPrevViewId The id of the previous view
  @param aCustomMessageId message identifier
  @param aCustomMessage custom message provided when the view is changed
*/
  void DoActivateL(const TVwsViewId& aPrevViewId,
                   TUid aCustomMessageId,
                   const TDesC8& aCustomMessage);

/*!
  @function DoDeactivate
  
  @discussion Remove the container class instance from the App UI's stack and
  deletes the instance
  */
  void DoDeactivate();

/*!
  @function CPlutoEventView
  
  @discussion Perform the first phase of two phase construction 
*/
    CPlutoEventView();

/*!
  @function ConstructL
  
  @discussion  Perform the second phase construction of a CPlutoEventView object
*/
    void ConstructL();

	/*! @var iContainer container for this view */
	CPlutoEventContainer* iContainer;

private:

    /*! @var iIdentifier identifier for this view */
    TUid iIdentifier;
    };


#endif // __PLUTOEVENT_VIEW_H__
