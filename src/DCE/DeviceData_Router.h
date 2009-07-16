/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file DeviceData_Router.h
 A number of DCE classes.
 */

#ifndef DCEDEVICE_H
#define DCEDEVICE_H

#include "DeviceData_Impl.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Device_Device_Pipe.h"

class Row_Device_Device_Pipe;

namespace DCE {

    /** @class Pipe
    This is the class for moving data around the system.

    This indicates connectivity, that something is being sent somewhere else, such as audio/video connections.
    For example, the Movie Scaler is connected to the TV, using output ID 5 on the Scaler, and Input ID 3 on the TV
    There would be 2 pipes, with a destination of the TV.
    One an output with ID 5, one an Input with ID 3, both with Kind="Video".
    */
    class Pipe {
public:
        class DeviceData_Router *m_pDevice_From,*m_pDevice_To;
        class Command *m_pCommand_Input,*m_pCommand_Output;

        /** The ID of this pipe instance.
        */
        int m_PK_Pipe;

        /** Flags.
              A plugin or something can set this flag, and then off's won't get propagated.  That way the pipe
         that goes to the display devices for an on-screen orbiter, for example, can be set to true
         so that only the 'off' from the on screen orbiter will turn the rendering device off. */
        bool m_bDontSendOff,m_bDontSendOn;

        /** Flag.
              A plugin can set this flag, and then the input selects won't get propagated.
              That way an external media device, like a dvd, can be outputting using an overlay
               capture card, so the m/d's inputs are used
         instead, and DCERouter won't switch the inputs to the direct a/v pipes until this flag is cleared. */
        bool m_bDontSendInputs;

        /** Flag.
              It's possible for an output device, like a receiver, to have multiple embedded zones.
              If that's the case, the media handler
          may decide to create a temporary pipe to this output zone that is not one of the
              'normal' available pipes going to the main device.
          If so, it creates the pipe in the map of active pips, sets this
              flag m_bTemporary=true, and when we clear the active pipes, we'll delete it. */
        bool m_bTemporary;

        /** A constructor for creating temporary pipes.
        @param pDevice_From is the from device data router.
        @param pDevice_To is the to device data router.
        @param PK_Pipe is the pipe ID.
        @param pCommand_Input is the command in.
        @param pCommand_Output is the command out.
        @param bTemporary makes the pipe a temporary pipe.
        */
        Pipe(DeviceData_Router *pDevice_From,DeviceData_Router *pDevice_To,int PK_Pipe,Command *pCommand_Input,Command *pCommand_Output,bool bTemporary=false) {
            m_pDevice_From = pDevice_From;
            m_pDevice_To = pDevice_To;
            m_PK_Pipe = PK_Pipe;
            m_pCommand_Input = pCommand_Input;
            m_pCommand_Output = pCommand_Output;
            m_bDontSendInputs=m_bDontSendOff=m_bDontSendOn=false;
            m_bTemporary=bTemporary;
        }
    };

    typedef map<int, class Pipe *> MapPipe;  /**< Map of PK_Pipe (ie audio/video) to a pipe. */


    /** @class Command
    The Command class.
    This class holds each instnce of a command.
    All data is public - no accessors used here.
    */
    class Command {
public:
    /** Constructor.
    This builds a command object and stores all the data as public.
    @param PK_Command is the command.
    @param sDescription is the description of the command.
    @param PK_CommandCategory the command category
    @param sCommandCategory_Description describes the category.
    @param bLog is a flag to allow logging.
    */
        Command(int PK_Command,string sDescription,int PK_CommandCategory,string sCommandCategory_Description,bool bLog) {
            m_dwPK_Command=PK_Command;
            m_sDescription=sDescription;
            m_bLog=bLog;
            m_dwPK_CommandCategory=PK_CommandCategory;
            m_sCommandCategory_Description=sCommandCategory_Description;
        }
        bool m_bLog;
        int m_dwPK_Command,m_dwPK_CommandCategory;
        string m_sDescription,m_sCommandCategory_Description;

        list<int> m_listPipe;
    };


    /** @class Event_Router
    The Event_Router class stores the data about an event.
    */
    class Event_Router {
public:
        /** Constructor
        @param PK_Event is the event ID.
        @param sDescription is the description of the event.
        @param bLog is the log flag.
        */
        Event_Router(int PK_Event,string sDescription,bool bLog) {
            m_dwPK_Event=PK_Event;
            m_sDescription=sDescription;
            m_bLog=bLog;
        }
        bool m_bLog; /**< the log flag.  True means log the event. */
        int m_dwPK_Event; /**< The unique ID of the event. */
        string m_sDescription; /** The description of this event instance */
    };


    /** @class CommandGroup_Command
    The CommandGroup_Command class
    */
    class CommandGroup_Command {
public:
        Command *m_pCommand;
        class DeviceData_Router *m_pDeviceData_Router;
        map<int,string> m_mapParameter;

