/*
 PlutoMODocument
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "PlutoMODocument.h"
#include "PlutoMOAppUi.h"

CPlutoMODocument::CPlutoMODocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

CPlutoMODocument::~CPlutoMODocument()
    {
    }

void CPlutoMODocument::ConstructL()
    {
    }

CPlutoMODocument* CPlutoMODocument::NewL(
        CEikApplication& aApp)    
    {
    CPlutoMODocument* self = new (ELeave) CPlutoMODocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


    
CEikAppUi* CPlutoMODocument::CreateAppUiL()
    {
		return new (ELeave) CPlutoMOAppUi;
    }

// End of File  
