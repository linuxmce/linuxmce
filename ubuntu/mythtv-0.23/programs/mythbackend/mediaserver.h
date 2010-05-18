//////////////////////////////////////////////////////////////////////////////
// Program Name: mediaserver.h
//                                                                            
// Purpose - uPnp Media Server main Class
//                                                                            
// Created By  : David Blain                    Created On : Jan. 15, 2007
// Modified By :                                Modified On:                  
//                                                                            
//////////////////////////////////////////////////////////////////////////////

#ifndef __MEDIASERVER_H__
#define __MEDIASERVER_H__

#include <QString>

#include "upnp.h"
#include "upnpcds.h"
#include "upnpcmgr.h"
#include "upnpmsrr.h"
#include "upnpmedia.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class MediaServer : public UPnp
{

    protected:
        
        UPnpCDS         *m_pUPnpCDS;      // Do not delete (auto deleted)
        UPnpCMGR        *m_pUPnpCMGR;     // Do not delete (auto deleted)
        UPnpMedia	*upnpMedia;

        QString          m_sSharePath;

    public:
                 MediaServer( bool bMaster, bool bDisableUPnp = FALSE );

        virtual ~MediaServer();

        void RebuildMediaMap(void) { upnpMedia->BuildMediaMap(); };

        void     RegisterExtension  ( UPnpCDSExtension    *pExtension );
        void     UnregisterExtension( UPnpCDSExtension    *pExtension );

};

#endif
