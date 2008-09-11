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
/**
 *
 * @file MediaHandlerInfo.h
 * @brief documentation
 * The set of classes that support a Media Plugin object
 *
 */

#ifndef MediaHandlerInfo_h
#define MediaHandlerInfo_h

#include "DCE/Logger.h"
#include "DeviceData_Router.h"
#include <deque>

class MediaFile;
void operator+= (deque<MediaFile *> &dTarget, deque<MediaFile *> &dAdditional);

namespace DCE
{
    enum SourceType {
        st_RemovableMedia,  /** The media is coming from a removable disc */
        st_Storage,         /** The media is stored on the core */
        st_Broadcast,       /** Transmission from one DCE to another */
        st_Transmission     /** The media is coming from an external transmission, like Radio or Internet Radio */
    };

#define MEDIASTREAM_TYPE_GENERIC        0
#define MEDIASTREAM_TYPE_CD             1
#define MEDIASTREAM_TYPE_DVD            2

    // Specific to plugins
    // ? Shouldn't this be managed using plugin database ID's or something ?
#define MEDIASTREAM_TYPE_MYTHTV         100
#define MEDIASTREAM_TYPE_XINE           101
#define MEDIASTREAM_TYPE_GAME		300	

    /** @brief
     *
     * A Media Plugin is derived from the MediaHandlerBase abstract class.  When it registers, it passes in a MediaHandlerInfo pointer
     * indicating what type of media it can play.  It may register several times with different types of media and different capabilities.
     * So there exists 1 MediaHandlerInfo for each combination of MediaPlugin and MediaType (ie: xine/audio, xine/video, myth/tv, etc.)
     */
    class MediaHandlerInfo
    {
    public:

		int m_PK_MediaType;

        bool    m_bUsesRemovableMedia,      /** True means it can play from a removable drive.
                                                For example with MediaType DVD, true means it can play a DVD disc. */

        m_bCanStoreOnServer,        /** True means it can play stored media from the server */

        m_bCanBroadcastInHouse,     /** Means it can be broadcast within the house, using one of the protocols
                                        in the send/receive lists below  */

        m_bIsExternalTransmission,      /** True means it is an external transmissions, like Radio.
                                            Not related to internal broadcasting between devices */

        m_bCanJumpPosition;         /** True means the device can save its position, and return to it, or have another device
                                    continue the stream where this left off  */

        int m_iPK_DesignObj;            /** What remote control to use for this type of media */

        list<class MediaDevice *> m_listMediaDevice;    /** Which Media Devices it is representing.  This can be empty.
                                            Then the media handler will give preference to a plug-in that specifically represents a device */

        list<class EntertainArea *> m_listEntertainArea;    /** Which entertainment areas it can play this media on.
                                                                Normally this would be all the entertainment areas corresponding to m_listMediaDevice.
                                                                But in whole house audio it can be more complex. */

        list<string> m_listExtensions;  /** What file extensions it can play for this type of media (ie mp3, wav, vob, etc.) */

        list<int> m_PK_Broadcast_Send;  /** What broadcast formats it supports to send */

        list<int> m_PK_Broadcast_Receive;   /** What broadcast formats it supports to receive */

        bool m_bUsesDevices;            /** If the devices it represents are DCE, or 'dumb' devices, like infrared a/v gear */

        class MediaHandlerBase *m_pMediaHandlerBase;  /** The actual handler */

        class Command_Impl *m_pCommand_Impl;  /** The same handler, but we need the v-table for the Command_Impl so we can pass messages
                                                  into it's received dcemessage */

        /** @brief A constructor for a plug-in that wants to specify all this stuff manually */

    MediaHandlerInfo(class MediaHandlerBase *pMediaHandlerBase,class Command_Impl *pCommand_Impl)
    { m_pMediaHandlerBase=pMediaHandlerBase; m_pCommand_Impl=pCommand_Impl; m_iPK_DesignObj=0; };

        /** @brief
     * This will perform some standard tasks commonly needed to setup a plug-in.  First, if PK_DeviceTemplate is specified,
         * it will add all devices of that type to this plug-in.  If the media type has associated extensions, and none were already added,
     * it will add the ones from the database.
         * It assigns all the default values for bUsesRemovableMedia, m_bCanStoreOnServer, m_bCanBroadcastInHouse, m_bIsExternalTransmission.
         * If the plug-in isn't associated with any entertainment areas, it will add itself to all entertainment areas where it's devices
     * are located.  Then it registers
         * The calling plug-in can change values after the fact if it deviates from the defaults (ie doesn't support something, or uses different
     * file extensions)
         * If the plug-in wants to change the the entertainment areas, it should set bDontRegister to true, change these values, then register.
     * At registration the media handler associates all the plug-ins with their media types.  Broadcast formats can be set after registration.
    */

        MediaHandlerInfo(class MediaHandlerBase *pMediaHandlerBase,class Command_Impl *pCommand_Impl,int PK_MediaType,int PK_DeviceTemplate,bool bCanJumpPosition,bool bUsesDevices,bool bDontRegister=false);
    };

    typedef list<MediaHandlerInfo *> List_MediaHandlerInfo;


    class BoundRemote
    {
    public:
        class OH_Orbiter *m_pOH_Orbiter;
        class EntertainArea *m_pEntertainArea;
        class Media_Plugin *m_pMedia_Plugin;
        string m_sPK_DesignObj_Remote,m_sPK_DesignObj_GraphicImage,m_sOptions;
        int m_iPK_Text_Description,m_iPK_Text_Section,m_iPK_Text_Synopsis,m_iPK_Text_Timecode;

        BoundRemote(class Media_Plugin *pMedia_Plugin) { m_iPK_Text_Description=m_iPK_Text_Timecode=0; m_pMedia_Plugin=pMedia_Plugin;}
        void UpdateOrbiter(MediaStream *pMediaStream);
    };

    typedef map<int,BoundRemote *> MapBoundRemote;
}

#endif
