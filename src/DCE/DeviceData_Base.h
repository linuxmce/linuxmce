/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/**
 * @file DeviceData_Base.h
 Header file for the DeviceCategory, DeviceData_Base and AllDevices classes

 @todo not commented
 */
#ifndef DEVICEDATA_BASE_H
#define DEVICEDATA_BASE_H

#include "SerializeClass/SerializeClass.h"
#include "pluto_main/Define_DeviceCategory.h"

#define SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE          2000
#define SERIALIZE_DATA_TYPE_MAP_DCECATEGORY                 2001
#define SERIALIZE_DATA_TYPE_MAP_DCEDEVICE_GROUP         2002

#define DCE_SCHEMA          1

namespace DCE {
    typedef map<int,class Command_Base *> Map_Command_Base;
    typedef map<int,class DeviceData_Base *> Map_DeviceData_Base;
    typedef map<int,class DeviceCategory *> Map_DeviceCategory;
    typedef map<int,class DeviceGroup *> Map_DeviceGroup;

    /**
       @class DeviceCategory
     This class contains information about a device category
     */
class DeviceCategory : public SerializeClass {

public:

        long m_dwPK_DeviceCategory; /**< an identifier for the device category */
        long m_dwPK_DeviceCategory_Parent; /**< an identifier for the parent category for this device category */
        string m_sDescription; /**< a short description of the device category */

        class DeviceCategory *m_pDeviceCategory_Parent; /**< pointer to the parent category */

        /**
         * @brief constructor the assignes values to the member data
         */
        DeviceCategory( unsigned long dwPK_DeviceCategory, unsigned long dwPK_DeviceCategory_Parent, string sDescription ) {
            m_dwPK_DeviceCategory = dwPK_DeviceCategory;
            m_dwPK_DeviceCategory_Parent = dwPK_DeviceCategory_Parent;
            m_sDescription = sDescription;
            m_pDeviceCategory_Parent = NULL;
        }

        /**
         * @brief constructor that assignes NULL to the pointer member data (for de-serializing)
         */
        DeviceCategory() {
            m_pDeviceCategory_Parent = NULL;
        }

        /**
         * @brief setups serialization for the class data
         */
        void SetupSerialization(int /*iSC_Version*/) {
            StartSerializeList() + m_dwPK_DeviceCategory + m_dwPK_DeviceCategory_Parent + m_sDescription;
        }

        /**
         * @brief returns "DeviceCategory" so that the SerializeClass knows what it has to serialize
         */
        virtual string SerializeClassClassName() {
            return "DeviceCategory";
        }

        bool WithinCategory(int PK_DeviceCategory) {
            if( PK_DeviceCategory==m_dwPK_DeviceCategory )
                return true;
            if( m_pDeviceCategory_Parent )
                return m_pDeviceCategory_Parent->WithinCategory(PK_DeviceCategory);
            return false;
        }
    };


    /** @class AllDevices
     A class to contain all the devices, that can be serialized and de-serialized
     */
class AllDevices : public SerializeClass {
        // Normally this class should delete all the pointers it creates.  But if for some reason
        // something needs the pointers to survive, it can call DontDeleteData
        bool m_bDontDeleteData;
public:
        AllDevices() {
            m_bDontDeleteData=false;
            m_iSC_Version = DCE_SCHEMA;
        }
        virtual ~AllDevices();
        void DontDeleteData() {
            m_bDontDeleteData=true;
        }

        Map_DeviceData_Base m_mapDeviceData_Base; /**< a map with all the devices */
        Map_DeviceCategory m_mapDeviceCategory; /**< a map with all the device categories */
        Map_DeviceGroup m_mapDeviceGroup; /**< a map with all the device groups */

        /**
         * @brief finds the DeviceData_Base item associated with the specified device
         */
        DeviceData_Base *m_mapDeviceData_Base_Find( unsigned long dwPK_Device ) {
            map<int,class DeviceData_Base *>::iterator it = m_mapDeviceData_Base.find( dwPK_Device );
            return it == m_mapDeviceData_Base.end() ? NULL : (*it).second;
        }

