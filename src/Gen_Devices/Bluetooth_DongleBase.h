/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/** @file Bluetooth_DongleBase.h
A core piece of LinuxMCE.
*/
#ifndef Bluetooth_DongleBase_h
#define Bluetooth_DongleBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"
#include "Logger.h"


namespace DCE
{
//   OUR EVENT CLASS
/** @class Bluetooth_Dongle_Event
    All the events for a bluetooth dongle.

    This class has to do with the acquiring and release of an orbiter containing
    a bluetooth radio.

    Much of the code in here should probably be in the .cpp file in order to clean
    up this header file.
    */
class Bluetooth_Dongle_Event : public Event_Impl
{
public:
	Bluetooth_Dongle_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID,13, ServerAddress, bConnectEventHandler) {};
	Bluetooth_Dongle_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice );
	virtual void Mobile_orbiter_detected(string sMac_Address,int iSignal_Strength,string sID)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 4,3,5,sMac_Address.c_str(),6,StringUtils::itos(iSignal_Strength).c_str(),7,sID.c_str()));
	}

	virtual void Mobile_orbiter_linked(string sMac_Address,string sVersion)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 5,2,5,sMac_Address.c_str(),8,sVersion.c_str()));
	}

	virtual void Mobile_orbiter_lost(string sMac_Address,bool bConnectionFailed)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 6,2,5,sMac_Address.c_str(),11,(bConnectionFailed ? "1" : "0")));
	}

};


//   OUR DATA CLASS

/** @class Bluetooth_Dongle_Data
    This class deals with a bit of data about the bluetooth equiped orbiter.
*/
class Bluetooth_Dongle_Data : public DeviceData_Impl
{
public:
	virtual ~Bluetooth_Dongle_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 13; } ;
	virtual const char *GetDeviceDescription() { return "Bluetooth_Dongle"; } ;
};



//   OUR COMMAND CLASS