        CommandGroup_Command(Command *pCommand,class DeviceData_Router *pDeviceData_Router) {
            m_pCommand=pCommand;
            m_pDeviceData_Router=pDeviceData_Router;
        }

        ~CommandGroup_Command() {
            m_mapParameter.clear();
        }
    };


    /** @class CommandGroup
    The CommandGroup class
    */
    class CommandGroup {
public:
        int m_PK_CommandGroup;
        vector<class CommandGroup_Command *> m_vectCommandGroup_Command;
        string m_Description;
        int m_PK_C_Array;

CommandGroup(int PK_CommandGroup,int PK_C_Array=0) :
        m_PK_CommandGroup(PK_CommandGroup), m_PK_C_Array(PK_C_Array) {}

        ~CommandGroup() {
            vector<class CommandGroup_Command *>::iterator iA;

            for (iA = m_vectCommandGroup_Command.begin(); iA != m_vectCommandGroup_Command.end(); ++iA) {
                delete (*iA);
            }
        }
    };


    /** @class Room
    The Room class
    This class holds information about a room in the house.
    Rooms are stored in room maps and the public data allows access to
    the room information.
    */
    class Room {
public:
        unsigned long m_dwPK_Room; /**< Yhe room's ID. */
        string m_sDescription; /** The room's descrioption. */
        vector<class CommandGroup *> m_vectCommandGroups;
        list<class DeviceData_Router *> m_listDevices;

        /** Constructor.
        @param dwPK_Room is the room's ID.
        @param sDescription is a string describing the room.
        */
        Room(unsigned long dwPK_Room,string sDescription) {
            m_dwPK_Room = dwPK_Room;
            m_sDescription = sDescription;
        }
    };


    /** @class DeviceRelation
    The DeviceRelation class
    */
    class DeviceRelation {
public:
        class DeviceData_Router *m_pDevice;
        string m_sParms;

        /** Constructor.
        @param pDevice is
        @param sParms are the parameters for the device.
        */
        DeviceRelation(class DeviceData_Router *pDevice,string sParms) {
            m_pDevice=pDevice;
            m_sParms=sParms;
        }
    };


    /** @class DeviceData_Router
    The DeviceData_Router class.

    General ID's for this Device:
    These are in the base-> int m_dwPK_Device, m_dwPK_Installation,
    m_dwPK_DeviceTemplate,m_dwPK_DeviceCategory,m_dwPK_Room,;
    string m_sDescription;
    */
class DeviceData_Router : public DeviceData_Impl {
private:
        // The following are stored in the Device table so they persist

        /** A device can set a generic status message, such as "triggered".
        There is no predefined purpose.
        Status is considered a constantly changing value.
        */
        string m_sStatus;

        /** This is user defined.
        Examples are: on, off, 50%, bypassed, etc.
        State is the value that the user set and wants the device to stay in.
        */
        string m_sState;
public:

        // **** SERIALIZED VALUES FROM THE DATABASE ****

        // General ID's for this Device
        // These are in the base-> int m_dwPK_Device,m_dwPK_Installation,m_dwPK_DeviceTemplate,m_dwPK_DeviceCategory,m_dwPK_Room,;
        // string m_sDescription;

        /** If the device is running on a different installation,
              this will contain the IP address of the Router to send the messages to.
             */
        string m_sForeignRouter;

        /** The command line to execute for this device.
              */
        string m_sCommandLine;

        /** If set to true, the first time this device connects we will send it a reload.
        This is good for controllers which try to continually reconnect and
        may not realize the server has been restarted.
              */
        bool m_bForceReloadOnFirstConnect;

        /** If set to true, all on/off's to this device will be skipped.
              */
        bool m_bIgnoreOnOff;

        MapPipe m_mapPipe_Available; /**< The available pipes. */

        Pipe *m_mapPipe_Available_Find(int PK_Pipe) {
            map<int,class Pipe *>::iterator it = m_mapPipe_Available.find(PK_Pipe);
            return it==m_mapPipe_Available.end() ? NULL : (*it).second;
        }

        MapPipe m_mapPipe_Active; /**< The currently activated pipes. */
        Pipe *m_mapPipe_Active_Find(int PK_Pipe) {
            map<int,class Pipe *>::iterator it = m_mapPipe_Active.find(PK_Pipe);
            return it==m_mapPipe_Active.end() ? NULL : (*it).second;
        }

        /** The currently activated pipes that are not using the normal pipes.
        These are for handling separate embedded zones.
        The int a unique ID that will be the parameter '
        */
        map<int,MapPipe *> m_mapPipe_Temporary; /**< The PipeID to the map of a/v pipes. */
        MapPipe *m_mapPipe_Temporary_Find(int PK_PipeID) {
            map<int,MapPipe *>::iterator it = m_mapPipe_Temporary.find(PK_PipeID);
            return it==m_mapPipe_Temporary.end() ? NULL : (*it).second;
        }

