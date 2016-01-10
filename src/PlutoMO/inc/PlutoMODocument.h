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
#ifndef PLUTOMODOCUMENT_H
#define PLUTOMODOCUMENT_H

#include <akndoc.h>

   
class  CEikAppUi;

class CPlutoMODocument : public CAknDocument
    {
    public: 
        static CPlutoMODocument* NewL(CEikApplication& aApp);

        virtual ~CPlutoMODocument();

    private:

        CPlutoMODocument(CEikApplication& aApp);
        void ConstructL();

    private: 
        CEikAppUi* CreateAppUiL();

    };

#endif

// End of File

