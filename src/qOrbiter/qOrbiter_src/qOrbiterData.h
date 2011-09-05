#ifndef qOrbiterData_H
#define qOrbiterData_H

#include "Message.h"

#include <deque>

#define ORBITER_SCHEMA		32	// Used to determine if cached screens can be used or if the schema has changed

#ifdef ORBITER
#include "DesignObj_Orbiter.h"
#include "DesignObj_DataGrid.h"
#endif

// This is all the data for a Orbiter that we will serialize.  Be sure "ORBITER" is defined when including this in the Orbiter projects.
// Note that the generator also uses this class to serialize the object data, but in that case, it's serializing m_ScreenMap as a list of
// DesignObj_Data pointers.  The Orbiter adds more functionality to DesignObj_Data and needs DesignObj_Orbiter.  The serialization still
// works fine because DesignObj_Orbiter doesn't add any extra serialization.

#define SERIALIZE_DATA_TYPE_SCREEN_MAP		2000
#define SERIALIZE_DATA_TYPE_STYLE_MAP		2001
#define SERIALIZE_DATA_TYPE_LOCATIONS		2002

typedef map<int, string> VariableMap;



class qLocationInfo : public SerializeClass
{
public:
        int PK_Room,PK_EntertainArea,iLocation;
        // Some virtual devices within the media director
        int m_dwPK_Device_MediaDirector,m_dwPK_Device_DiscDrive,m_dwPK_Device_AppServer,m_dwPK_Device_LCD_VFD,
                m_dwPK_Device_IRReceiver,m_dwPK_Device_MediaBurner,m_dwPK_Device_VideoConf,m_dwPK_Device_CamcorderCapt,
                m_dwPK_Device_Orbiter,m_dwPK_Device_WebBrowser;
        vector<int> m_vectAllowedUsers;
        string Description;
        class Row_Icon *drIcon; // Not serialized, just a temporary pointer for the generator
        vector<class PlutoGraphic*> *m_pvectGraphic; // Not serialized, just a temporary pointer for the orbiter with the button for this location

        qLocationInfo() { m_dwPK_Device_MediaDirector = m_dwPK_Device_DiscDrive = m_dwPK_Device_AppServer = m_dwPK_Device_LCD_VFD = m_dwPK_Device_IRReceiver = m_dwPK_Device_MediaBurner = m_dwPK_Device_VideoConf = m_dwPK_Device_CamcorderCapt = m_dwPK_Device_Orbiter = m_dwPK_Device_WebBrowser = DEVICEID_NULL; m_pvectGraphic=NULL; }
        void SetupSerialization(int iSC_Version)
        {
                StartSerializeList() + PK_Room + PK_EntertainArea + iLocation + Description +
                        m_dwPK_Device_MediaDirector + m_dwPK_Device_DiscDrive + m_dwPK_Device_AppServer + m_dwPK_Device_LCD_VFD + m_dwPK_Device_IRReceiver + m_dwPK_Device_MediaBurner + m_dwPK_Device_VideoConf +
                        m_dwPK_Device_CamcorderCapt + m_dwPK_Device_Orbiter + m_dwPK_Device_WebBrowser + m_vectAllowedUsers;
        }
        virtual string SerializeClassClassName() { return "qLocationInfo"; }
};

typedef deque<qLocationInfo *> DequeqLocationInfo;

class qOrbiterData : public SerializeClass
{
public:
        int m_Width, m_Height, m_AnimationStyle, m_dwPK_Users_Default, m_iLocation_Initial, m_iUiVersion;
        unsigned long m_tGenerationTime;
        string m_sMainMenu,m_sSleepingMenu,m_sScreenSaveMenu,m_sInitialScreen,m_sSkin;

        map<int,string> m_mapTextString;
        vector<int> m_vectPK_Users_RequiringPIN;
        map<int,int> m_mapDesignObj; //Used to map a screen to a DesignObj
        map<int,int> m_mapPK_Screen_GoBackToScreen;  // For screens in this map, if there's a go back
        map<int,int> m_mapPK_MediaType_PK_Attribute_Sort;  // The default sort for each type of media
        map<int,int> m_mapScreen_Interrupt; // Map of which scripts can be interrupted
        map<int,string> m_mapPK_MediaType_Description; // The description for all the media
        map<int,string> m_mapPK_AttributeType_Description; // The description for all attribute types
        bool m_bIsOSD,m_bNewOrbiter,m_bUseAlphaBlending,m_bUseMask;
        int m_rSpacing;  // Offsets for the screen image