        /**
         * @brief finds the DeviceData_Base item associated with the specified device
         */
        DeviceData_Base *m_mapDeviceData_Base_FindFirstOfCategory( unsigned long dwPK_DeviceCategory );

        /**
         * @brief finds the DeviceData_Base item associated with the specified device
         */
        DeviceData_Base *m_mapDeviceData_Base_FindFirstOfTemplate( long dwPK_DeviceTemplate );

        /**
         * @brief finds the DeviceCategory item associated with the specified device category id
         */
        DeviceCategory *m_mapDeviceCategory_Find( unsigned long dwPK_DeviceCategory ) {
            Map_DeviceCategory::iterator it = m_mapDeviceCategory.find( dwPK_DeviceCategory );
            return it == m_mapDeviceCategory.end() ? NULL : (*it).second;
        }

        /**
         * @brief finds the DeviceGroup item associated with the specified device Group id
         */
        DeviceGroup *m_mapDeviceGroup_Find( unsigned long dwPK_DeviceGroup ) {
            Map_DeviceGroup::iterator it = m_mapDeviceGroup.find( dwPK_DeviceGroup );
            return it == m_mapDeviceGroup.end() ? NULL : (*it).second;
        }

        /**
         * @brief setups serialization for the class member data
         * @warning the maps are serialized custom
         * @see the overloads for the + operator
         */
        void SetupSerialization(int /*iSC_Version*/) {
            (*this) + m_mapDeviceData_Base + m_mapDeviceCategory + m_mapDeviceGroup; // this is serialized custom
        }

        virtual bool OkayToDeserialize(int iSC_Version) {
            return iSC_Version==DCE_SCHEMA;
        }

        /**
         * @returns "AllDevices" so that the SerializeClass knows what to serialize
         */
        virtual string SerializeClassClassName() {
            return "AllDevices";
        }

        /** overloding + for our custom serialize types */


        /**
         * @brief adds a new ItemToSerialize to the m_vectItemToSerialize list (the DeviceData_Base map)
         */
        AllDevices &operator+ ( Map_DeviceData_Base &i ) {
            m_vectItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE, (void *) &i) );
            return (*this);
        }

        /**
         * @brief adds a new ItemToSerialize to the m_vectItemToSerialize list (the DeviceCategory map)
         */
        AllDevices &operator+ ( Map_DeviceCategory &i ) {
            m_vectItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_MAP_DCECATEGORY, (void *) &i ) );
            return (*this);
        }

        /**
         * @brief adds a new ItemToSerialize to the m_vectItemToSerialize list (the DeviceGroup vector)
         */
        AllDevices &operator+ ( Map_DeviceGroup &i ) {
            m_vectItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_MAP_DCEDEVICE_GROUP, (void *) &i) );
            return (*this);
        }

        /**
         * @brief overrides the SerializeClass::UnknownSerialize method
         * @todo ask
         */
        virtual bool UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition );

        /**
         * @brief
         * we'll override this so we can match up the categories after de-serializing
         */
        virtual bool Serialize( bool bWriting,char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL );
    };

    /** @class DeviceGroup
     Contains a group of devices
     A DeviceGroup is just a user-defined group of devices.
       The purpose of the group depends on the implementation.
       This class does nothing but store the devices within the group.
     */
class DeviceGroup : public SerializeClass {
public:
        int m_dwPK_DeviceGroup;
        string m_sDescription;
        int m_Type;
        vector<int> m_vectPK_Device;
        vector<class DeviceData_Base *> m_vectDeviceData_Base;

        DeviceGroup(int dwPK_DeviceGroup,string sDescription,int Type) {
            m_sDescription=sDescription;
            m_dwPK_DeviceGroup=dwPK_DeviceGroup;
            m_Type=Type;
        }
        DeviceGroup() {}

        void SetupSerialization(int /*iSC_Version*/) {
            StartSerializeList() + m_dwPK_DeviceGroup + m_sDescription + m_vectPK_Device;
        }
    };


    /** @class DeviceData_Base
    This is the lowest level base class that contains primitive information about a Device.

    Every DCE device is derived from this base class.
    The higher level classes which contain more information needed
    for the DCE Implementation: DeviceData_Impl and for the router:
    DeviceData_Router are derived from this base class.
    */
