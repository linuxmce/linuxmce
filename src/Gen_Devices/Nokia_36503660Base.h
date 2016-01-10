/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef Nokia_36503660Base_h
#define Nokia_36503660Base_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Nokia_36503660_Event : public Event_Impl
{
public:
	Nokia_36503660_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID,24, ServerAddress, bConnectEventHandler) {};
	Nokia_36503660_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice );
};


//   OUR DATA CLASS 

class Nokia_36503660_Data : public DeviceData_Impl
{
public:
	virtual ~Nokia_36503660_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 24; } ;
	virtual const char *GetDeviceDescription() { return "Nokia_36503660"; } ;
	int Get_PK_Users() { return atoi(m_mapParameters[3].c_str());}
	int Get_PK_Distro() { return atoi(m_mapParameters[7].c_str());}
	bool Get_Development() { return (m_mapParameters[8]=="1" ? true : false);}
	bool Get_No_Effects() { return (m_mapParameters[20]=="1" ? true : false);}
	int Get_PK_Skin() { return atoi(m_mapParameters[24].c_str());}
	int Get_PK_Size() { return atoi(m_mapParameters[25].c_str());}
	int Get_PK_Language() { return atoi(m_mapParameters[26].c_str());}
	string Get_FK_EntertainArea() { return m_mapParameters[27];}
	string Get_Timeout() { return m_mapParameters[56];}
	string Get_CacheFolder() { return m_mapParameters[57];}
	int Get_CacheSize() { return atoi(m_mapParameters[58].c_str());}
	bool Get_Use_OCG_Format() { return (m_mapParameters[64]=="1" ? true : false);}
	int Get_VideoFrameInterval() { return atoi(m_mapParameters[72].c_str());}
	int Get_ImageQuality() { return atoi(m_mapParameters[75].c_str());}
	string Get_Ignore_State() { return m_mapParameters[87];}
};



//   OUR COMMAND CLASS 