        // Some global devices
        int m_dwPK_Device_Router,m_dwPK_Device_DatagridPlugIn,m_dwPK_Device_InfraredPlugIn,m_dwPK_Device_GeneralInfoPlugIn,m_dwPK_Device_EventPlugIn,m_dwPK_Device_OrbiterPlugIn,
                m_dwPK_Device_LightingPlugIn, m_dwPK_Device_ClimatePlugIn, m_dwPK_Device_MediaPlugIn, m_dwPK_Device_TelecomPlugIn, m_dwPK_Device_SecurityPlugIn, m_dwPK_Device_PlugAndPlayPlugIn,
                m_dwPK_Device_LocalAppServer, m_dwPK_Device_LocalMediaPlayer, m_iRotation,
                m_dwPK_Device_LocalOsdIRReceiver,m_dwPK_Device_LocalOsdVfdLcd;  // These 2 only used if it's an OSD

        qOrbiterData() { m_dwPK_Device_Router = m_dwPK_Device_DatagridPlugIn = m_dwPK_Device_InfraredPlugIn = m_dwPK_Device_GeneralInfoPlugIn = m_dwPK_Device_EventPlugIn = m_dwPK_Device_OrbiterPlugIn =
                m_dwPK_Device_LightingPlugIn = m_dwPK_Device_ClimatePlugIn = m_dwPK_Device_MediaPlugIn = m_dwPK_Device_TelecomPlugIn = m_dwPK_Device_SecurityPlugIn = m_dwPK_Device_PlugAndPlayPlugIn = DEVICEID_NULL; m_iRotation=0; }

        virtual ~qOrbiterData()
        {

                for(size_t iIndex = 0; iIndex < m_dequeLocation.size(); iIndex++)
                        delete m_dequeLocation[iIndex];
                m_dequeLocation.clear();
        }



        deque<class qLocationInfo *> m_dequeLocation;
        VariableMap m_mapVariable;  // our current variables

        qOrbiterData &operator+ (DequeqLocationInfo &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_LOCATIONS,(void *) &i)); return (*this); }

        virtual void ShowProgress(int nPercent) {}
        void SetupSerialization(int iSC_Version)
        {
                StartSerializeList() + m_mapVariable + m_Width + m_Height + m_rSpacing + m_AnimationStyle + m_sInitialScreen + m_sMainMenu + m_sSleepingMenu + m_sScreenSaveMenu + m_dwPK_Users_Default + m_iLocation_Initial + m_iUiVersion + m_sSkin
                        + m_dwPK_Device_Router + m_dwPK_Device_DatagridPlugIn + m_dwPK_Device_EventPlugIn + m_dwPK_Device_InfraredPlugIn + m_dwPK_Device_GeneralInfoPlugIn + m_dwPK_Device_OrbiterPlugIn
                        + m_dwPK_Device_LightingPlugIn + m_dwPK_Device_ClimatePlugIn + m_dwPK_Device_MediaPlugIn + m_dwPK_Device_TelecomPlugIn + m_dwPK_Device_SecurityPlugIn + m_dwPK_Device_PlugAndPlayPlugIn
                        + m_dwPK_Device_LocalAppServer + m_dwPK_Device_LocalMediaPlayer + m_tGenerationTime + m_mapTextString + m_vectPK_Users_RequiringPIN + m_iRotation
                        + m_dwPK_Device_LocalOsdIRReceiver + m_dwPK_Device_LocalOsdVfdLcd + m_bIsOSD + m_bNewOrbiter + m_bUseAlphaBlending + m_bUseMask + m_mapDesignObj
                        + m_mapPK_MediaType_PK_Attribute_Sort + m_mapPK_MediaType_Description + m_mapPK_AttributeType_Description + m_mapPK_Screen_GoBackToScreen
                        + m_mapScreen_Interrupt;

                (*this) + m_dequeLocation; // this is serialized custom
        }
        virtual string SerializeClassClassName() { return "qOrbiterData"; }
        // For our custom serialize types.

        virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition)
        {
                m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
                if( bWriting )
                {
                        switch(pItem->m_iSerializeDataType)
                        {
                        case SERIALIZE_DATA_TYPE_LOCATIONS:
                                {
                                        Write_unsigned_long((unsigned long) m_dequeLocation.size());
                                        DequeqLocationInfo::iterator it;
                                        for(it=m_dequeLocation.begin();it!=m_dequeLocation.end();++it)
                                        {
                                                if( !(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
                                                        return false;
                                        }
                                        return true;
                                }
                                break;
                        };
                }
                else
                {
                        switch(pItem->m_iSerializeDataType)
                        {

                        case SERIALIZE_DATA_TYPE_LOCATIONS:
                                {
                                        unsigned long count = Read_unsigned_long();
                                        for(unsigned long i=0;i<count;++i)
                                        {
                                                qLocationInfo *pqLocationInfo = new qLocationInfo();
                                                if( !pqLocationInfo->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
                                                        return false;
                                                m_dequeLocation.push_back(pqLocationInfo);
                                        }
                                        return true;
                                }
                                break;
                        };
                }
                return false;
        }
};
#endif // qOrbiterData_H
