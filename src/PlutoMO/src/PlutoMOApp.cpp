/*
 PlutoMOApp
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifdef __SERIES60_30__
#include <eikstart.h>
#endif

#include    "PlutoMOApp.h"
#include    "PlutoMODocument.h"

TUid CPlutoMOApp::AppDllUid() const
    {
    return KUidPlutoMO;
    }

CApaDocument* CPlutoMOApp::CreateDocumentL()
    {
    return CPlutoMODocument::NewL( *this );
    }

EXPORT_C CApaApplication* NewApplication()
    {
    return new CPlutoMOApp;
    }
#ifdef __SERIES60_30__
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
# else
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }
#endif

// End of File  