        // All the groups, parameters, inputs, etc.
        map<int,class DeviceRelation *> m_mapDeviceRelation;
        map<int,class DeviceGroup *> m_mapDeviceGroup;
        vector<DeviceData_Router *> m_vectDevices_SendingPipes;  /**< The devices that send this one something. */
        vector<class MessageInterceptorCallBack *> m_vectMessageInterceptorCallBack;  /**< All interceptors this device has registered. */

        // **** FLAGS AND INFORMATION SET AT RUNTIME ****

        bool m_bIsRegistered,m_bIsReady; //!< Some devices will set this by sending a ready message/event
        bool m_bBusy; //!< A device can be marked busy.  It's messages will be queued until this flag is cleared
        bool m_bAlert;  //!< A device with this flag set is having a problem

        string m_IPAddr;
        time_t m_tCanReceiveNextCommand,m_tLastused;

        char *m_pMySerializedData;  //!< A copy of the device serialized so we don't have to do it for each request
        int m_iConfigSize;

        string m_sStatus_get() {
            return m_sStatus;
        }
        string m_sState_get() {
            return m_sState;
        }
        void m_sStatus_set(string sStatus) {
            // Do this manually since we don't want to reset the psc_mod, causing others to think something has changed for this device
            string sSQL = "UPDATE Device SET Status='" + StringUtils::SQLEscape(sStatus) + "',psc_mod=psc_mod WHERE PK_Device=" + StringUtils::itos(m_pRow_Device->PK_Device_get());
            m_pRow_Device->Table_Device_get()->Database_pluto_main_get()->threaded_db_wrapper_query(sSQL);
            m_pRow_Device->Reload();
            m_sStatus=sStatus;
        }
        void m_sState_set(string sState) {
            // Do this manually since we don't want to reset the psc_mod, causing others to think something has changed for this device
            string sSQL = "UPDATE Device SET State='" + StringUtils::SQLEscape(sState) + "',psc_mod=psc_mod WHERE PK_Device=" + StringUtils::itos(m_pRow_Device->PK_Device_get());
            m_pRow_Device->Table_Device_get()->Database_pluto_main_get()->threaded_db_wrapper_query(sSQL);
            m_pRow_Device->Reload();
            m_sState=sState;
        }

        // **** POINTERS CREATED BY THE SERIALIZED ID'S ****

        Room *m_pRoom;
        // A virtual device that doesn't really exist, but serves as a placeholder will have it's messages routed to another device
        // For example, a Television may have several tuners.  Each tuner must be a separate device so the user
        // can tune on any one of them.  They are all marked as RouteTo the television itself.
        class DeviceData_Router *m_pDevice_RouteTo;
        class DeviceData_Router *m_pDevice_Audio,*m_pDevice_Video;
        Row_Device *m_pRow_Device;

        map<int,class ServerSocket *> m_mapSocket_Event;
        class ServerSocket *m_pSocket_Command;


        DeviceData_Router(Row_Device *pRow_Device, Room *pRoom, string sCommandLine)
: DeviceData_Impl(pRow_Device->PK_Device_get(),pRow_Device->FK_Installation_get(),pRow_Device->FK_DeviceTemplate_get(),pRow_Device->FK_Device_ControlledVia_get(),pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get(),pRoom ? pRoom->m_dwPK_Room : 0,
                          pRow_Device->FK_DeviceTemplate_getrow()->ImplementsDCE_get()==1,
                          pRow_Device->FK_DeviceTemplate_getrow()->IsEmbedded_get()==1,sCommandLine,pRow_Device->FK_DeviceTemplate_getrow()->IsPlugIn_get()==1,pRow_Device->Description_get(),pRow_Device->IPaddress_get(),pRow_Device->MACaddress_get(),pRow_Device->FK_DeviceTemplate_getrow()->InheritsMacFromPC_get()==1, pRow_Device->Disabled_get()==1 ) {
            m_pRow_Device=pRow_Device;
            m_bForceReloadOnFirstConnect=m_bIsRegistered=m_bIsReady=m_bBusy=m_bAlert=false;
            m_tLastused=m_tCanReceiveNextCommand=0;
            m_sState = m_pRow_Device->State_get();
            m_sStatus = m_pRow_Device->Status_get();
            m_bIgnoreOnOff = m_pRow_Device->IgnoreOnOff_get()==1;

            m_pRoom=pRoom;
            m_pDevice_ControlledVia=m_pDevice_RouteTo=NULL;
            m_pDevice_Audio=m_pDevice_Video=NULL;
            m_pMySerializedData=NULL;
            m_pSocket_Command=NULL;
            m_iConfigSize=0;
        }