class DeviceData_Base : public SerializeClass {
public:

        /** fields that corespond to primary keys */

        long m_dwPK_Device; /**< the ID of the device */
        unsigned long m_dwPK_Installation; /**< the installation associated with the device @todo ask*/
        long m_dwPK_DeviceTemplate; /**< the identifier for the template used to build the device */
        long m_dwPK_Device_ControlledVia; /**< the specifies how the device is controlled @todo ask */
        unsigned long m_dwPK_DeviceCategory; /**< the device category */
        unsigned long m_dwPK_Room; /**< identifies the room where the device resides */
        bool m_bDisabled; /**< specifies if device is disabled */

        long m_dwPK_Device_MD,m_dwPK_Device_Core; /**< the ID of the MD and Core, if there is one */

        /** flags */

        bool m_bImplementsDCE; /**< @todo ask */
        bool m_bIsEmbedded; /**< specifies if the device is embedded */
        bool m_bIsPlugIn; /**< specifies if the device is a plugin */
        bool m_bDeviceData_Impl;  /**< true if this is created as a DeviceData_Impl */

        /** other info */

        string m_sCommandLine; /**< the command line for the device @todo ask */
        string m_sIPAddress; /**< the IP address for the device */
        string m_sMacAddress; /**< the MAC address for the device */
        string m_sDescription; /**< freeform description for the device */

        bool m_bInheritsMacFromPC; /**< If true, this will return the MAC Address from the controlled_via tree that is a pc, otherwise it returns its own m_sMacAddress */

        map<int,string> m_mapCommands; /**< map of commands for the device */
        AllDevices m_AllDevices;  /**< all the devices in the system */
        DeviceCategory *m_pDeviceCategory; /**< pointer to a DeviceCategory object to witch this device belongs */
        vector<DeviceGroup *> m_vectDeviceGroup; /**< the groups we are a member of */
        vector<DeviceData_Base *> m_vectDeviceData_Base_Children; /**< Only populated for instances within m_AllDevices */

        /** @class
         * @brief pointer to devices controlling this one
         * @todo ask
         * @warning whatever creates this will need to set these pointers
         */
        class DeviceData_Base *m_pDevice_ControlledVia,*m_pDevice_Core,*m_pDevice_MD;

        /** @todo check comment */
        /*
        // When this class is being de-serialized the following constructor will be called
        DCEDeviceData_Base(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition,int x)
        {
            m_pDCEDeviceCategory=NULL;
            m_pDevice_ControlledVia=NULL;
            ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);

            m_iPK_Device = Read_unsigned_long();
            m_iPK_Installation = Read_unsigned_long();
            m_iPK_DeviceTemplate = Read_unsigned_long();
            m_iPK_Device_ControlledVia = Read_unsigned_long();
            m_iPK_DeviceCategory=Read_unsigned_long();
            m_iPK_Room=Read_unsigned_long();

            m_bImplementsDCE = (Read_unsigned_char()==1);
            m_iNumberCommands = Read_unsigned_long();
            if( m_iNumberCommands )
            {
                m_piCommands = new unsigned long[m_iNumberCommands];
                for(unsigned long iCommands=0;iCommands<m_iNumberCommands;++iCommands)
                    m_piCommands[iCommands] = Read_unsigned_long();
            }
            else
                m_piCommands=NULL;
        }
        */

        /** Default constructor.
              Assigns default values to the member data.
         */
        DeviceData_Base() {
            m_pDeviceCategory = NULL;
            m_pDevice_ControlledVia = m_pDevice_Core = m_pDevice_MD = NULL;
            m_bDeviceData_Impl = m_bIsPlugIn = false;
            m_dwPK_Device=m_dwPK_Installation=m_dwPK_DeviceTemplate=m_dwPK_Device_ControlledVia=m_dwPK_DeviceCategory=
                                                  m_dwPK_Room=m_dwPK_Device_MD=m_dwPK_Device_Core=0;
            m_bDisabled = false;
            m_iSC_Version = DCE_SCHEMA;
        }

