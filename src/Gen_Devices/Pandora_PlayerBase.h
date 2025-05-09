#ifndef Pandora_PlayerBase_h
#define Pandora_PlayerBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"
#include "Logger.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_EventParameter.h"
#include "../pluto_main/Define_DeviceData.h"


/**
* THESE CLASSES ARE AUTOGENERATED WITH PLUTO DCEGEN APPLICATION
* THIS FILE SHOULD NOT BE MODIFIED MANUALLY
*/

namespace DCE
{

/**
* @brief OUR EVENT CLASS
*/

class Pandora_Player_Event : public Event_Impl
{
public:

	/**
	* @brief Constructors
	*/
	Pandora_Player_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) :
		Event_Impl(DeviceID, DEVICETEMPLATE_Pandora_Player_CONST, ServerAddress, bConnectEventHandler, SOCKET_TIMEOUT) {};
	Pandora_Player_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};

	/**
	* @brief Events builder method
	*/
	class Event_Impl *CreateEvent(unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice);

	/**
	* @brief Events methods for our device
	*/

	virtual void Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Playback_Info_Changed_CONST,
			3 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_MediaDescription_CONST, sMediaDescription.c_str(),
			EVENTPARAMETER_SectionDescription_CONST, sSectionDescription.c_str(),
			EVENTPARAMETER_SynposisDescription_CONST, sSynposisDescription.c_str()));
	}

	virtual void Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Playback_Completed_CONST,
			3 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_MRL_CONST, sMRL.c_str(),
			EVENTPARAMETER_Stream_ID_CONST, StringUtils::itos(iStream_ID).c_str(),
			EVENTPARAMETER_With_Errors_CONST, (bWith_Errors ? "1" : "0")));
	}

	virtual void Media_Description_Changed(string sText)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Media_Description_Changed_CONST,
			1 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_Text_CONST, sText.c_str()));
	}

	virtual void Playback_Started(string sMRL,int iStream_ID,string sSectionDescription,string sAudio,string sVideo)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Playback_Started_CONST,
			5 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_MRL_CONST, sMRL.c_str(),
			EVENTPARAMETER_Stream_ID_CONST, StringUtils::itos(iStream_ID).c_str(),
			EVENTPARAMETER_SectionDescription_CONST, sSectionDescription.c_str(),
			EVENTPARAMETER_Audio_CONST, sAudio.c_str(),
			EVENTPARAMETER_Video_CONST, sVideo.c_str()));
	}

};


/**
* @brief OUR DATA CLASS
*/

class Pandora_Player_Data : public DeviceData_Impl
{
public:

	/**
	* @brief No-op destructor
	*/
	virtual ~Pandora_Player_Data() {};

	/**
	* @brief Builder data method
	*/
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent, char *pDataBlock, 
		unsigned long AllocatedSize, char *CurrentPosition);

	/**
	* @brief Returns the id of the device template
	*/
	virtual int GetPK_DeviceList() { return DEVICETEMPLATE_Pandora_Player_CONST; } ;

	/**
	* @brief Returns the description of the device
	*/
	virtual const char *GetDeviceDescription() { return "Pandora_Player"; } ;

	/**
	* @brief Device data access methods:
	*/

	string Get_Alsa_Output_Device()
	{
		if( m_bRunningWithoutDeviceData )
			return m_pEvent_Impl->GetDeviceDataFromDatabase(m_dwPK_Device,DEVICEDATA_Alsa_Output_Device_CONST);
		else
			return m_mapParameters[DEVICEDATA_Alsa_Output_Device_CONST];
	}

	int Get_Time_Code_Report_Frequency()
	{
		if( m_bRunningWithoutDeviceData )
			return atoi(m_pEvent_Impl->GetDeviceDataFromDatabase(m_dwPK_Device,DEVICEDATA_Time_Code_Report_Frequency_CONST).c_str());
		else
			return atoi(m_mapParameters[DEVICEDATA_Time_Code_Report_Frequency_CONST].c_str());
	}

	int Get_Port()
	{
		if( m_bRunningWithoutDeviceData )
			return atoi(m_pEvent_Impl->GetDeviceDataFromDatabase(m_dwPK_Device,DEVICEDATA_Port_CONST).c_str());
		else
			return atoi(m_mapParameters[DEVICEDATA_Port_CONST].c_str());
	}

};



//   OUR COMMAND CLASS 