        /** Another constructor for dynamically loaded plug-ins.
        */
        DeviceData_Router(int PK_Device,int PK_DeviceTemplate,int PK_Installation, int PK_Device_ControlledVia)
: DeviceData_Impl(PK_Device,PK_Installation,PK_DeviceTemplate,PK_Device_ControlledVia,0 /* category */,0 /* room */,true /* implements dce */,false /* is embedded */,
                          "" /* Command line */,true /* Is Plugin */,"" /* Description */,"localhost","" /* Mac Address */, false, false) {
            m_pRow_Device=NULL;
            m_bForceReloadOnFirstConnect=m_bIsRegistered=m_bIsReady=m_bBusy=m_bAlert=false;
            m_tLastused=m_tCanReceiveNextCommand=0;

            m_pRoom=NULL;
            m_pDevice_ControlledVia=m_pDevice_RouteTo=NULL;
            m_pDevice_Audio=m_pDevice_Video=NULL;
            m_pMySerializedData=NULL;
            m_pSocket_Command=NULL;
            m_iConfigSize=0;
        }

        ~DeviceData_Router() {
            delete[] m_pMySerializedData;
            map<int,class DeviceRelation *>::iterator itMD;
            for(itMD=m_mapDeviceRelation.begin();itMD!=m_mapDeviceRelation.end();++itMD) {
                class DeviceRelation *pR = (*itMD).second;
                delete pR;
            }
        }

        DeviceRelation *mapDeviceRelation_Find(int PK_Device) {
            map<int,class DeviceRelation *>::iterator it = m_mapDeviceRelation.find(PK_Device);
            return it==m_mapDeviceRelation.end() ? NULL : (*it).second;
        }

        void FindSibblingsWithinCategory(DeviceCategory *pDeviceCategory,vector<DeviceData_Router *> &vectDeviceData_Router) {
            if( !pDeviceCategory || !m_pDevice_ControlledVia )
                return;
            for( size_t s=0;s<((DeviceData_Router *)m_pDevice_ControlledVia)->m_vectDeviceData_Impl_Children.size();++s ) {
                DeviceData_Router *pDevice = (DeviceData_Router *) ((DeviceData_Router *)m_pDevice_ControlledVia)->m_vectDeviceData_Impl_Children[s];
                if( pDevice->WithinCategory(pDeviceCategory) )
                    vectDeviceData_Router.push_back(pDevice);
            }

        }

        void FindSibblingsWithinCategory(int PK_DeviceCategory,vector<DeviceData_Router *> &vectDeviceData_Router) {
            if( !PK_DeviceCategory || !m_pDevice_ControlledVia )
                return;
            for( size_t s=0;s<((DeviceData_Router *)m_pDevice_ControlledVia)->m_vectDeviceData_Impl_Children.size();++s ) {
                DeviceData_Router *pDevice = (DeviceData_Router *) ((DeviceData_Router *)m_pDevice_ControlledVia)->m_vectDeviceData_Impl_Children[s];
                if( pDevice->WithinCategory(PK_DeviceCategory) )
                    vectDeviceData_Router.push_back(pDevice);
            }
        }

        void FindChildrenWithinCategory(int PK_DeviceCategory,vector<DeviceData_Router *> &vectDeviceData_Router) {
            if( !PK_DeviceCategory )
                return;
            for( size_t s=0;s<m_vectDeviceData_Impl_Children.size();++s ) {
                DeviceData_Router *pDevice = (DeviceData_Router *) m_vectDeviceData_Impl_Children[s];
                if( pDevice->WithinCategory(PK_DeviceCategory) )
                    vectDeviceData_Router.push_back(pDevice);
            }
        }

        // This will return the topmost device
        DeviceData_Router *GetTopMostDevice() {
            if( m_pDevice_ControlledVia )
                return ((DeviceData_Router *) m_pDevice_ControlledVia)->GetTopMostDevice();
            return this;
        }


        // This will return the first device within the given category that is in any way
        // related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
        DeviceData_Base *FindFirstRelatedDeviceOfCategory(int PK_DeviceCategory,class Command_Impl *pCommand_Impl_Confirm_Registration=NULL,int TimeoutToWait=20,bool bScanParent=true,int PK_Device_ExcludeChild=0);

        // This will return the first device of the given template that is in any way
        // related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
        DeviceData_Base *FindFirstRelatedDeviceOfTemplate(int PK_DeviceCategory,class Command_Impl *pCommand_Impl_Confirm_Registration=NULL,int TimeoutToWait=20,bool bScanParent=true,int PK_Device_ExcludeChild=0);


        // We're not going to use this, since we're not creating actual devices.  Implement this pure virtual function from our base class
        class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition) {
            return NULL;
        };
    };

    typedef list<DeviceData_Router *> ListDeviceData_Router;
}

#endif