        /** Constructor.
              @param dwPK_Device is the Device ID.
              @param dwPK_Installation is ?
              @param dwPK_DeviceTemplate is ?
              @param dwPK_Device_ControlledVia is ??
              @param dwPK_DeviceCategory is ?
              @param dwPK_Room is the room ID where the device is located.
              @param bImplementsDCE is a flag ?
              @param bIsEmbedded is a flag ?
              @param sCommandLine is the command string.
              @param bIsPlugIn is the plugin flag.
              @param sDescription is the device description.
              @param sMacAddress is the Ethernet MAC address of the device.
              @param bInheritsMacFromPC is ?
              @param bDisabled is a flag.

             Assigns specified values to the member data.
             All parameters are required.
             Call this contstructor to create it dynamically
         */
        DeviceData_Base( unsigned long dwPK_Device, unsigned long dwPK_Installation, unsigned long dwPK_DeviceTemplate,
                         unsigned long dwPK_Device_ControlledVia,
                         unsigned long dwPK_DeviceCategory, unsigned long dwPK_Room, bool bImplementsDCE, bool bIsEmbedded,
                         string sCommandLine, bool bIsPlugIn,
                         string sDescription, string sIPAddress, string sMacAddress, bool bInheritsMacFromPC, bool bDisabled ) {
            m_pDeviceCategory = NULL;
            m_pDevice_ControlledVia = NULL;
            m_dwPK_Device = dwPK_Device;
            m_dwPK_Device_MD = m_dwPK_Device_Core = 0;
            m_dwPK_Installation = dwPK_Installation;
            m_dwPK_DeviceTemplate = dwPK_DeviceTemplate;
            m_dwPK_Device_ControlledVia = dwPK_Device_ControlledVia;
            m_dwPK_DeviceCategory = dwPK_DeviceCategory;
            m_dwPK_Room = dwPK_Room;
            m_bImplementsDCE = bImplementsDCE;
            m_bIsEmbedded = bIsEmbedded;
            m_sCommandLine = sCommandLine;
            m_bIsPlugIn = bIsPlugIn;
            m_sDescription=sDescription;
            m_sIPAddress = sIPAddress;
            m_sMacAddress = sMacAddress;
            m_bInheritsMacFromPC = bInheritsMacFromPC;
            m_bDeviceData_Impl = false;
            m_bDisabled = bDisabled;
            m_iSC_Version = DCE_SCHEMA;
        }

        /**
         * @brief setups serialization for the class data
         */
        void SetupSerialization(int /*iSC_Version*/) {
            StartSerializeList() + m_bImplementsDCE + m_dwPK_Device + m_dwPK_Installation + m_dwPK_DeviceTemplate + m_dwPK_Device_ControlledVia +
            m_dwPK_DeviceCategory + m_dwPK_Room + m_bIsPlugIn + m_bIsEmbedded + m_sCommandLine + m_mapCommands + m_sDescription + m_sIPAddress + m_sMacAddress +
            m_bInheritsMacFromPC + m_dwPK_Device_MD + m_dwPK_Device_Core + m_bDisabled;
        }

        virtual bool OkayToDeserialize(int iSC_Version) {
            return iSC_Version==DCE_SCHEMA;
        }

        /**
         * @brief searches to see if the device belongs to the specified category
         * if it isn't the device exact category it goes up to the device parents
         */
        bool WithinCategory( long dwPK_DeviceCategory, DeviceCategory *pStarting=NULL ) {
            if ( !pStarting ) // no device category to start with supplied
                pStarting = m_pDeviceCategory;

            if ( !pStarting ) // the m_pDeviceCategory member data is not assigned
                return false;

            if ( pStarting->m_dwPK_DeviceCategory == dwPK_DeviceCategory ) // the device belongs to the specified category
                return true;

            if ( !pStarting->m_pDeviceCategory_Parent ) // the current device category is top-level (has no parent)
                return false;

            return WithinCategory( dwPK_DeviceCategory, pStarting->m_pDeviceCategory_Parent ); // call it recursivelly for the parent
        }