/** @class Bluetooth_Dongle_Command
    This class deals with the commands to communicate with a bluteooth equipped orbiter.

    Much of the code in here should probably be in the .cpp file in order to clean
    up this header file.
*/
class Bluetooth_Dongle_Command : public Command_Impl
{
public:
	Bluetooth_Dongle_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
	}
	virtual bool GetConfig()
	{

		m_pData=NULL;
		m_pEvent = new Bluetooth_Dongle_Event(m_dwPK_Device, m_sHostName, !m_bLocalMode);
		if( m_pEvent->m_dwPK_Device )
			m_dwPK_Device = m_pEvent->m_dwPK_Device;
		if( m_sIPAddress!=m_pEvent->m_pClientSocket->m_sIPAddress )
			m_sIPAddress=m_pEvent->m_pClientSocket->m_sIPAddress;
		m_sMacAddress=m_pEvent->m_pClientSocket->m_sMacAddress;
		if( m_pEvent->m_pClientSocket->m_eLastError!=cs_err_None )
		{
			if( m_pEvent->m_pClientSocket->m_eLastError==cs_err_BadDevice )
			{
				while( m_pEvent->m_pClientSocket->m_eLastError==cs_err_BadDevice && (m_dwPK_Device = DeviceIdInvalid())!=0 )
				{
					delete m_pEvent;
					m_pEvent = new Bluetooth_Dongle_Event(m_dwPK_Device, m_sHostName, !m_bLocalMode);
					if( m_pEvent->m_dwPK_Device )
						m_dwPK_Device = m_pEvent->m_dwPK_Device;
				}
			}
			if( m_pEvent->m_pClientSocket->m_eLastError==cs_err_NeedReload )
			{
				if( RouterNeedsReload() )
				{
					string sResponse;
					Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
					event_Impl.m_pClientSocket->SendString( "RELOAD" );
					if( !event_Impl.m_pClientSocket->ReceiveString( sResponse ) || sResponse!="OK" )
					{
						CannotReloadRouter();
						LoggerWrapper::GetInstance()->Write(LV_WARNING,"Reload request denied: %s",sResponse.c_str());
					}
				Sleep(10000);  // Give the router 10 seconds before we re-attempt, otherwise we'll get an error right away
				}
			}
		}

		if( m_bLocalMode )
		{
			m_pData = new Bluetooth_Dongle_Data();
			return true;
		}
		if( (m_pEvent->m_pClientSocket->m_eLastError!=cs_err_None && m_pEvent->m_pClientSocket->m_eLastError!=cs_err_NeedReload) || m_pEvent->m_pClientSocket->m_Socket==INVALID_SOCKET )
			return false;

		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			return false;
		m_pData = new Bluetooth_Dongle_Data();
		if( Size )
			m_pData->SerializeRead(Size,pConfig);
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete[] pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(m_dwPK_Device, 13,m_sHostName);
		if( m_iInstanceID )
		{
			m_pEvent->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
			m_pcRequestSocket->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
		}
		return true;
	};
	Bluetooth_Dongle_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Bluetooth_Dongle_Command() {};
	Bluetooth_Dongle_Event *GetEvents() { return (Bluetooth_Dongle_Event *) m_pEvent; };
	Bluetooth_Dongle_Data *GetData() { return (Bluetooth_Dongle_Data *) m_pData; };
	const char *GetClassName() { return "Bluetooth_Dongle_Command"; };
	virtual int PK_DeviceTemplate_get() { return 13; };
	static int PK_DeviceTemplate_get_static() { return 13; };
	virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	void EVENT_Mobile_orbiter_detected(string sMac_Address,int iSignal_Strength,string sID) { GetEvents()->Mobile_orbiter_detected(sMac_Address.c_str(),iSignal_Strength,sID.c_str()); }
	void EVENT_Mobile_orbiter_linked(string sMac_Address,string sVersion) { GetEvents()->Mobile_orbiter_linked(sMac_Address.c_str(),sVersion.c_str()); }
	void EVENT_Mobile_orbiter_lost(string sMac_Address,bool bConnectionFailed) { GetEvents()->Mobile_orbiter_lost(sMac_Address.c_str(),bConnectionFailed); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_Link_with_mobile_orbiter(string sMac_address,string sVMC_File,string sConfig_File,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Get_Signal_Strength(string sMac_address,int *iValue,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Create_Mobile_Orbiter(int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Ignore_MAC_Address(string sMac_address,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Disconnect_From_Mobile_Orbiter(string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual ReceivedMessageResult ReceivedMessage(class Message *pMessageOriginal)
	{
		map<long, string>::iterator itRepeat;
		if( Command_Impl::ReceivedMessage(pMessageOriginal)==rmr_Processed )
			return rmr_Processed;
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
			if (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND)
			{
				switch(pMessage->m_dwID)
				{
				case 60:
					{
						string sCMD_Result="OK";
					string sMac_address=pMessage->m_mapParameters[47];
					string sVMC_File=pMessage->m_mapParameters[118];
					string sConfig_File=pMessage->m_mapParameters[130];
						CMD_Link_with_mobile_orbiter(sMac_address.c_str(),sVMC_File.c_str(),sConfig_File.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Link_with_mobile_orbiter(sMac_address.c_str(),sVMC_File.c_str(),sConfig_File.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 61:
					{
						string sCMD_Result="OK";
					string sMac_address=pMessage->m_mapParameters[47];
					int iValue=atoi(pMessage->m_mapParameters[48].c_str());
						CMD_Get_Signal_Strength(sMac_address.c_str(),&iValue,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[48]=StringUtils::itos(iValue);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Get_Signal_Strength(sMac_address.c_str(),&iValue,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 62:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					string sMac_address=pMessage->m_mapParameters[47];
					int iPK_Room=atoi(pMessage->m_mapParameters[57].c_str());
						CMD_Create_Mobile_Orbiter(iPK_Device,sPK_EntertainArea.c_str(),sMac_address.c_str(),iPK_Room,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Create_Mobile_Orbiter(iPK_Device,sPK_EntertainArea.c_str(),sMac_address.c_str(),iPK_Room,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 332:
					{
						string sCMD_Result="OK";
					string sMac_address=pMessage->m_mapParameters[47];
						CMD_Ignore_MAC_Address(sMac_address.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Ignore_MAC_Address(sMac_address.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 333:
					{
						string sCMD_Result="OK";
					string sMac_address=pMessage->m_mapParameters[47];
					string sVMC_File=pMessage->m_mapParameters[118];
					int iDeviceToLink=atoi(pMessage->m_mapParameters[124].c_str());
					string sConfig_File=pMessage->m_mapParameters[130];
						CMD_Disconnect_From_Mobile_Orbiter(sMac_address.c_str(),sVMC_File.c_str(),iDeviceToLink,sConfig_File.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Disconnect_From_Mobile_Orbiter(sMac_address.c_str(),sVMC_File.c_str(),iDeviceToLink,sConfig_File.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				}
				iHandled += Command_Impl::ReceivedMessage(pMessage);
			}
			else if( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND )
			{
				MapCommand_Impl::iterator it = m_mapCommandImpl_Children.find(pMessage->m_dwPK_Device_To);
				if( it!=m_mapCommandImpl_Children.end() && !(*it).second->m_bGeneric )
				{
					Command_Impl *pCommand_Impl = (*it).second;
					iHandled += pCommand_Impl->ReceivedMessage(pMessage);
			}
			else
			{
				DeviceData_Impl *pDeviceData_Impl = m_pData->FindChild(pMessage->m_dwPK_Device_To);
				string sCMD_Result="UNHANDLED";
				if( pDeviceData_Impl )
					ReceivedCommandForChild(pDeviceData_Impl,sCMD_Result,pMessage);
				else
					ReceivedUnknownCommand(sCMD_Result,pMessage);
					if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
					{
							pMessage->m_bRespondedToMessage=true;
						Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[0]=sCMD_Result;
						SendMessage(pMessageOut);
					}
					else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
						SendString(sCMD_Result);
						}
					if( sCMD_Result!="UNHANDLED" && sCMD_Result!="UNKNOWN DEVICE" )
						iHandled++;
				}
			}
			if( iHandled==0 && !pMessage->m_bRespondedToMessage &&
			(pMessage->m_eExpectedResponse==ER_ReplyMessage || pMessage->m_eExpectedResponse==ER_ReplyString) )
			{
				pMessage->m_bRespondedToMessage=true;
				if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
				{
					Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
					pMessageOut->m_mapParameters[0]="UNHANDLED";
					SendMessage(pMessageOut);
				}
				else
					SendString("UNHANDLED");
			}
		}
		return iHandled!=0 ? rmr_Processed : rmr_NotProcessed;
	}
}; // end class


}
#endif