class Nokia_36503660_Command : public Command_Impl
{
public:
	Nokia_36503660_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Nokia_36503660_Event(DeviceID, ServerAddress);
		if( m_pEvent->m_dwPK_Device )
			m_dwPK_Device = m_pEvent->m_dwPK_Device;
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Nokia_36503660_Data();
		if( Size )
			m_pData->SerializeRead(Size,pConfig);
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete[] pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, 24,ServerAddress);
	};
	Nokia_36503660_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Nokia_36503660_Command() {};
	Nokia_36503660_Event *GetEvents() { return (Nokia_36503660_Event *) m_pEvent; };
	Nokia_36503660_Data *GetData() { return (Nokia_36503660_Data *) m_pData; };
	const char *GetClassName() { return "Nokia_36503660_Command"; };
	virtual int PK_DeviceTemplate_get() { return 24; };
	static int PK_DeviceTemplate_get_static() { return 24; };
	virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	int DATA_Get_PK_Users() { return GetData()->Get_PK_Users(); }
	int DATA_Get_PK_Distro() { return GetData()->Get_PK_Distro(); }
	bool DATA_Get_Development() { return GetData()->Get_Development(); }
	bool DATA_Get_No_Effects() { return GetData()->Get_No_Effects(); }
	int DATA_Get_PK_Skin() { return GetData()->Get_PK_Skin(); }
	int DATA_Get_PK_Size() { return GetData()->Get_PK_Size(); }
	int DATA_Get_PK_Language() { return GetData()->Get_PK_Language(); }
	string DATA_Get_FK_EntertainArea() { return GetData()->Get_FK_EntertainArea(); }
	string DATA_Get_Timeout() { return GetData()->Get_Timeout(); }
	string DATA_Get_CacheFolder() { return GetData()->Get_CacheFolder(); }
	int DATA_Get_CacheSize() { return GetData()->Get_CacheSize(); }
	bool DATA_Get_Use_OCG_Format() { return GetData()->Get_Use_OCG_Format(); }
	int DATA_Get_VideoFrameInterval() { return GetData()->Get_VideoFrameInterval(); }
	int DATA_Get_ImageQuality() { return GetData()->Get_ImageQuality(); }
	string DATA_Get_Ignore_State() { return GetData()->Get_Ignore_State(); }
	//Event accessors
	//Commands - Override these to handle commands from the server
	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Orbiter_Beep(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Display_OnOff(string sOnOff,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Goto_Screen(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Terminate_Orbiter(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Remove_Screen_From_History(string sPK_DesignObj,string sID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Regen_Screen(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Requires_Special_Handling(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Bound_Icon(string sValue_To_Assign,string sType,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Stop_Sound(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Store_Variables(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Reset_Highlight(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Continuous_Refresh(string sTime,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Now_Playing(string sValue_To_Assign,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Clear_Selected_Devices(string sPK_DesignObj,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Main_Menu(string sText,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Quit(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Timeout(string sPK_DesignObj,string sTime,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Keep_Screen_On(string sOnOff,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Activate_Window(string sName,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Send_Message(string sText,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual ReceivedMessageResult ReceivedMessage(class Message *pMessageOriginal)
	{
		map<long, string>::iterator itRepeat;
		if( Command_Impl::ReceivedMessage(pMessageOriginal)==rmr_Processed )
			return true;
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
			if (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND)
			{
				switch(pMessage->m_dwID)
				{
				case 1:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPK_Variable=atoi(pMessage->m_mapParameters[4].c_str());
					string sOnOff=pMessage->m_mapParameters[8];
					string sType=pMessage->m_mapParameters[14];
					string sReset=pMessage->m_mapParameters[24];
					int iPK_Text=atoi(pMessage->m_mapParameters[25].c_str());
					bool bDataGrid=(pMessage->m_mapParameters[55]=="1" ? true : false);
						CMD_Capture_Keyboard_To_Variable(sPK_DesignObj.c_str(),iPK_Variable,sOnOff.c_str(),sType.c_str(),sReset.c_str(),iPK_Text,bDataGrid,sCMD_Result,pMessage);
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
								CMD_Capture_Keyboard_To_Variable(sPK_DesignObj.c_str(),iPK_Variable,sOnOff.c_str(),sType.c_str(),sReset.c_str(),iPK_Text,bDataGrid,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 2:
					{
						string sCMD_Result="OK";
						CMD_Orbiter_Beep(sCMD_Result,pMessage);
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
								CMD_Orbiter_Beep(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 3:
					{
						string sCMD_Result="OK";
					string sOnOff=pMessage->m_mapParameters[8];
						CMD_Display_OnOff(sOnOff.c_str(),sCMD_Result,pMessage);
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
								CMD_Display_OnOff(sOnOff.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 4:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj_CurrentScreen=pMessage->m_mapParameters[16];
					string sForce=pMessage->m_mapParameters[21];
						CMD_Go_back(sPK_DesignObj_CurrentScreen.c_str(),sForce.c_str(),sCMD_Result,pMessage);
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
								CMD_Go_back(sPK_DesignObj_CurrentScreen.c_str(),sForce.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 5:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sID=pMessage->m_mapParameters[10];
					string sPK_DesignObj_CurrentScreen=pMessage->m_mapParameters[16];
					bool bStore_Variables=(pMessage->m_mapParameters[22]=="1" ? true : false);
					bool bCant_Go_Back=(pMessage->m_mapParameters[114]=="1" ? true : false);
						CMD_Goto_Screen(iPK_Device,sPK_DesignObj.c_str(),sID.c_str(),sPK_DesignObj_CurrentScreen.c_str(),bStore_Variables,bCant_Go_Back,sCMD_Result,pMessage);
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
								CMD_Goto_Screen(iPK_Device,sPK_DesignObj.c_str(),sID.c_str(),sPK_DesignObj_CurrentScreen.c_str(),bStore_Variables,bCant_Go_Back,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 6:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPK_Variable=atoi(pMessage->m_mapParameters[4].c_str());
					string sComparisson_Operator=pMessage->m_mapParameters[6];
					string sComparisson_Value=pMessage->m_mapParameters[7];
					string sOnOff=pMessage->m_mapParameters[8];
						CMD_Show_Object(sPK_DesignObj.c_str(),iPK_Variable,sComparisson_Operator.c_str(),sComparisson_Value.c_str(),sOnOff.c_str(),sCMD_Result,pMessage);
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
								CMD_Show_Object(sPK_DesignObj.c_str(),iPK_Variable,sComparisson_Operator.c_str(),sComparisson_Value.c_str(),sOnOff.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 7:
					{
						string sCMD_Result="OK";
						CMD_Terminate_Orbiter(sCMD_Result,pMessage);
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
								CMD_Terminate_Orbiter(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 8:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sID=pMessage->m_mapParameters[10];
						CMD_Remove_Screen_From_History(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,pMessage);
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
								CMD_Remove_Screen_From_History(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 9:
					{
						string sCMD_Result="OK";
					string sRelative_Level=pMessage->m_mapParameters[1];
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPK_Direction=atoi(pMessage->m_mapParameters[30].c_str());
						CMD_Scroll_Grid(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,pMessage);
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
								CMD_Scroll_Grid(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 10:
					{
						string sCMD_Result="OK";
					string sRelative_Level=pMessage->m_mapParameters[1];
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPK_Direction=atoi(pMessage->m_mapParameters[30].c_str());
						CMD_Move_Highlight(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,pMessage);
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
								CMD_Move_Highlight(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 13:
					{
						string sCMD_Result="OK";
					char *pData=pMessage->m_mapData_Parameters[19];
					int iData_Size=pMessage->m_mapData_Lengths[19];
					string sFormat=pMessage->m_mapParameters[20];
						CMD_Play_Sound(pData,iData_Size,sFormat.c_str(),sCMD_Result,pMessage);
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
								CMD_Play_Sound(pData,iData_Size,sFormat.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 14:
					{
						string sCMD_Result="OK";
					string sDataGrid_ID=pMessage->m_mapParameters[15];
						CMD_Refresh(sDataGrid_ID.c_str(),sCMD_Result,pMessage);
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
								CMD_Refresh(sDataGrid_ID.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 15:
					{
						string sCMD_Result="OK";
						CMD_Regen_Screen(sCMD_Result,pMessage);
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
								CMD_Regen_Screen(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 16:
					{
						string sCMD_Result="OK";
						CMD_Requires_Special_Handling(sCMD_Result,pMessage);
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
								CMD_Requires_Special_Handling(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 17:
					{
						string sCMD_Result="OK";
					string sText=pMessage->m_mapParameters[9];
					int iPosition_X=atoi(pMessage->m_mapParameters[11].c_str());
					int iPosition_Y=atoi(pMessage->m_mapParameters[12].c_str());
					string sDataGrid_ID=pMessage->m_mapParameters[15];
						CMD_Seek_Data_Grid(sText.c_str(),iPosition_X,iPosition_Y,sDataGrid_ID.c_str(),sCMD_Result,pMessage);
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
								CMD_Seek_Data_Grid(sText.c_str(),iPosition_X,iPosition_Y,sDataGrid_ID.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 18:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sID=pMessage->m_mapParameters[10];
						CMD_Set_Graphic_To_Display(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Graphic_To_Display(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 20:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sValue_To_Assign=pMessage->m_mapParameters[5];
					int iPK_DesignObjParameter=atoi(pMessage->m_mapParameters[27].c_str());
						CMD_Set_Object_Parameter(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),iPK_DesignObjParameter,sCMD_Result,pMessage);
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
								CMD_Set_Object_Parameter(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),iPK_DesignObjParameter,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 21:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPosition_X=atoi(pMessage->m_mapParameters[11].c_str());
					int iPosition_Y=atoi(pMessage->m_mapParameters[12].c_str());
						CMD_Set_Object_Position(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
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
								CMD_Set_Object_Position(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 22:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPosition_X=atoi(pMessage->m_mapParameters[11].c_str());
					int iPosition_Y=atoi(pMessage->m_mapParameters[12].c_str());
						CMD_Set_Object_Size(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
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
								CMD_Set_Object_Size(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 23:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPosition_X=atoi(pMessage->m_mapParameters[11].c_str());
					int iPosition_Y=atoi(pMessage->m_mapParameters[12].c_str());
						CMD_Set_Pos_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
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
								CMD_Set_Pos_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 24:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					int iPosition_X=atoi(pMessage->m_mapParameters[11].c_str());
					int iPosition_Y=atoi(pMessage->m_mapParameters[12].c_str());
						CMD_Set_Size_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
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
								CMD_Set_Size_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 25:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sText=pMessage->m_mapParameters[9];
					int iPK_Text=atoi(pMessage->m_mapParameters[25].c_str());
						CMD_Set_Text(sPK_DesignObj.c_str(),sText.c_str(),iPK_Text,sCMD_Result,pMessage);
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
								CMD_Set_Text(sPK_DesignObj.c_str(),sText.c_str(),iPK_Text,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 26:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
					string sType=pMessage->m_mapParameters[14];
						CMD_Set_Bound_Icon(sValue_To_Assign.c_str(),sType.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Bound_Icon(sValue_To_Assign.c_str(),sType.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 27:
					{
						string sCMD_Result="OK";
					int iPK_Variable=atoi(pMessage->m_mapParameters[4].c_str());
					string sValue_To_Assign=pMessage->m_mapParameters[5];
						CMD_Set_Variable(iPK_Variable,sValue_To_Assign.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Variable(iPK_Variable,sValue_To_Assign.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 28:
					{
						string sCMD_Result="OK";
					string sPK_Button=pMessage->m_mapParameters[26];
					string sName=pMessage->m_mapParameters[50];
						CMD_Simulate_Keypress(sPK_Button.c_str(),sName.c_str(),sCMD_Result,pMessage);
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
								CMD_Simulate_Keypress(sPK_Button.c_str(),sName.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 29:
					{
						string sCMD_Result="OK";
					int iPosition_X=atoi(pMessage->m_mapParameters[11].c_str());
					int iPosition_Y=atoi(pMessage->m_mapParameters[12].c_str());
						CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,sCMD_Result,pMessage);
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
								CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 30:
					{
						string sCMD_Result="OK";
						CMD_Stop_Sound(sCMD_Result,pMessage);
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
								CMD_Stop_Sound(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 31:
					{
						string sCMD_Result="OK";
						CMD_Store_Variables(sCMD_Result,pMessage);
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
								CMD_Store_Variables(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 32:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sType=pMessage->m_mapParameters[14];
					char *pData=pMessage->m_mapData_Parameters[19];
					int iData_Size=pMessage->m_mapData_Lengths[19];
					string sDisable_Aspect_Lock=pMessage->m_mapParameters[23];
						CMD_Update_Object_Image(sPK_DesignObj.c_str(),sType.c_str(),pData,iData_Size,sDisable_Aspect_Lock.c_str(),sCMD_Result,pMessage);
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
								CMD_Update_Object_Image(sPK_DesignObj.c_str(),sType.c_str(),pData,iData_Size,sDisable_Aspect_Lock.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 58:
					{
						string sCMD_Result="OK";
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
						CMD_Set_Current_User(iPK_Users,sCMD_Result,pMessage);
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
								CMD_Set_Current_User(iPK_Users,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 59:
					{
						string sCMD_Result="OK";
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
						CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 66:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sPK_DesignObj_CurrentScreen=pMessage->m_mapParameters[16];
					string sTime=pMessage->m_mapParameters[102];
						CMD_Select_Object(sPK_DesignObj.c_str(),sPK_DesignObj_CurrentScreen.c_str(),sTime.c_str(),sCMD_Result,pMessage);
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
								CMD_Select_Object(sPK_DesignObj.c_str(),sPK_DesignObj_CurrentScreen.c_str(),sTime.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 72:
					{
						string sCMD_Result="OK";
					string sOnOff=pMessage->m_mapParameters[8];
					bool bFully_release_keyboard=(pMessage->m_mapParameters[54]=="1" ? true : false);
						CMD_Surrender_to_OS(sOnOff.c_str(),bFully_release_keyboard,sCMD_Result,pMessage);
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
								CMD_Surrender_to_OS(sOnOff.c_str(),bFully_release_keyboard,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 77:
					{
						string sCMD_Result="OK";
					int iPK_Room=atoi(pMessage->m_mapParameters[57].c_str());
						CMD_Set_Current_Room(iPK_Room,sCMD_Result,pMessage);
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
								CMD_Set_Current_Room(iPK_Room,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 85:
					{
						string sCMD_Result="OK";
						CMD_Reset_Highlight(sCMD_Result,pMessage);
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
								CMD_Reset_Highlight(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 88:
					{
						string sCMD_Result="OK";
					int iLocationID=atoi(pMessage->m_mapParameters[65].c_str());
						CMD_Set_Current_Location(iLocationID,sCMD_Result,pMessage);
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
								CMD_Set_Current_Location(iLocationID,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 192:
					{
						string sCMD_Result="OK";
					int iPK_Pipe=atoi(pMessage->m_mapParameters[97].c_str());
					string sPK_Device_Pipes=pMessage->m_mapParameters[98];
						CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,pMessage);
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
								CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 193:
					{
						string sCMD_Result="OK";
					int iPK_Pipe=atoi(pMessage->m_mapParameters[97].c_str());
						CMD_Off(iPK_Pipe,sCMD_Result,pMessage);
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
								CMD_Off(iPK_Pipe,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 238:
					{
						string sCMD_Result="OK";
					string sTime=pMessage->m_mapParameters[102];
						CMD_Continuous_Refresh(sTime.c_str(),sCMD_Result,pMessage);
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
								CMD_Continuous_Refresh(sTime.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 242:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
						CMD_Set_Now_Playing(sValue_To_Assign.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Now_Playing(sValue_To_Assign.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 254:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sType=pMessage->m_mapParameters[14];
					bool bChild=(pMessage->m_mapParameters[104]=="1" ? true : false);
						CMD_Bind_Icon(sPK_DesignObj.c_str(),sType.c_str(),bChild,sCMD_Result,pMessage);
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
								CMD_Bind_Icon(sPK_DesignObj.c_str(),sType.c_str(),bChild,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 258:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
						CMD_Clear_Selected_Devices(sPK_DesignObj.c_str(),sCMD_Result,pMessage);
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
								CMD_Clear_Selected_Devices(sPK_DesignObj.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 260:
					{
						string sCMD_Result="OK";
					string sText=pMessage->m_mapParameters[9];
						CMD_Set_Main_Menu(sText.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Main_Menu(sText.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 265:
					{
						string sCMD_Result="OK";
						CMD_Quit(sCMD_Result,pMessage);
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
								CMD_Quit(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 324:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sTime=pMessage->m_mapParameters[102];
						CMD_Set_Timeout(sPK_DesignObj.c_str(),sTime.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Timeout(sPK_DesignObj.c_str(),sTime.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 325:
					{
						string sCMD_Result="OK";
					string sOnOff=pMessage->m_mapParameters[8];
						CMD_Keep_Screen_On(sOnOff.c_str(),sCMD_Result,pMessage);
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
								CMD_Keep_Screen_On(sOnOff.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 330:
					{
						string sCMD_Result="OK";
					string sPK_DesignObj=pMessage->m_mapParameters[3];
						CMD_Set_Mouse_Pointer_Over_Object(sPK_DesignObj.c_str(),sCMD_Result,pMessage);
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
								CMD_Set_Mouse_Pointer_Over_Object(sPK_DesignObj.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 354:
					{
						string sCMD_Result="OK";
					string sOnOff=pMessage->m_mapParameters[8];
						CMD_Show_Mouse_Pointer(sOnOff.c_str(),sCMD_Result,pMessage);
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
								CMD_Show_Mouse_Pointer(sOnOff.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 366:
					{
						string sCMD_Result="OK";
					string sName=pMessage->m_mapParameters[50];
						CMD_Activate_Window(sName.c_str(),sCMD_Result,pMessage);
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
								CMD_Activate_Window(sName.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 389:
					{
						string sCMD_Result="OK";
					string sText=pMessage->m_mapParameters[9];
						CMD_Send_Message(sText.c_str(),sCMD_Result,pMessage);
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
								CMD_Send_Message(sText.c_str(),sCMD_Result,pMessage);
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
