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
#ifndef PLUTOMOAPP_H
#define PLUTOMOAPP_H

#include <aknapp.h>
#include <documenthandler.h>

const TUid KUidPlutoMO = { 0xA1ef0015 };
class CPlutoMOApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        CApaDocument* CreateDocumentL();
        
        TUid AppDllUid() const;
        CDocumentHandler* iDocHandler;
    };

#endif

// End of File