class Pandora_Player_Command : public Command_Impl
{
public:
	Pandora_Player_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
	}
	virtual bool GetConfig()
	{
		m_pData=NULL;
		m_pEvent = new Pandora_Player_Event(m_dwPK_Device, m_sHostName, !m_bLocalMode);
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
					m_pEvent = new Pandora_Player_Event(m_dwPK_Device, m_sHostName, !m_bLocalMode);
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
			m_pData = new Pandora_Player_Data();
			return true;
		}
		if( (m_pEvent->m_pClientSocket->m_eLastError!=cs_err_None && m_pEvent->m_pClientSocket->m_eLastError!=cs_err_NeedReload) || m_pEvent->m_pClientSocket->m_Socket==INVALID_SOCKET )
			return false;

		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			return false;
		m_pData = new Pandora_Player_Data();
		if( Size )
		{
			if( m_pData->SerializeRead(Size,pConfig)==false )
				return false;
		}
		else
		{
			m_pData->m_dwPK_Device=m_dwPK_Device;  // Assign this here since it didn't get it's own data
			string sResponse;
			Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
			event_Impl.m_pClientSocket->SendString( "PARENT " + StringUtils::itos(m_dwPK_Device) );
			if( event_Impl.m_pClientSocket->ReceiveString( sResponse ) && sResponse.size()>=8 )
				m_pData->m_dwPK_Device_ControlledVia = atoi( sResponse.substr(7).c_str() );
			m_pData->m_bRunningWithoutDeviceData=true;
		}
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		if( m_pData->m_AllDevices.SerializeRead(Size,pConfig)==false )
			return false;
		delete[] pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(m_dwPK_Device, DEVICETEMPLATE_Pandora_Player_CONST,m_sHostName);
		if( m_iInstanceID )
		{
			m_pEvent->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
			m_pcRequestSocket->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
		}
		PostConfigCleanup();
		return true;
	};
	Pandora_Player_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Pandora_Player_Command() {};
	Pandora_Player_Event *GetEvents() { return (Pandora_Player_Event *) m_pEvent; };
	Pandora_Player_Data *GetData() { return (Pandora_Player_Data *) m_pData; };
	const char *GetClassName() { return "Pandora_Player_Command"; };
	virtual int PK_DeviceTemplate_get() { return DEVICETEMPLATE_Pandora_Player_CONST; };
	static int PK_DeviceTemplate_get_static() { return DEVICETEMPLATE_Pandora_Player_CONST; };
	virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	string DATA_Get_Alsa_Output_Device() { return GetData()->Get_Alsa_Output_Device(); }
	int DATA_Get_Time_Code_Report_Frequency() { return GetData()->Get_Time_Code_Report_Frequency(); }
	int DATA_Get_Port() { return GetData()->Get_Port(); }
	//Event accessors
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription) { GetEvents()->Playback_Info_Changed(sMediaDescription.c_str(),sSectionDescription.c_str(),sSynposisDescription.c_str()); }
	void EVENT_Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors) { GetEvents()->Playback_Completed(sMRL.c_str(),iStream_ID,bWith_Errors); }
	void EVENT_Media_Description_Changed(string sText) { GetEvents()->Media_Description_Changed(sText.c_str()); }
	void EVENT_Playback_Started(string sMRL,int iStream_ID,string sSectionDescription,string sAudio,string sVideo) { GetEvents()->Playback_Started(sMRL.c_str(),iStream_ID,sSectionDescription.c_str(),sAudio.c_str(),sVideo.c_str()); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Pause(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Play(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Thumbs_Down(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Thumbs_Up(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Add_Station(string sType,int iStreamID,string sName,string *sID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Remove_Station(string sID,int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Rename_Station(string sID,int iStreamID,string sName,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Explain_Song(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Add_Music_to_Station(string sID,int iStreamID,string sName,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Add_Station_to_QuickMix(string sID,int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Remove_Station_from_QuickMix(string sID,int iStreamID,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual ReceivedMessageResult ReceivedMessage(class Message *pMessageOriginal)
	{
		map<long, string>::iterator itRepeat;
		if( Command_Impl::ReceivedMessage(pMessageOriginal)==rmr_Processed )
		{
			if( pMessageOriginal->m_eExpectedResponse==ER_ReplyMessage && !pMessageOriginal->m_bRespondedToMessage )
			{
				pMessageOriginal->m_bRespondedToMessage=true;
				Message *pMessageOut=new Message(m_dwPK_Device,pMessageOriginal->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
				pMessageOut->m_mapParameters[0]="OK";
				SendMessage(pMessageOut);
			}
			else if( (pMessageOriginal->m_eExpectedResponse==ER_DeliveryConfirmation || pMessageOriginal->m_eExpectedResponse==ER_ReplyString) && !pMessageOriginal->m_bRespondedToMessage )
			{
				pMessageOriginal->m_bRespondedToMessage=true;
				SendString("OK");
			}
			return rmr_Processed;
		}
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
			if (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND)
			{
				// Only buffer single messages, otherwise the caller won't know which messages were buffered and which weren't
				if( m_pMessageBuffer && pMessage->m_bCanBuffer && pMessageOriginal->m_vectExtraMessages.size()==1 && m_pMessageBuffer->BufferMessage(pMessage) )
					return rmr_Buffered;
				switch(pMessage->m_dwID)
				{
				case COMMAND_Play_Media_CONST:
					{
						string sCMD_Result="OK";
						int iPK_MediaType=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_MediaType_CONST].c_str());
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						string sMediaPosition=pMessage->m_mapParameters[COMMANDPARAMETER_MediaPosition_CONST];
						string sMediaURL=pMessage->m_mapParameters[COMMANDPARAMETER_MediaURL_CONST];
						CMD_Play_Media(iPK_MediaType,iStreamID,sMediaPosition.c_str(),sMediaURL.c_str(),sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Play_Media(iPK_MediaType,iStreamID,sMediaPosition.c_str(),sMediaURL.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Restart_Media_CONST:
					{
						string sCMD_Result="OK";
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Restart_Media(iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Restart_Media(iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Skip_Fwd_ChannelTrack_Greater_CONST:
					{
						string sCMD_Result="OK";
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Jump_Position_In_Playlist_CONST:
					{
						string sCMD_Result="OK";
						string sValue_To_Assign=pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST];
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Pause_CONST:
					{
						string sCMD_Result="OK";
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Pause(iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Pause(iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Play_CONST:
					{
						string sCMD_Result="OK";
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Play(iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Play(iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Thumbs_Down_CONST:
					{
						string sCMD_Result="OK";
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Thumbs_Down(iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Thumbs_Down(iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Thumbs_Up_CONST:
					{
						string sCMD_Result="OK";
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Thumbs_Up(iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Thumbs_Up(iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Add_Station_CONST:
					{
						string sCMD_Result="OK";
						string sType=pMessage->m_mapParameters[COMMANDPARAMETER_Type_CONST];
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						string sName=pMessage->m_mapParameters[COMMANDPARAMETER_Name_CONST];
						string sID=pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
						CMD_Add_Station(sType.c_str(),iStreamID,sName.c_str(),&sID,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[COMMANDPARAMETER_ID_CONST]=sID;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Add_Station(sType.c_str(),iStreamID,sName.c_str(),&sID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Remove_Station_CONST:
					{
						string sCMD_Result="OK";
						string sID=pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Remove_Station(sID.c_str(),iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Remove_Station(sID.c_str(),iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Rename_Station_CONST:
					{
						string sCMD_Result="OK";
						string sID=pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						string sName=pMessage->m_mapParameters[COMMANDPARAMETER_Name_CONST];
						CMD_Rename_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Rename_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Explain_Song_CONST:
					{
						string sCMD_Result="OK";
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Explain_Song(iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Explain_Song(iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Add_Music_to_Station_CONST:
					{
						string sCMD_Result="OK";
						string sID=pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						string sName=pMessage->m_mapParameters[COMMANDPARAMETER_Name_CONST];
						CMD_Add_Music_to_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Add_Music_to_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Add_Station_to_QuickMix_CONST:
					{
						string sCMD_Result="OK";
						string sID=pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Add_Station_to_QuickMix(sID.c_str(),iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Add_Station_to_QuickMix(sID.c_str(),iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Remove_Station_from_QuickMix_CONST:
					{
						string sCMD_Result="OK";
						string sID=pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
						int iStreamID=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());
						CMD_Remove_Station_from_QuickMix(sID.c_str(),iStreamID,sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Remove_Station_from_QuickMix(sID.c_str(),iStreamID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				}
				iHandled += (Command_Impl::ReceivedMessage(pMessage)==rmr_NotProcessed ? 0 : 1);
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
				{
					// Only buffer single messages, otherwise the caller won't know which messages were buffered and which weren't
					if( m_pMessageBuffer && pMessage->m_bCanBuffer && pMessageOriginal->m_vectExtraMessages.size()==1 && m_pMessageBuffer->BufferMessage(pMessage) )
						return rmr_Buffered;
					ReceivedCommandForChild(pDeviceData_Impl,sCMD_Result,pMessage);
				}
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
			(pMessage->m_eExpectedResponse==ER_ReplyMessage || pMessage->m_eExpectedResponse==ER_ReplyString || pMessage->m_eExpectedResponse==ER_DeliveryConfirmation) )
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