        /**
         * @brief searches to see if the device belongs to the specified category
         * same as WithinCategory( unsigned long ... ) just that the device category is specified through a pointer to a device category
         * @see the other WithinCategory method
         */
        bool WithinCategory( DeviceCategory *pCategory, DeviceCategory *pStarting=NULL ) {
            if( !pStarting ) // no device category to start with supplied
                pStarting = m_pDeviceCategory;

            if( !pStarting ) // the m_pDeviceCategory member data is not assigned
                return false;

            if( pStarting->m_dwPK_DeviceCategory == pCategory->m_dwPK_DeviceCategory ) // the device belongs to the specified category
                return true;

            if( !pStarting->m_pDeviceCategory_Parent ) // the current device category is top-level (has no parent)
                return false;

            return WithinCategory( pCategory, pStarting->m_pDeviceCategory_Parent ); // call it recursivelly for the parent
        }

        /*
         * @brief Find self or parent with category
         */
        DeviceData_Base *FindSelfOrParentWithinCategory( unsigned long dwPK_DeviceCategory ) {
            if( WithinCategory(dwPK_DeviceCategory) )
                return this;
            if( m_pDevice_ControlledVia )
                return m_pDevice_ControlledVia->FindSelfOrParentWithinCategory(dwPK_DeviceCategory);
            return NULL;
        }

        bool IsChildOf(DeviceData_Base *pDeviceData_Base) {
            if( pDeviceData_Base->m_dwPK_Device == m_dwPK_Device_ControlledVia )
                return true;
            if( !m_pDevice_ControlledVia )
                return false;
            return m_pDevice_ControlledVia->IsChildOf(pDeviceData_Base);
        }

        // This will return the topmost device
        DeviceData_Base *GetTopMostDevice() {
            if( m_bDeviceData_Impl ) {
                DeviceData_Base *pDeviceData_Base = m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device);
                if( pDeviceData_Base )
                    return pDeviceData_Base->GetTopMostDevice();
                else
                    return NULL;
            }

            if( m_pDevice_ControlledVia )
                return m_pDevice_ControlledVia->GetTopMostDevice();
            return this;
        }

        // This will return the first device within the given category that is in any way
        // related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
        // If pCommand_Impl_Confirm_Registration is not null, that will be used to determine if the device registered, and will wait for it to register, and will not return the device unless it is
        DeviceData_Base *FindFirstRelatedDeviceOfCategory(int PK_DeviceCategory,class Command_Impl *pCommand_Impl_Confirm_Registration=NULL,int TimeoutToWait=20,bool bScanParent=true,int PK_Device_ExcludeChild=0);

        // This will return the first device of the given template that is in any way
        // related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
        // If pCommand_Impl_Confirm_Registration is not null, that will be used to determine if the device registered, and will wait for it to register, and will not return the device unless it is
        DeviceData_Base *FindFirstRelatedDeviceOfTemplate(int PK_DeviceTemplate,class Command_Impl *pCommand_Impl_Confirm_Registration=NULL,int TimeoutToWait=20,bool bScanParent=true, int PK_Device_ExcludeChild = 0);
        /**
         * @brief checks if the specified command is supported by the device
         */
        bool SupportsCommand( unsigned long dwPK_Command ) {
            return m_mapCommands.find( dwPK_Command ) != m_mapCommands.end();
        }

        string GetMacAddress() {
            if( !m_bInheritsMacFromPC )
                return m_sMacAddress;

            if( WithinCategory(DEVICECATEGORY_Computers_CONST) )
                return m_sMacAddress;

            DeviceData_Base *pParent = m_pDevice_ControlledVia;
            while( pParent && !pParent->WithinCategory(DEVICECATEGORY_Computers_CONST) )
                pParent = pParent->m_pDevice_ControlledVia;

            if( pParent )
                return pParent->m_sMacAddress;
            else
                return "";
        }

        string GetIPAddress() {
            if( !m_bInheritsMacFromPC )
                return m_sIPAddress;

            if( WithinCategory(DEVICECATEGORY_Computers_CONST) )
                return m_sIPAddress;

            DeviceData_Base *pParent = m_pDevice_ControlledVia;
            while( pParent && !pParent->WithinCategory(DEVICECATEGORY_Computers_CONST) )
                pParent = pParent->m_pDevice_ControlledVia;

            if( pParent )
                return pParent->m_sIPAddress;
            else
                return "";
        }
    };
}

#endif
