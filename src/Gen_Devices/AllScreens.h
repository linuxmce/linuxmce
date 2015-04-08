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
#ifndef AllScreens_h
#define AllScreens_h
#include "Message.h"
#include "PreformedCommand.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Command.h"

// Orbiter will create an instance of a class derived from ScreenHandler
// It must call ReceivedGotoScreenMessage when it receives such a message
// and it must implement GotoDesignObj, as well as override any
// screens that have to do something, like SCREEN_DisplayMessageToUser

namespace DCE
{
	// Specify how to handle interrupting the user if he's doing something
	enum eInterruption {interuptAlways=0, /* always interupt his activity and change screens */
		interuptNever=1, /* never interupt, only change the screen if the system is idle */
		interuptNoVideo=2, /* will interrupt a web browser but not a movie */
		interuptOnlyAudio=3 /* will not interrupt a web browser, only when audio is playing */
	};

	//  Commands
	class SCREEN_Main : public PreformedCommand
	{
	public:
		SCREEN_Main(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Main_DL : public PreformedCommand
	{
	public:
		SCREEN_Main_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Main_DT : public PreformedCommand
	{
	public:
		SCREEN_Main_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Main_Cat : public PreformedCommand
	{
	public:
		SCREEN_Main_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights : public PreformedCommand
	{
	public:
		SCREEN_Lights(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights_DL : public PreformedCommand
	{
	public:
		SCREEN_Lights_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights_DT : public PreformedCommand
	{
	public:
		SCREEN_Lights_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights_Cat : public PreformedCommand
	{
	public:
		SCREEN_Lights_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media : public PreformedCommand
	{
	public:
		SCREEN_Media(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media_DL : public PreformedCommand
	{
	public:
		SCREEN_Media_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media_DT : public PreformedCommand
	{
	public:
		SCREEN_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media_Cat : public PreformedCommand
	{
	public:
		SCREEN_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate : public PreformedCommand
	{
	public:
		SCREEN_Climate(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate_DL : public PreformedCommand
	{
	public:
		SCREEN_Climate_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate_DT : public PreformedCommand
	{
	public:
		SCREEN_Climate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate_Cat : public PreformedCommand
	{
	public:
		SCREEN_Climate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security : public PreformedCommand
	{
	public:
		SCREEN_Security(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security_DL : public PreformedCommand
	{
	public:
		SCREEN_Security_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security_DT : public PreformedCommand
	{
	public:
		SCREEN_Security_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security_Cat : public PreformedCommand
	{
	public:
		SCREEN_Security_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony : public PreformedCommand
	{
	public:
		SCREEN_Telephony(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony_DL : public PreformedCommand
	{
	public:
		SCREEN_Telephony_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony_DT : public PreformedCommand
	{
	public:
		SCREEN_Telephony_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony_Cat : public PreformedCommand
	{
	public:
		SCREEN_Telephony_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc : public PreformedCommand
	{
	public:
		SCREEN_Misc(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc_DL : public PreformedCommand
	{
	public:
		SCREEN_Misc_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc_DT : public PreformedCommand
	{
	public:
		SCREEN_Misc_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc_Cat : public PreformedCommand
	{
	public:
		SCREEN_Misc_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_UserStatus : public PreformedCommand
	{
	public:
		SCREEN_UserStatus(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UserStatus_DL : public PreformedCommand
	{
	public:
		SCREEN_UserStatus_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UserStatus_DT : public PreformedCommand
	{
	public:
		SCREEN_UserStatus_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UserStatus_Cat : public PreformedCommand
	{
	public:
		SCREEN_UserStatus_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallFavorites : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallFavorites_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallFavorites_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallFavorites_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDialNumber : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDialNumber_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDialNumber_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDialNumber_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPlutoUser : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPlutoUser_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPlutoUser_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPlutoUser_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityPanel : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityPanel_DL : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityPanel_DT : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityPanel_Cat : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityStatusReport : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityStatusReport_DL : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityStatusReport_DT : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SecurityStatusReport_Cat : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SingleCameraViewOnly : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_SingleCameraViewOnly_DL : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_SingleCameraViewOnly_DT : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_SingleCameraViewOnly_Cat : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Intercom : public PreformedCommand
	{
	public:
		SCREEN_Intercom(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Intercom_DL : public PreformedCommand
	{
	public:
		SCREEN_Intercom_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Intercom_DT : public PreformedCommand
	{
	public:
		SCREEN_Intercom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Intercom_Cat : public PreformedCommand
	{
	public:
		SCREEN_Intercom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_QuadViewCameras : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras(long DeviceIDFrom, long DeviceIDTo,
			string sList_PK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_QuadViewCameras_DL : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sList_PK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_QuadViewCameras_DT : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sList_PK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_QuadViewCameras_Cat : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sList_PK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_VideoCallInProgress : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideoCallInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideoCallInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideoCallInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileSave : public PreformedCommand
	{
	public:
		SCREEN_FileSave(long DeviceIDFrom, long DeviceIDTo,
			int iPK_MediaType, int iEK_Disc, string sCaption, string sCommand, bool bAdvanced_options,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				29 /* The type of media being saved, if known */, StringUtils::ltos(iPK_MediaType).c_str(), 131 /* If ripping a disc, the disc to rip */, StringUtils::ltos(iEK_Disc).c_str(), 162 /* Caption */, sCaption.c_str(), 236 /* Command to be executed */, sCommand.c_str(), 237 /* Show advanced options like choose folder or choose drive */, StringUtils::ltos(bAdvanced_options).c_str());
		}
	};

	class SCREEN_FileSave_DL : public PreformedCommand
	{
	public:
		SCREEN_FileSave_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_MediaType, int iEK_Disc, string sCaption, string sCommand, bool bAdvanced_options,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				29 /* The type of media being saved, if known */, StringUtils::ltos(iPK_MediaType).c_str(), 131 /* If ripping a disc, the disc to rip */, StringUtils::ltos(iEK_Disc).c_str(), 162 /* Caption */, sCaption.c_str(), 236 /* Command to be executed */, sCommand.c_str(), 237 /* Show advanced options like choose folder or choose drive */, StringUtils::ltos(bAdvanced_options).c_str());
		}
	};

	class SCREEN_FileSave_DT : public PreformedCommand
	{
	public:
		SCREEN_FileSave_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_MediaType, int iEK_Disc, string sCaption, string sCommand, bool bAdvanced_options,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				29 /* The type of media being saved, if known */, StringUtils::ltos(iPK_MediaType).c_str(), 131 /* If ripping a disc, the disc to rip */, StringUtils::ltos(iEK_Disc).c_str(), 162 /* Caption */, sCaption.c_str(), 236 /* Command to be executed */, sCommand.c_str(), 237 /* Show advanced options like choose folder or choose drive */, StringUtils::ltos(bAdvanced_options).c_str());
		}
	};

	class SCREEN_FileSave_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileSave_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_MediaType, int iEK_Disc, string sCaption, string sCommand, bool bAdvanced_options,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				29 /* The type of media being saved, if known */, StringUtils::ltos(iPK_MediaType).c_str(), 131 /* If ripping a disc, the disc to rip */, StringUtils::ltos(iEK_Disc).c_str(), 162 /* Caption */, sCaption.c_str(), 236 /* Command to be executed */, sCommand.c_str(), 237 /* Show advanced options like choose folder or choose drive */, StringUtils::ltos(bAdvanced_options).c_str());
		}
	};

	class SCREEN_ManagePlaylist : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ManagePlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ManagePlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ManagePlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDevice : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDevice_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDevice_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallDevice_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook2 : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook2_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook2_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallPhonebook2_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallIntercom : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallIntercom_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallIntercom_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakeCallIntercom_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentUser : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentUser_DL : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentUser_DT : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentUser_Cat : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CDTrackCopy : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Users, string sFormat, string sName, int iEK_Disc, int iSlot_Number, int iDriveID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str(), 20 /* Format */, sFormat.c_str(), 50 /* The name to save this disc as */, sName.c_str(), 131 /* EK_Disc */, StringUtils::ltos(iEK_Disc).c_str(), 151 /* Slot */, StringUtils::ltos(iSlot_Number).c_str(), 233 /* The id of the external drive */, StringUtils::ltos(iDriveID).c_str());
		}
	};

	class SCREEN_CDTrackCopy_DL : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Users, string sFormat, string sName, int iEK_Disc, int iSlot_Number, int iDriveID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str(), 20 /* Format */, sFormat.c_str(), 50 /* The name to save this disc as */, sName.c_str(), 131 /* EK_Disc */, StringUtils::ltos(iEK_Disc).c_str(), 151 /* Slot */, StringUtils::ltos(iSlot_Number).c_str(), 233 /* The id of the external drive */, StringUtils::ltos(iDriveID).c_str());
		}
	};

	class SCREEN_CDTrackCopy_DT : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Users, string sFormat, string sName, int iEK_Disc, int iSlot_Number, int iDriveID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str(), 20 /* Format */, sFormat.c_str(), 50 /* The name to save this disc as */, sName.c_str(), 131 /* EK_Disc */, StringUtils::ltos(iEK_Disc).c_str(), 151 /* Slot */, StringUtils::ltos(iSlot_Number).c_str(), 233 /* The id of the external drive */, StringUtils::ltos(iDriveID).c_str());
		}
	};

	class SCREEN_CDTrackCopy_Cat : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Users, string sFormat, string sName, int iEK_Disc, int iSlot_Number, int iDriveID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str(), 20 /* Format */, sFormat.c_str(), 50 /* The name to save this disc as */, sName.c_str(), 131 /* EK_Disc */, StringUtils::ltos(iEK_Disc).c_str(), 151 /* Slot */, StringUtils::ltos(iSlot_Number).c_str(), 233 /* The id of the external drive */, StringUtils::ltos(iDriveID).c_str());
		}
	};

	class SCREEN_CDRemote : public PreformedCommand
	{
	public:
		SCREEN_CDRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CDRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_CDRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CDRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_CDRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CDRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_CDRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Main : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Main(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Main_DL : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Main_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Main_DT : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Main_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Main_Cat : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Main_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferAllUsers : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferAllUsers_DL : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferAllUsers_DT : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferAllUsers_Cat : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Sleeping : public PreformedCommand
	{
	public:
		SCREEN_Sleeping(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Sleeping_DL : public PreformedCommand
	{
	public:
		SCREEN_Sleeping_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Sleeping_DT : public PreformedCommand
	{
	public:
		SCREEN_Sleeping_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Sleeping_Cat : public PreformedCommand
	{
	public:
		SCREEN_Sleeping_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ControllerStatus : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ControllerStatus_DL : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ControllerStatus_DT : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ControllerStatus_Cat : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_OtherFolders : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_OtherFolders_DL : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_OtherFolders_DT : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_OtherFolders_Cat : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ConfirmDelete : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ConfirmDelete_DL : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ConfirmDelete_DT : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ConfirmDelete_Cat : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebURL : public PreformedCommand
	{
	public:
		SCREEN_WebURL(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebURL_DL : public PreformedCommand
	{
	public:
		SCREEN_WebURL_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebURL_DT : public PreformedCommand
	{
	public:
		SCREEN_WebURL_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebURL_Cat : public PreformedCommand
	{
	public:
		SCREEN_WebURL_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Browser : public PreformedCommand
	{
	public:
		SCREEN_Browser(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Browser_DL : public PreformedCommand
	{
	public:
		SCREEN_Browser_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Browser_DT : public PreformedCommand
	{
	public:
		SCREEN_Browser_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Browser_Cat : public PreformedCommand
	{
	public:
		SCREEN_Browser_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ModeChanged : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged(long DeviceIDFrom, long DeviceIDTo,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_ModeChanged_DL : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_ModeChanged_DT : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_ModeChanged_Cat : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_SensorsNotReady : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady(long DeviceIDFrom, long DeviceIDTo,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_SensorsNotReady_DL : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_SensorsNotReady_DT : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_SensorsNotReady_Cat : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_RecordedTVShows : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RecordedTVShows_DL : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RecordedTVShows_DT : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RecordedTVShows_Cat : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DeviceControl : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DeviceControl_DL : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DeviceControl_DT : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DeviceControl_Cat : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Computing : public PreformedCommand
	{
	public:
		SCREEN_Computing(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Computing_DL : public PreformedCommand
	{
	public:
		SCREEN_Computing_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Computing_DT : public PreformedCommand
	{
	public:
		SCREEN_Computing_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Computing_Cat : public PreformedCommand
	{
	public:
		SCREEN_Computing_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowserConsole : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowserConsole_DL : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowserConsole_DT : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowserConsole_Cat : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote2 : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote2(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote2_DL : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote2_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote2_DT : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playstation_Remote2_Cat : public PreformedCommand
	{
	public:
		SCREEN_Playstation_Remote2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI994A_Keyboard : public PreformedCommand
	{
	public:
		SCREEN_TI994A_Keyboard(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI994A_Keyboard_DL : public PreformedCommand
	{
	public:
		SCREEN_TI994A_Keyboard_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI994A_Keyboard_DT : public PreformedCommand
	{
	public:
		SCREEN_TI994A_Keyboard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI994A_Keyboard_Cat : public PreformedCommand
	{
	public:
		SCREEN_TI994A_Keyboard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedOptions : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingCall : public PreformedCommand
	{
	public:
		SCREEN_MakingCall(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingCall_DL : public PreformedCommand
	{
	public:
		SCREEN_MakingCall_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingCall_DT : public PreformedCommand
	{
	public:
		SCREEN_MakingCall_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingCall_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakingCall_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast_DL : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast_DT : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Remote : public PreformedCommand
	{
	public:
		SCREEN_PVR_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVD_Remote : public PreformedCommand
	{
	public:
		SCREEN_DVD_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVD_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_DVD_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVD_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_DVD_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVD_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVD_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenu : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenu_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenu_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenu_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapMedia : public PreformedCommand
	{
	public:
		SCREEN_MapMedia(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapMedia_DL : public PreformedCommand
	{
	public:
		SCREEN_MapMedia_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapMedia_DT : public PreformedCommand
	{
	public:
		SCREEN_MapMedia_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapMedia_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapMedia_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedMediaOptions : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedMediaOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedMediaOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvancedMediaOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PopupMessage : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage(long DeviceIDFrom, long DeviceIDTo,
			string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_PopupMessage_DL : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_PopupMessage_DT : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_PopupMessage_Cat : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 10, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_MusicRemote : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MusicRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MusicRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MusicRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LinPhones : public PreformedCommand
	{
	public:
		SCREEN_LinPhones(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LinPhones_DL : public PreformedCommand
	{
	public:
		SCREEN_LinPhones_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LinPhones_DT : public PreformedCommand
	{
	public:
		SCREEN_LinPhones_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LinPhones_Cat : public PreformedCommand
	{
	public:
		SCREEN_LinPhones_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Orbiters : public PreformedCommand
	{
	public:
		SCREEN_Orbiters(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Orbiters_DL : public PreformedCommand
	{
	public:
		SCREEN_Orbiters_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Orbiters_DT : public PreformedCommand
	{
	public:
		SCREEN_Orbiters_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Orbiters_Cat : public PreformedCommand
	{
	public:
		SCREEN_Orbiters_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Phones : public PreformedCommand
	{
	public:
		SCREEN_Phones(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Phones_DL : public PreformedCommand
	{
	public:
		SCREEN_Phones_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Phones_DT : public PreformedCommand
	{
	public:
		SCREEN_Phones_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Phones_Cat : public PreformedCommand
	{
	public:
		SCREEN_Phones_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDOptions : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AnswertoLinPhones : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AnswertoLinPhones_DL : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AnswertoLinPhones_DT : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AnswertoLinPhones_Cat : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UserPinCode : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UserPinCode_DL : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UserPinCode_DT : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UserPinCode_Cat : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PreviewOptions : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PreviewOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PreviewOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PreviewOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferOutside : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferOutside_DL : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferOutside_DT : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TransferOutside_Cat : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SatelliteCableBox : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SatelliteCableBox_DL : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SatelliteCableBox_DT : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SatelliteCableBox_Cat : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentLocation : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentLocation_DL : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentLocation_DT : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentLocation_Cat : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchInput : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchInput_DL : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchInput_DT : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchInput_Cat : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchResult : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchResult_DL : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchResult_DT : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MediaAttributeSearchResult_Cat : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NewPhoneDetected : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected(long DeviceIDFrom, long DeviceIDTo,
			string sMac_address, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str(), 224 /* The pnp queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPhoneDetected_DL : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sMac_address, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str(), 224 /* The pnp queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPhoneDetected_DT : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sMac_address, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str(), 224 /* The pnp queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPhoneDetected_Cat : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sMac_address, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str(), 224 /* The pnp queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_FirstTime : public PreformedCommand
	{
	public:
		SCREEN_FirstTime(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FirstTime_DL : public PreformedCommand
	{
	public:
		SCREEN_FirstTime_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FirstTime_DT : public PreformedCommand
	{
	public:
		SCREEN_FirstTime_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FirstTime_Cat : public PreformedCommand
	{
	public:
		SCREEN_FirstTime_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_PlayLists : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_PlayLists_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_PlayLists_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_PlayLists_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosRemote : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Pictures_Docs : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Pictures_Docs_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Pictures_Docs_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Pictures_Docs_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TryCodes : public PreformedCommand
	{
	public:
		SCREEN_TryCodes(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TryCodes_DL : public PreformedCommand
	{
	public:
		SCREEN_TryCodes_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TryCodes_DT : public PreformedCommand
	{
	public:
		SCREEN_TryCodes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TryCodes_Cat : public PreformedCommand
	{
	public:
		SCREEN_TryCodes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ScreenSaver : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ScreenSaver_DL : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ScreenSaver_DT : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ScreenSaver_Cat : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Pick_Room_For_Device : public PreformedCommand
	{
	public:
		SCREEN_Pick_Room_For_Device(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device, string sDescription, string ssComments,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK_Device if positive.  If negative, this is a PK_PnpQueue before a device is created */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_Pick_Room_For_Device_DL : public PreformedCommand
	{
	public:
		SCREEN_Pick_Room_For_Device_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device, string sDescription, string ssComments,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK_Device if positive.  If negative, this is a PK_PnpQueue before a device is created */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_Pick_Room_For_Device_DT : public PreformedCommand
	{
	public:
		SCREEN_Pick_Room_For_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device, string sDescription, string ssComments,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK_Device if positive.  If negative, this is a PK_PnpQueue before a device is created */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_Pick_Room_For_Device_Cat : public PreformedCommand
	{
	public:
		SCREEN_Pick_Room_For_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device, string sDescription, string ssComments,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* PK_Device if positive.  If negative, this is a PK_PnpQueue before a device is created */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_Power : public PreformedCommand
	{
	public:
		SCREEN_Power(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Power_DL : public PreformedCommand
	{
	public:
		SCREEN_Power_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Power_DT : public PreformedCommand
	{
	public:
		SCREEN_Power_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Power_Cat : public PreformedCommand
	{
	public:
		SCREEN_Power_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_DVD_Remote : public PreformedCommand
	{
	public:
		SCREEN_Generic_DVD_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_DVD_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Generic_DVD_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_DVD_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Generic_DVD_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_DVD_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Generic_DVD_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewPlaylist : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewPlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewPlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewPlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericDvdMenu : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericDvdMenu_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericDvdMenu_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericDvdMenu_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewAttributes : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewAttributes_DL : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewAttributes_DT : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ViewAttributes_Cat : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PendingTasks : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PendingTasks_DL : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PendingTasks_DT : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PendingTasks_Cat : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AddAttribute : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AddAttribute_DL : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AddAttribute_DT : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AddAttribute_Cat : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RadioRemote : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RadioRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RadioRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RadioRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericCDRemote : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericCDRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericCDRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericCDRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Popup_Container : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Popup_Container_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Popup_Container_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_FileList_Popup_Container_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarks : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarks_DL : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarks_DT : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarks_Cat : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowseBookmarks : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowseBookmarks_DL : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowseBookmarks_DT : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BrowseBookmarks_Cat : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppController : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppController_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppController_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppController_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BookmarksByMediaType : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BookmarksByMediaType_DL : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BookmarksByMediaType_DT : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_BookmarksByMediaType_Cat : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MythTvRemote : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MythTvRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MythTvRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MythTvRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_all_commands : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_all_commands_DL : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_all_commands_DT : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_all_commands_Cat : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Mytv_watching_tv : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Mytv_watching_tv_DL : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Mytv_watching_tv_DT : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Mytv_watching_tv_Cat : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode_DL : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode_DT : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode_Cat : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_playback_recording : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_playback_recording_DL : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_playback_recording_DT : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Myth_playback_recording_Cat : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericKeyboard : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard(long DeviceIDFrom, long DeviceIDTo,
			string sText, string sCommand_Line, string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages (MessageSendFormat) for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_GenericKeyboard_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sText, string sCommand_Line, string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages (MessageSendFormat) for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_GenericKeyboard_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages (MessageSendFormat) for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_GenericKeyboard_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages (MessageSendFormat) for the 4 options, | delimited */, sCommand_Line.c_str(), 163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_PingPong : public PreformedCommand
	{
	public:
		SCREEN_PingPong(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PingPong_DL : public PreformedCommand
	{
	public:
		SCREEN_PingPong_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PingPong_DT : public PreformedCommand
	{
	public:
		SCREEN_PingPong_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PingPong_Cat : public PreformedCommand
	{
	public:
		SCREEN_PingPong_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG1 : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG1_DL : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG1_DT : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG1_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG2 : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG2_DL : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG2_DT : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG2_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG3 : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG3_DL : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG3_DT : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVEPG3_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ShowInfo : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ShowInfo_DL : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ShowInfo_DT : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ShowInfo_Cat : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarksTV : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarksTV_DL : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarksTV_DT : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CreateViewBookmarksTV_Cat : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebSites : public PreformedCommand
	{
	public:
		SCREEN_WebSites(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebSites_DL : public PreformedCommand
	{
	public:
		SCREEN_WebSites_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebSites_DT : public PreformedCommand
	{
	public:
		SCREEN_WebSites_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WebSites_Cat : public PreformedCommand
	{
	public:
		SCREEN_WebSites_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WhatModelMobileOrbiter : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Users, string sMac_address,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* The user to whom the phone belongs */, StringUtils::ltos(iPK_Users).c_str(), 47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_WhatModelMobileOrbiter_DL : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Users, string sMac_address,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* The user to whom the phone belongs */, StringUtils::ltos(iPK_Users).c_str(), 47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_WhatModelMobileOrbiter_DT : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Users, string sMac_address,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* The user to whom the phone belongs */, StringUtils::ltos(iPK_Users).c_str(), 47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_WhatModelMobileOrbiter_Cat : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Users, string sMac_address,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				17 /* The user to whom the phone belongs */, StringUtils::ltos(iPK_Users).c_str(), 47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_Filelist : public PreformedCommand
	{
	public:
		SCREEN_Filelist(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Filelist_DL : public PreformedCommand
	{
	public:
		SCREEN_Filelist_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Filelist_DT : public PreformedCommand
	{
	public:
		SCREEN_Filelist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Filelist_Cat : public PreformedCommand
	{
	public:
		SCREEN_Filelist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDmenu : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDmenu_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDmenu_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDmenu_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Attributes : public PreformedCommand
	{
	public:
		SCREEN_Attributes(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Attributes_DL : public PreformedCommand
	{
	public:
		SCREEN_Attributes_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Attributes_DT : public PreformedCommand
	{
	public:
		SCREEN_Attributes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Attributes_Cat : public PreformedCommand
	{
	public:
		SCREEN_Attributes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDoptions : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDoptions_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDoptions_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDoptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvRemote : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdvRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playlist : public PreformedCommand
	{
	public:
		SCREEN_Playlist(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playlist_DL : public PreformedCommand
	{
	public:
		SCREEN_Playlist_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playlist_DT : public PreformedCommand
	{
	public:
		SCREEN_Playlist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Playlist_Cat : public PreformedCommand
	{
	public:
		SCREEN_Playlist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AudioOptions : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AudioOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AudioOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AudioOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Subtitles : public PreformedCommand
	{
	public:
		SCREEN_Subtitles(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Subtitles_DL : public PreformedCommand
	{
	public:
		SCREEN_Subtitles_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Subtitles_DT : public PreformedCommand
	{
	public:
		SCREEN_Subtitles_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Subtitles_Cat : public PreformedCommand
	{
	public:
		SCREEN_Subtitles_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Angles : public PreformedCommand
	{
	public:
		SCREEN_Angles(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Angles_DL : public PreformedCommand
	{
	public:
		SCREEN_Angles_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Angles_DT : public PreformedCommand
	{
	public:
		SCREEN_Angles_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Angles_Cat : public PreformedCommand
	{
	public:
		SCREEN_Angles_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NewPnpDevice : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPnpDevice_DL : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPnpDevice_DT : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPnpDevice_Cat : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_VDR_Remote : public PreformedCommand
	{
	public:
		SCREEN_VDR_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CdRemote : public PreformedCommand
	{
	public:
		SCREEN_CdRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CdRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_CdRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CdRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_CdRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CdRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_CdRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PowerFile : public PreformedCommand
	{
	public:
		SCREEN_PowerFile(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PowerFile_DL : public PreformedCommand
	{
	public:
		SCREEN_PowerFile_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PowerFile_DT : public PreformedCommand
	{
	public:
		SCREEN_PowerFile_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PowerFile_Cat : public PreformedCommand
	{
	public:
		SCREEN_PowerFile_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DevCallInProgress : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress(long DeviceIDFrom, long DeviceIDTo,
			string sPhoneCallerID, string sPhoneCallID, string sChannel,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				84 /* My Caller ID */, sPhoneCallerID.c_str(), 87 /* My Call ID */, sPhoneCallID.c_str(), 264 /* My Channel ID */, sChannel.c_str());
		}
	};

	class SCREEN_DevCallInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPhoneCallerID, string sPhoneCallID, string sChannel,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				84 /* My Caller ID */, sPhoneCallerID.c_str(), 87 /* My Call ID */, sPhoneCallID.c_str(), 264 /* My Channel ID */, sChannel.c_str());
		}
	};

	class SCREEN_DevCallInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPhoneCallerID, string sPhoneCallID, string sChannel,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				84 /* My Caller ID */, sPhoneCallerID.c_str(), 87 /* My Call ID */, sPhoneCallID.c_str(), 264 /* My Channel ID */, sChannel.c_str());
		}
	};

	class SCREEN_DevCallInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPhoneCallerID, string sPhoneCallID, string sChannel,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				84 /* My Caller ID */, sPhoneCallerID.c_str(), 87 /* My Call ID */, sPhoneCallID.c_str(), 264 /* My Channel ID */, sChannel.c_str());
		}
	};

	class SCREEN_DevIncomingCall : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall(long DeviceIDFrom, long DeviceIDTo,
			string sPhoneNumber, string sSource_Channel, string sDestination_Channel, string sSource_Caller_ID, string sDestination_Caller_ID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				75 /* The phone number of the one calling */, sPhoneNumber.c_str(), 269 /* Source channel  */, sSource_Channel.c_str(), 270 /* Destination channel */, sDestination_Channel.c_str(), 271 /* Source caller id */, sSource_Caller_ID.c_str(), 272 /* Destination caller id */, sDestination_Caller_ID.c_str());
		}
	};

	class SCREEN_DevIncomingCall_DL : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPhoneNumber, string sSource_Channel, string sDestination_Channel, string sSource_Caller_ID, string sDestination_Caller_ID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				75 /* The phone number of the one calling */, sPhoneNumber.c_str(), 269 /* Source channel  */, sSource_Channel.c_str(), 270 /* Destination channel */, sDestination_Channel.c_str(), 271 /* Source caller id */, sSource_Caller_ID.c_str(), 272 /* Destination caller id */, sDestination_Caller_ID.c_str());
		}
	};

	class SCREEN_DevIncomingCall_DT : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPhoneNumber, string sSource_Channel, string sDestination_Channel, string sSource_Caller_ID, string sDestination_Caller_ID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				75 /* The phone number of the one calling */, sPhoneNumber.c_str(), 269 /* Source channel  */, sSource_Channel.c_str(), 270 /* Destination channel */, sDestination_Channel.c_str(), 271 /* Source caller id */, sSource_Caller_ID.c_str(), 272 /* Destination caller id */, sDestination_Caller_ID.c_str());
		}
	};

	class SCREEN_DevIncomingCall_Cat : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPhoneNumber, string sSource_Channel, string sDestination_Channel, string sSource_Caller_ID, string sDestination_Caller_ID,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				75 /* The phone number of the one calling */, sPhoneNumber.c_str(), 269 /* Source channel  */, sSource_Channel.c_str(), 270 /* Destination channel */, sDestination_Channel.c_str(), 271 /* Source caller id */, sSource_Caller_ID.c_str(), 272 /* Destination caller id */, sDestination_Caller_ID.c_str());
		}
	};

	class SCREEN_DevMoveCallsInProgress : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DevMoveCallsInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DevMoveCallsInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DevMoveCallsInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapSecurity : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapSecurity_DL : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapSecurity_DT : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapSecurity_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapLighting : public PreformedCommand
	{
	public:
		SCREEN_MapLighting(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapLighting_DL : public PreformedCommand
	{
	public:
		SCREEN_MapLighting_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapLighting_DT : public PreformedCommand
	{
	public:
		SCREEN_MapLighting_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapLighting_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapLighting_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapClimate : public PreformedCommand
	{
	public:
		SCREEN_MapClimate(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapClimate_DL : public PreformedCommand
	{
	public:
		SCREEN_MapClimate_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapClimate_DT : public PreformedCommand
	{
	public:
		SCREEN_MapClimate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapClimate_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapClimate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapTelecom : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapTelecom_DL : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapTelecom_DT : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_MapTelecom_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NavMode : public PreformedCommand
	{
	public:
		SCREEN_NavMode(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NavMode_DL : public PreformedCommand
	{
	public:
		SCREEN_NavMode_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NavMode_DT : public PreformedCommand
	{
	public:
		SCREEN_NavMode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NavMode_Cat : public PreformedCommand
	{
	public:
		SCREEN_NavMode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_Keyboard : public PreformedCommand
	{
	public:
		SCREEN_TI_Keyboard(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_Keyboard_DL : public PreformedCommand
	{
	public:
		SCREEN_TI_Keyboard_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_Keyboard_DT : public PreformedCommand
	{
	public:
		SCREEN_TI_Keyboard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_Keyboard_Cat : public PreformedCommand
	{
	public:
		SCREEN_TI_Keyboard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help01 : public PreformedCommand
	{
	public:
		SCREEN_Help01(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help01_DL : public PreformedCommand
	{
	public:
		SCREEN_Help01_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help01_DT : public PreformedCommand
	{
	public:
		SCREEN_Help01_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help01_Cat : public PreformedCommand
	{
	public:
		SCREEN_Help01_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help02 : public PreformedCommand
	{
	public:
		SCREEN_Help02(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help02_DL : public PreformedCommand
	{
	public:
		SCREEN_Help02_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help02_DT : public PreformedCommand
	{
	public:
		SCREEN_Help02_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help02_Cat : public PreformedCommand
	{
	public:
		SCREEN_Help02_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help03 : public PreformedCommand
	{
	public:
		SCREEN_Help03(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help03_DL : public PreformedCommand
	{
	public:
		SCREEN_Help03_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help03_DT : public PreformedCommand
	{
	public:
		SCREEN_Help03_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Help03_Cat : public PreformedCommand
	{
	public:
		SCREEN_Help03_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv01 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv01_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv01_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv01_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv02 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv02_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv02_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv02_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv03 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv03_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv03_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv03_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv04 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv04_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv04_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_SetupMythTv04_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen_Options : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Full_Screen_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Full_Screen_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreen : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreen : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideosFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppFullScreen : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_GenericAppFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreen : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreen : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VlcFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Commodore_Remote_Alt : public PreformedCommand
	{
	public:
		SCREEN_Game_Commodore_Remote_Alt(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Commodore_Remote_Alt_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Commodore_Remote_Alt_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Commodore_Remote_Alt_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Commodore_Remote_Alt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Commodore_Remote_Alt_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Commodore_Remote_Alt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuRemote : public PreformedCommand
	{
	public:
		SCREEN_rokuRemote(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_rokuRemote_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_rokuRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_rokuRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuKBD : public PreformedCommand
	{
	public:
		SCREEN_rokuKBD(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuKBD_DL : public PreformedCommand
	{
	public:
		SCREEN_rokuKBD_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuKBD_DT : public PreformedCommand
	{
	public:
		SCREEN_rokuKBD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_rokuKBD_Cat : public PreformedCommand
	{
	public:
		SCREEN_rokuKBD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Atari800KBD : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari800KBD(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Atari800KBD_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari800KBD_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Atari800KBD_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari800KBD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Atari800KBD_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari800KBD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Intellivision : public PreformedCommand
	{
	public:
		SCREEN_Game_Intellivision(long DeviceIDFrom, long DeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Intellivision_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Intellivision_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Intellivision_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Intellivision_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Game_Intellivision_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Intellivision_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popOthers : public PreformedCommand
	{
	public:
		SCREEN_popOthers(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_popOthers_DL : public PreformedCommand
	{
	public:
		SCREEN_popOthers_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_popOthers_DT : public PreformedCommand
	{
	public:
		SCREEN_popOthers_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_popOthers_Cat : public PreformedCommand
	{
	public:
		SCREEN_popOthers_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200 : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_Monitor_Mode : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_Monitor_Mode(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_Monitor_Mode_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_Monitor_Mode_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_Monitor_Mode_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_Monitor_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_Monitor_Mode_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_Monitor_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_5200_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_5200_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Menu_Onscreen_NonOSD : public PreformedCommand
	{
	public:
		SCREEN_Game_Menu_Onscreen_NonOSD(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Menu_Onscreen_NonOSD_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Menu_Onscreen_NonOSD_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Menu_Onscreen_NonOSD_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Menu_Onscreen_NonOSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Menu_Onscreen_NonOSD_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Menu_Onscreen_NonOSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_Remote : public PreformedCommand
	{
	public:
		SCREEN_Hulu_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Hulu_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Hulu_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Hulu_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD_DL : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD_DT : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD_Cat : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD_Options : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Hulu_OSD_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Hulu_OSD_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Capture_Card_Audio_Port : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Audio_Port(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Capture_Card_Audio_Port_DL : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Audio_Port_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Capture_Card_Audio_Port_DT : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Audio_Port_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Capture_Card_Audio_Port_Cat : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Audio_Port_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_Remote : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_FS : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Apple_2_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_Apple_2_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_Remote : public PreformedCommand
	{
	public:
		SCREEN_Commodore_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Commodore_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Commodore_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Commodore_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes : public PreformedCommand
	{
	public:
		SCREEN_AirTunes(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_DL : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_DT : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_Cat : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AirTunes_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_AirTunes_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Commodore_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Commodore_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CleanScreen : public PreformedCommand
	{
	public:
		SCREEN_CleanScreen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CleanScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_CleanScreen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CleanScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_CleanScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CleanScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_CleanScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideoWizard : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideoWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideoWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VideoWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogCannotPlayMedia : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia(long DeviceIDFrom, long DeviceIDTo,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotPlayMedia_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotPlayMedia_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotPlayMedia_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress(long DeviceIDFrom, long DeviceIDTo,
			string sPK_Device_From, string sPK_Device_Ripping,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPK_Device_From, string sPK_Device_Ripping,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPK_Device_From, string sPK_Device_Ripping,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPK_Device_From, string sPK_Device_Ripping,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogCheckingDrive : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogCheckingDrive_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogCheckingDrive_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogCheckingDrive_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogPlaylistSaved : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogPlaylistSaved_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogPlaylistSaved_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogPlaylistSaved_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogRippingError : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sTimeout,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingError_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sTimeout,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingError_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sTimeout,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingError_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sTimeout,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingInstructions : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogRippingInstructions_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogRippingInstructions_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogRippingInstructions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_DialogGenericError : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericError_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericError_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericError_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark(long DeviceIDFrom, long DeviceIDTo,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sErrors,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogAskToResume : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume(long DeviceIDFrom, long DeviceIDTo,
			string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 11, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str(), 226 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen_GoTo).c_str());
		}
	};

	class SCREEN_DialogAskToResume_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 11, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str(), 226 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen_GoTo).c_str());
		}
	};

	class SCREEN_DialogAskToResume_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 11, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str(), 226 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen_GoTo).c_str());
		}
	};

	class SCREEN_DialogAskToResume_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 11, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str(), 226 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen_GoTo).c_str());
		}
	};

	class SCREEN_DialogGC100Error : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGC100Error_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGC100Error_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGC100Error_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sPhoneName,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sPhoneName,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sPhoneName,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sPhoneName,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed(long DeviceIDFrom, long DeviceIDTo,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box_DL : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box_DT : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box_Cat : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RoomsWizard : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RoomsWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RoomsWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_RoomsWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UsersWizard : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UsersWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UsersWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_UsersWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CountryWizard : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CountryWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CountryWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CountryWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Manufacturer : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Manufacturer_DL : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Manufacturer_DT : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Manufacturer_Cat : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Receiver : public PreformedCommand
	{
	public:
		SCREEN_Receiver(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Receiver_DL : public PreformedCommand
	{
	public:
		SCREEN_Receiver_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Receiver_DT : public PreformedCommand
	{
	public:
		SCREEN_Receiver_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Receiver_Cat : public PreformedCommand
	{
	public:
		SCREEN_Receiver_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LightsSetup : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LightsSetup_DL : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LightsSetup_DT : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_LightsSetup_Cat : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AlarmPanel : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AlarmPanel_DL : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AlarmPanel_DT : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AlarmPanel_Cat : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VOIP_Provider : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VOIP_Provider_DL : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VOIP_Provider_DT : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VOIP_Provider_Cat : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AV_Devices : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AV_Devices_DL : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AV_Devices_DT : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AV_Devices_Cat : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NAS_Options : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options(long DeviceIDFrom, long DeviceIDTo,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NAS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NAS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NAS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Need_Reload_Router : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Need_Reload_Router_DL : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Need_Reload_Router_DT : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Need_Reload_Router_Cat : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Need_Regen_Orbiter : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Need_Regen_Orbiter_DL : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Need_Regen_Orbiter_DT : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Need_Regen_Orbiter_Cat : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera(long DeviceIDFrom, long DeviceIDTo,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* 1=Sensors only, 2=Lights only, 3=both */, sOptions.c_str(), 224 /* The pnp queue id */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera_DL : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* 1=Sensors only, 2=Lights only, 3=both */, sOptions.c_str(), 224 /* The pnp queue id */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera_DT : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* 1=Sensors only, 2=Lights only, 3=both */, sOptions.c_str(), 224 /* The pnp queue id */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera_Cat : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* 1=Sensors only, 2=Lights only, 3=both */, sOptions.c_str(), 224 /* The pnp queue id */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_PVR_OSD : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_OSD_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_OSD_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_OSD_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Demo_Media : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Demo_Media_DL : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Demo_Media_DT : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Demo_Media_Cat : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Download_are_ready_to_install : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_Download_are_ready_to_install_DL : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_Download_are_ready_to_install_DT : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_Download_are_ready_to_install_Cat : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device, string sPK_Device_AppServer,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_mnuAmbiance : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuAmbiance_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuAmbiance_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuAmbiance_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVConfirmInputsToggle : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVConfirmInputsToggle_DL : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVConfirmInputsToggle_DT : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TVConfirmInputsToggle_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_OSD_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD_Full_Screen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_OSD_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_OSD_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_OSD_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_OSD_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentlyActiveRemote : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentlyActiveRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentlyActiveRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CurrentlyActiveRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuLights : public PreformedCommand
	{
	public:
		SCREEN_mnuLights(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuLights_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuLights_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuLights_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuLights_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuLights_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuLights_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuPlaybackControl : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuPlaybackControl_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuPlaybackControl_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuPlaybackControl_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuSpeedControl : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuSpeedControl_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuSpeedControl_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuSpeedControl_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuVolume : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuVolume_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuVolume_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_mnuVolume_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Media_Tracks : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Media_Tracks_DL : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Media_Tracks_DT : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Media_Tracks_Cat : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Which_Wizard : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Which_Wizard_DL : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Which_Wizard_DT : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Which_Wizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_This_Room : public PreformedCommand
	{
	public:
		SCREEN_This_Room(long DeviceIDFrom, long DeviceIDTo,
			bool bAlways,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "232" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				225 /* if true, always prompt for a new room even if a valid one is already there */, StringUtils::ltos(bAlways).c_str());
		}
	};

	class SCREEN_This_Room_DL : public PreformedCommand
	{
	public:
		SCREEN_This_Room_DL(long DeviceIDFrom, string sDeviceIDTo,
			bool bAlways,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "232" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				225 /* if true, always prompt for a new room even if a valid one is already there */, StringUtils::ltos(bAlways).c_str());
		}
	};

	class SCREEN_This_Room_DT : public PreformedCommand
	{
	public:
		SCREEN_This_Room_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			bool bAlways,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "232" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				225 /* if true, always prompt for a new room even if a valid one is already there */, StringUtils::ltos(bAlways).c_str());
		}
	};

	class SCREEN_This_Room_Cat : public PreformedCommand
	{
	public:
		SCREEN_This_Room_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			bool bAlways,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "232" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				225 /* if true, always prompt for a new room even if a valid one is already there */, StringUtils::ltos(bAlways).c_str());
		}
	};

	class SCREEN_NAS_Options_when_Mounting_device : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_when_Mounting_device(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "234" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NAS_Options_when_Mounting_device_DL : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_when_Mounting_device_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "234" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NAS_Options_when_Mounting_device_DT : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_when_Mounting_device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "234" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NAS_Options_when_Mounting_device_Cat : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_when_Mounting_device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "234" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_New_Pnp_Device_One_Possibility : public PreformedCommand
	{
	public:
		SCREEN_New_Pnp_Device_One_Possibility(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "235" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				57 /* The default room.  If this is not 0, give the user the option of confirming/changing this room */, StringUtils::ltos(iPK_Room).c_str(), 150 /* The DHCP device ID */, StringUtils::ltos(iPK_DHCPDevice).c_str(), 163 /* What to call this for the user */, sDescription.c_str(), 164 /* A description for the room */, ssComments.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_New_Pnp_Device_One_Possibility_DL : public PreformedCommand
	{
	public:
		SCREEN_New_Pnp_Device_One_Possibility_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "235" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				57 /* The default room.  If this is not 0, give the user the option of confirming/changing this room */, StringUtils::ltos(iPK_Room).c_str(), 150 /* The DHCP device ID */, StringUtils::ltos(iPK_DHCPDevice).c_str(), 163 /* What to call this for the user */, sDescription.c_str(), 164 /* A description for the room */, ssComments.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_New_Pnp_Device_One_Possibility_DT : public PreformedCommand
	{
	public:
		SCREEN_New_Pnp_Device_One_Possibility_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "235" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				57 /* The default room.  If this is not 0, give the user the option of confirming/changing this room */, StringUtils::ltos(iPK_Room).c_str(), 150 /* The DHCP device ID */, StringUtils::ltos(iPK_DHCPDevice).c_str(), 163 /* What to call this for the user */, sDescription.c_str(), 164 /* A description for the room */, ssComments.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_New_Pnp_Device_One_Possibility_Cat : public PreformedCommand
	{
	public:
		SCREEN_New_Pnp_Device_One_Possibility_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 9, 
				COMMANDPARAMETER_PK_Screen_CONST, "235" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				57 /* The default room.  If this is not 0, give the user the option of confirming/changing this room */, StringUtils::ltos(iPK_Room).c_str(), 150 /* The DHCP device ID */, StringUtils::ltos(iPK_DHCPDevice).c_str(), 163 /* What to call this for the user */, sDescription.c_str(), 164 /* A description for the room */, ssComments.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Wizard_Done : public PreformedCommand
	{
	public:
		SCREEN_Wizard_Done(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "236" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Wizard_Done_DL : public PreformedCommand
	{
	public:
		SCREEN_Wizard_Done_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "236" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Wizard_Done_DT : public PreformedCommand
	{
	public:
		SCREEN_Wizard_Done_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "236" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Wizard_Done_Cat : public PreformedCommand
	{
	public:
		SCREEN_Wizard_Done_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "236" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_House_Setup_Popup_Message : public PreformedCommand
	{
	public:
		SCREEN_House_Setup_Popup_Message(long DeviceIDFrom, long DeviceIDTo,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "237" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_House_Setup_Popup_Message_DL : public PreformedCommand
	{
	public:
		SCREEN_House_Setup_Popup_Message_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "237" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_House_Setup_Popup_Message_DT : public PreformedCommand
	{
	public:
		SCREEN_House_Setup_Popup_Message_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "237" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_House_Setup_Popup_Message_Cat : public PreformedCommand
	{
	public:
		SCREEN_House_Setup_Popup_Message_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "237" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_Media_Player_Setup_Popup_Message : public PreformedCommand
	{
	public:
		SCREEN_Media_Player_Setup_Popup_Message(long DeviceIDFrom, long DeviceIDTo,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "238" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_Media_Player_Setup_Popup_Message_DL : public PreformedCommand
	{
	public:
		SCREEN_Media_Player_Setup_Popup_Message_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "238" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_Media_Player_Setup_Popup_Message_DT : public PreformedCommand
	{
	public:
		SCREEN_Media_Player_Setup_Popup_Message_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "238" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_Media_Player_Setup_Popup_Message_Cat : public PreformedCommand
	{
	public:
		SCREEN_Media_Player_Setup_Popup_Message_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sText, string sCommand_Line, string sCannotGoBack,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "238" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				9 /* The main message followed by up to 4 messages, all | delimited */, sText.c_str(), 137 /* Up to 4 messages for the 4 options, | delimited */, sCommand_Line.c_str(), 183 /* Cant go back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_Add_Software : public PreformedCommand
	{
	public:
		SCREEN_Add_Software(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "239" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Add_Software_DL : public PreformedCommand
	{
	public:
		SCREEN_Add_Software_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "239" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Add_Software_DT : public PreformedCommand
	{
	public:
		SCREEN_Add_Software_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "239" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Add_Software_Cat : public PreformedCommand
	{
	public:
		SCREEN_Add_Software_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "239" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Folder : public PreformedCommand
	{
	public:
		SCREEN_Choose_Folder(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "240" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Folder_DL : public PreformedCommand
	{
	public:
		SCREEN_Choose_Folder_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "240" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Folder_DT : public PreformedCommand
	{
	public:
		SCREEN_Choose_Folder_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "240" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Folder_Cat : public PreformedCommand
	{
	public:
		SCREEN_Choose_Folder_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "240" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Drive : public PreformedCommand
	{
	public:
		SCREEN_Choose_Drive(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "241" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Drive_DL : public PreformedCommand
	{
	public:
		SCREEN_Choose_Drive_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "241" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Drive_DT : public PreformedCommand
	{
	public:
		SCREEN_Choose_Drive_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "241" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Drive_Cat : public PreformedCommand
	{
	public:
		SCREEN_Choose_Drive_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "241" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Channels : public PreformedCommand
	{
	public:
		SCREEN_TV_Channels(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "242" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Channels_DL : public PreformedCommand
	{
	public:
		SCREEN_TV_Channels_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "242" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Channels_DT : public PreformedCommand
	{
	public:
		SCREEN_TV_Channels_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "242" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TV_Channels_Cat : public PreformedCommand
	{
	public:
		SCREEN_TV_Channels_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "242" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CD_Full_Screen_OSD : public PreformedCommand
	{
	public:
		SCREEN_CD_Full_Screen_OSD(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "243" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CD_Full_Screen_OSD_DL : public PreformedCommand
	{
	public:
		SCREEN_CD_Full_Screen_OSD_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "243" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CD_Full_Screen_OSD_DT : public PreformedCommand
	{
	public:
		SCREEN_CD_Full_Screen_OSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "243" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_CD_Full_Screen_OSD_Cat : public PreformedCommand
	{
	public:
		SCREEN_CD_Full_Screen_OSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "243" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Music_Full_Screen_OSD : public PreformedCommand
	{
	public:
		SCREEN_Music_Full_Screen_OSD(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "244" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Music_Full_Screen_OSD_DL : public PreformedCommand
	{
	public:
		SCREEN_Music_Full_Screen_OSD_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "244" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Music_Full_Screen_OSD_DT : public PreformedCommand
	{
	public:
		SCREEN_Music_Full_Screen_OSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "244" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Music_Full_Screen_OSD_Cat : public PreformedCommand
	{
	public:
		SCREEN_Music_Full_Screen_OSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "244" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Provider_for_Device : public PreformedCommand
	{
	public:
		SCREEN_Choose_Provider_for_Device(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "245" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Provider_for_Device_DL : public PreformedCommand
	{
	public:
		SCREEN_Choose_Provider_for_Device_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "245" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Provider_for_Device_DT : public PreformedCommand
	{
	public:
		SCREEN_Choose_Provider_for_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "245" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Choose_Provider_for_Device_Cat : public PreformedCommand
	{
	public:
		SCREEN_Choose_Provider_for_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "245" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Username_Password_For_Devices : public PreformedCommand
	{
	public:
		SCREEN_Get_Username_Password_For_Devices(long DeviceIDFrom, long DeviceIDTo,
			bool bAlready_processed, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "246" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				125 /* If true we already have the username, just get the password */, StringUtils::ltos(bAlready_processed).c_str(), 163 /* The description of the device */, sDescription.c_str(), 224 /* The PNP Queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Get_Username_Password_For_Devices_DL : public PreformedCommand
	{
	public:
		SCREEN_Get_Username_Password_For_Devices_DL(long DeviceIDFrom, string sDeviceIDTo,
			bool bAlready_processed, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "246" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				125 /* If true we already have the username, just get the password */, StringUtils::ltos(bAlready_processed).c_str(), 163 /* The description of the device */, sDescription.c_str(), 224 /* The PNP Queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Get_Username_Password_For_Devices_DT : public PreformedCommand
	{
	public:
		SCREEN_Get_Username_Password_For_Devices_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			bool bAlready_processed, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "246" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				125 /* If true we already have the username, just get the password */, StringUtils::ltos(bAlready_processed).c_str(), 163 /* The description of the device */, sDescription.c_str(), 224 /* The PNP Queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Get_Username_Password_For_Devices_Cat : public PreformedCommand
	{
	public:
		SCREEN_Get_Username_Password_For_Devices_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			bool bAlready_processed, string sDescription, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 7, 
				COMMANDPARAMETER_PK_Screen_CONST, "246" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				125 /* If true we already have the username, just get the password */, StringUtils::ltos(bAlready_processed).c_str(), 163 /* The description of the device */, sDescription.c_str(), 224 /* The PNP Queue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Get_Capture_Card_Port : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Port(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "247" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Capture_Card_Port_DL : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Port_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "247" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Capture_Card_Port_DT : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Port_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "247" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Get_Capture_Card_Port_Cat : public PreformedCommand
	{
	public:
		SCREEN_Get_Capture_Card_Port_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "247" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Floorplan : public PreformedCommand
	{
	public:
		SCREEN_Floorplan(long DeviceIDFrom, long DeviceIDTo,
			string sPK_DesignObj,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "248" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				3 /* The DesignObj for the floorplan */, sPK_DesignObj.c_str());
		}
	};

	class SCREEN_Floorplan_DL : public PreformedCommand
	{
	public:
		SCREEN_Floorplan_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPK_DesignObj,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "248" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				3 /* The DesignObj for the floorplan */, sPK_DesignObj.c_str());
		}
	};

	class SCREEN_Floorplan_DT : public PreformedCommand
	{
	public:
		SCREEN_Floorplan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPK_DesignObj,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "248" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				3 /* The DesignObj for the floorplan */, sPK_DesignObj.c_str());
		}
	};

	class SCREEN_Floorplan_Cat : public PreformedCommand
	{
	public:
		SCREEN_Floorplan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPK_DesignObj,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "248" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				3 /* The DesignObj for the floorplan */, sPK_DesignObj.c_str());
		}
	};

	class SCREEN_Halt_System : public PreformedCommand
	{
	public:
		SCREEN_Halt_System(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "249" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Halt_System_DL : public PreformedCommand
	{
	public:
		SCREEN_Halt_System_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "249" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Halt_System_DT : public PreformedCommand
	{
	public:
		SCREEN_Halt_System_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "249" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Halt_System_Cat : public PreformedCommand
	{
	public:
		SCREEN_Halt_System_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "249" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Software : public PreformedCommand
	{
	public:
		SCREEN_PVR_Software(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "250" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Software_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_Software_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "250" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Software_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_Software_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "250" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Software_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Software_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "250" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Final_House_Setup : public PreformedCommand
	{
	public:
		SCREEN_Final_House_Setup(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "251" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Final_House_Setup_DL : public PreformedCommand
	{
	public:
		SCREEN_Final_House_Setup_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "251" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Final_House_Setup_DT : public PreformedCommand
	{
	public:
		SCREEN_Final_House_Setup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "251" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Final_House_Setup_Cat : public PreformedCommand
	{
	public:
		SCREEN_Final_House_Setup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "251" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "252" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "252" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "252" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "252" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen_Opts : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen_Opts(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "253" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen_Opts_DL : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen_Opts_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "253" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen_Opts_DT : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen_Opts_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "253" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_ExternalDeviceFullScreen_Opts_Cat : public PreformedCommand
	{
	public:
		SCREEN_ExternalDeviceFullScreen_Opts_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "253" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Remote_Assistance : public PreformedCommand
	{
	public:
		SCREEN_Remote_Assistance(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "255" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Remote_Assistance_DL : public PreformedCommand
	{
	public:
		SCREEN_Remote_Assistance_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "255" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Remote_Assistance_DT : public PreformedCommand
	{
	public:
		SCREEN_Remote_Assistance_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "255" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Remote_Assistance_Cat : public PreformedCommand
	{
	public:
		SCREEN_Remote_Assistance_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "255" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_AV_Speed_Control : public PreformedCommand
	{
	public:
		SCREEN_Legacy_AV_Speed_Control(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "256" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_AV_Speed_Control_DL : public PreformedCommand
	{
	public:
		SCREEN_Legacy_AV_Speed_Control_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "256" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_AV_Speed_Control_DT : public PreformedCommand
	{
	public:
		SCREEN_Legacy_AV_Speed_Control_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "256" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_AV_Speed_Control_Cat : public PreformedCommand
	{
	public:
		SCREEN_Legacy_AV_Speed_Control_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "256" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Thumbnail : public PreformedCommand
	{
	public:
		SCREEN_Thumbnail(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "257" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Thumbnail_DL : public PreformedCommand
	{
	public:
		SCREEN_Thumbnail_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "257" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Thumbnail_DT : public PreformedCommand
	{
	public:
		SCREEN_Thumbnail_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "257" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Thumbnail_Cat : public PreformedCommand
	{
	public:
		SCREEN_Thumbnail_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "257" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Scanning_Progress : public PreformedCommand
	{
	public:
		SCREEN_Scanning_Progress(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "258" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Scanning_Progress_DL : public PreformedCommand
	{
	public:
		SCREEN_Scanning_Progress_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "258" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Scanning_Progress_DT : public PreformedCommand
	{
	public:
		SCREEN_Scanning_Progress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "258" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Scanning_Progress_Cat : public PreformedCommand
	{
	public:
		SCREEN_Scanning_Progress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "258" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Manage_Drives : public PreformedCommand
	{
	public:
		SCREEN_Manage_Drives(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "259" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Manage_Drives_DL : public PreformedCommand
	{
	public:
		SCREEN_Manage_Drives_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "259" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Manage_Drives_DT : public PreformedCommand
	{
	public:
		SCREEN_Manage_Drives_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "259" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Manage_Drives_Cat : public PreformedCommand
	{
	public:
		SCREEN_Manage_Drives_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "259" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Jukebox_Manager : public PreformedCommand
	{
	public:
		SCREEN_Jukebox_Manager(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "260" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The Jukebox to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Jukebox_Manager_DL : public PreformedCommand
	{
	public:
		SCREEN_Jukebox_Manager_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "260" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The Jukebox to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Jukebox_Manager_DT : public PreformedCommand
	{
	public:
		SCREEN_Jukebox_Manager_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "260" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The Jukebox to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Jukebox_Manager_Cat : public PreformedCommand
	{
	public:
		SCREEN_Jukebox_Manager_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "260" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The Jukebox to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_NAS_Manager : public PreformedCommand
	{
	public:
		SCREEN_NAS_Manager(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "261" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The NAS to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_NAS_Manager_DL : public PreformedCommand
	{
	public:
		SCREEN_NAS_Manager_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "261" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The NAS to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_NAS_Manager_DT : public PreformedCommand
	{
	public:
		SCREEN_NAS_Manager_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "261" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The NAS to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_NAS_Manager_Cat : public PreformedCommand
	{
	public:
		SCREEN_NAS_Manager_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "261" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				2 /* The NAS to manage */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_AutoConfigure_TV : public PreformedCommand
	{
	public:
		SCREEN_AutoConfigure_TV(long DeviceIDFrom, long DeviceIDTo,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "262" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* The TV Tuner device */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_AutoConfigure_TV_DL : public PreformedCommand
	{
	public:
		SCREEN_AutoConfigure_TV_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "262" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* The TV Tuner device */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_AutoConfigure_TV_DT : public PreformedCommand
	{
	public:
		SCREEN_AutoConfigure_TV_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "262" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* The TV Tuner device */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_AutoConfigure_TV_Cat : public PreformedCommand
	{
	public:
		SCREEN_AutoConfigure_TV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "262" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				224 /* The TV Tuner device */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_AdjustScreenSettings : public PreformedCommand
	{
	public:
		SCREEN_AdjustScreenSettings(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "263" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdjustScreenSettings_DL : public PreformedCommand
	{
	public:
		SCREEN_AdjustScreenSettings_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "263" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdjustScreenSettings_DT : public PreformedCommand
	{
	public:
		SCREEN_AdjustScreenSettings_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "263" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_AdjustScreenSettings_Cat : public PreformedCommand
	{
	public:
		SCREEN_AdjustScreenSettings_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "263" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PNP_Generic_Options : public PreformedCommand
	{
	public:
		SCREEN_PNP_Generic_Options(long DeviceIDFrom, long DeviceIDTo,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "264" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* The format is:
PK_DeviceData\tPK_Text for question\tPK_Text for Answer 1\tValue for answer 1\tPK_Text for Answer 2 ....\n
PK_DeviceData .... */, sOptions.c_str(), 224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_PNP_Generic_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_PNP_Generic_Options_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "264" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* The format is:
PK_DeviceData\tPK_Text for question\tPK_Text for Answer 1\tValue for answer 1\tPK_Text for Answer 2 ....\n
PK_DeviceData .... */, sOptions.c_str(), 224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_PNP_Generic_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_PNP_Generic_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "264" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* The format is:
PK_DeviceData\tPK_Text for question\tPK_Text for Answer 1\tValue for answer 1\tPK_Text for Answer 2 ....\n
PK_DeviceData .... */, sOptions.c_str(), 224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_PNP_Generic_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_PNP_Generic_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sOptions, int iPK_PnpQueue,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "264" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				39 /* The format is:
PK_DeviceData\tPK_Text for question\tPK_Text for Answer 1\tValue for answer 1\tPK_Text for Answer 2 ....\n
PK_DeviceData .... */, sOptions.c_str(), 224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_Zoom__Aspect : public PreformedCommand
	{
	public:
		SCREEN_Zoom__Aspect(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "265" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Zoom__Aspect_DL : public PreformedCommand
	{
	public:
		SCREEN_Zoom__Aspect_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "265" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Zoom__Aspect_DT : public PreformedCommand
	{
	public:
		SCREEN_Zoom__Aspect_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "265" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Zoom__Aspect_Cat : public PreformedCommand
	{
	public:
		SCREEN_Zoom__Aspect_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "265" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box2 : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box2(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "266" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box2_DL : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box2_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "266" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box2_DT : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "266" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Legacy_PVR_Cable_Box2_Cat : public PreformedCommand
	{
	public:
		SCREEN_Legacy_PVR_Cable_Box2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "266" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Active_Calls : public PreformedCommand
	{
	public:
		SCREEN_Active_Calls(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "267" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Active_Calls_DL : public PreformedCommand
	{
	public:
		SCREEN_Active_Calls_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "267" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Active_Calls_DT : public PreformedCommand
	{
	public:
		SCREEN_Active_Calls_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "267" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Active_Calls_Cat : public PreformedCommand
	{
	public:
		SCREEN_Active_Calls_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "267" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Call_Dropped : public PreformedCommand
	{
	public:
		SCREEN_Call_Dropped(long DeviceIDFrom, long DeviceIDTo,
			string sReason,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "268" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				273 /* The reason for dropping the call */, sReason.c_str());
		}
	};

	class SCREEN_Call_Dropped_DL : public PreformedCommand
	{
	public:
		SCREEN_Call_Dropped_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sReason,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "268" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				273 /* The reason for dropping the call */, sReason.c_str());
		}
	};

	class SCREEN_Call_Dropped_DT : public PreformedCommand
	{
	public:
		SCREEN_Call_Dropped_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sReason,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "268" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				273 /* The reason for dropping the call */, sReason.c_str());
		}
	};

	class SCREEN_Call_Dropped_Cat : public PreformedCommand
	{
	public:
		SCREEN_Call_Dropped_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sReason,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "268" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				273 /* The reason for dropping the call */, sReason.c_str());
		}
	};

	class SCREEN_Assisted_Transfer_In_Progress : public PreformedCommand
	{
	public:
		SCREEN_Assisted_Transfer_In_Progress(long DeviceIDFrom, long DeviceIDTo,
			bool bTrueFalse, string sTask,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "269" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				119 /* True if assisted transfered another call. False if assisted transfer to an extension */, StringUtils::ltos(bTrueFalse).c_str(), 257 /* The task id for assisted transfer */, sTask.c_str());
		}
	};

	class SCREEN_Assisted_Transfer_In_Progress_DL : public PreformedCommand
	{
	public:
		SCREEN_Assisted_Transfer_In_Progress_DL(long DeviceIDFrom, string sDeviceIDTo,
			bool bTrueFalse, string sTask,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "269" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				119 /* True if assisted transfered another call. False if assisted transfer to an extension */, StringUtils::ltos(bTrueFalse).c_str(), 257 /* The task id for assisted transfer */, sTask.c_str());
		}
	};

	class SCREEN_Assisted_Transfer_In_Progress_DT : public PreformedCommand
	{
	public:
		SCREEN_Assisted_Transfer_In_Progress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			bool bTrueFalse, string sTask,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "269" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				119 /* True if assisted transfered another call. False if assisted transfer to an extension */, StringUtils::ltos(bTrueFalse).c_str(), 257 /* The task id for assisted transfer */, sTask.c_str());
		}
	};

	class SCREEN_Assisted_Transfer_In_Progress_Cat : public PreformedCommand
	{
	public:
		SCREEN_Assisted_Transfer_In_Progress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			bool bTrueFalse, string sTask,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "269" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				119 /* True if assisted transfered another call. False if assisted transfer to an extension */, StringUtils::ltos(bTrueFalse).c_str(), 257 /* The task id for assisted transfer */, sTask.c_str());
		}
	};

	class SCREEN_Static_IP_settings : public PreformedCommand
	{
	public:
		SCREEN_Static_IP_settings(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "270" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Static_IP_settings_DL : public PreformedCommand
	{
	public:
		SCREEN_Static_IP_settings_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "270" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Static_IP_settings_DT : public PreformedCommand
	{
	public:
		SCREEN_Static_IP_settings_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "270" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Static_IP_settings_Cat : public PreformedCommand
	{
	public:
		SCREEN_Static_IP_settings_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "270" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Network_Settings : public PreformedCommand
	{
	public:
		SCREEN_Network_Settings(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "271" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Network_Settings_DL : public PreformedCommand
	{
	public:
		SCREEN_Network_Settings_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "271" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Network_Settings_DT : public PreformedCommand
	{
	public:
		SCREEN_Network_Settings_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "271" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Network_Settings_Cat : public PreformedCommand
	{
	public:
		SCREEN_Network_Settings_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "271" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen_Options : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "272" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "272" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "272" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_PVR_Recordings_Full_Screen_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Recordings_Full_Screen_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "272" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen_Options : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "273" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "273" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "273" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "273" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "274" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "274" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "274" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "274" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "275" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "275" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "275" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "275" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD_Full_Screen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "276" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "276" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "276" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_OSD_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_OSD_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "276" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen_Options : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "277" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "277" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "277" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "277" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "278" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "278" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "278" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "278" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "279" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_DL : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "279" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_DT : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "279" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_VDR_Recordings_Cat : public PreformedCommand
	{
	public:
		SCREEN_VDR_Recordings_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "279" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Simple_TV_Remote : public PreformedCommand
	{
	public:
		SCREEN_Simple_TV_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "280" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Simple_TV_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Simple_TV_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "280" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Simple_TV_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Simple_TV_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "280" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Simple_TV_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Simple_TV_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "280" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "281" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen_DL : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "281" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen_DT : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "281" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "281" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen_options : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen_options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "282" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen_options_DL : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen_options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "282" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen_options_DT : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen_options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "282" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Non_Pluto_TV_Full_screen_options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Non_Pluto_TV_Full_screen_options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "282" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Cannot_Reload_Router : public PreformedCommand
	{
	public:
		SCREEN_Cannot_Reload_Router(long DeviceIDFrom, long DeviceIDTo,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "283" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_Cannot_Reload_Router_DL : public PreformedCommand
	{
	public:
		SCREEN_Cannot_Reload_Router_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "283" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_Cannot_Reload_Router_DT : public PreformedCommand
	{
	public:
		SCREEN_Cannot_Reload_Router_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "283" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_Cannot_Reload_Router_Cat : public PreformedCommand
	{
	public:
		SCREEN_Cannot_Reload_Router_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "283" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_Game : public PreformedCommand
	{
	public:
		SCREEN_Game(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "284" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "284" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "284" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "284" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_Options : public PreformedCommand
	{
	public:
		SCREEN_Rip_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "285" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Rip_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "285" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Rip_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "285" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Rip_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "285" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_format_options : public PreformedCommand
	{
	public:
		SCREEN_Rip_format_options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "286" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_format_options_DL : public PreformedCommand
	{
	public:
		SCREEN_Rip_format_options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "286" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_format_options_DT : public PreformedCommand
	{
	public:
		SCREEN_Rip_format_options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "286" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Rip_format_options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Rip_format_options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "286" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Privacy_options_for_rip : public PreformedCommand
	{
	public:
		SCREEN_Privacy_options_for_rip(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "287" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Privacy_options_for_rip_DL : public PreformedCommand
	{
	public:
		SCREEN_Privacy_options_for_rip_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "287" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Privacy_options_for_rip_DT : public PreformedCommand
	{
	public:
		SCREEN_Privacy_options_for_rip_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "287" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Privacy_options_for_rip_Cat : public PreformedCommand
	{
	public:
		SCREEN_Privacy_options_for_rip_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "287" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "288" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "288" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "288" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "288" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "289" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "289" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "289" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Screen_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Screen_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "289" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Current_Disc_Contents : public PreformedCommand
	{
	public:
		SCREEN_Current_Disc_Contents(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "290" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Current_Disc_Contents_DL : public PreformedCommand
	{
	public:
		SCREEN_Current_Disc_Contents_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "290" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Current_Disc_Contents_DT : public PreformedCommand
	{
	public:
		SCREEN_Current_Disc_Contents_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "290" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Current_Disc_Contents_Cat : public PreformedCommand
	{
	public:
		SCREEN_Current_Disc_Contents_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "290" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Edit_Attributes : public PreformedCommand
	{
	public:
		SCREEN_Edit_Attributes(long DeviceIDFrom, long DeviceIDTo,
			string sTracks, int iEK_Attribute, int iEK_Disc, int iEK_File,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "291" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				121 /* If a disc is specified, this can be blank for the disc's attributes only, or a comma delimited list of tracks.  Include track 0 for the discs attributes */, sTracks.c_str(), 123 /* If this is specified, it will go straight to the screen letting the user edit the attribute instead of asking him to pick the attribute for the disc/file */, StringUtils::ltos(iEK_Attribute).c_str(), 131 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_Disc).c_str(), 145 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_File).c_str());
		}
	};

	class SCREEN_Edit_Attributes_DL : public PreformedCommand
	{
	public:
		SCREEN_Edit_Attributes_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sTracks, int iEK_Attribute, int iEK_Disc, int iEK_File,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "291" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				121 /* If a disc is specified, this can be blank for the disc's attributes only, or a comma delimited list of tracks.  Include track 0 for the discs attributes */, sTracks.c_str(), 123 /* If this is specified, it will go straight to the screen letting the user edit the attribute instead of asking him to pick the attribute for the disc/file */, StringUtils::ltos(iEK_Attribute).c_str(), 131 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_Disc).c_str(), 145 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_File).c_str());
		}
	};

	class SCREEN_Edit_Attributes_DT : public PreformedCommand
	{
	public:
		SCREEN_Edit_Attributes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sTracks, int iEK_Attribute, int iEK_Disc, int iEK_File,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "291" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				121 /* If a disc is specified, this can be blank for the disc's attributes only, or a comma delimited list of tracks.  Include track 0 for the discs attributes */, sTracks.c_str(), 123 /* If this is specified, it will go straight to the screen letting the user edit the attribute instead of asking him to pick the attribute for the disc/file */, StringUtils::ltos(iEK_Attribute).c_str(), 131 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_Disc).c_str(), 145 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_File).c_str());
		}
	};

	class SCREEN_Edit_Attributes_Cat : public PreformedCommand
	{
	public:
		SCREEN_Edit_Attributes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sTracks, int iEK_Attribute, int iEK_Disc, int iEK_File,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "291" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */,
				121 /* If a disc is specified, this can be blank for the disc's attributes only, or a comma delimited list of tracks.  Include track 0 for the discs attributes */, sTracks.c_str(), 123 /* If this is specified, it will go straight to the screen letting the user edit the attribute instead of asking him to pick the attribute for the disc/file */, StringUtils::ltos(iEK_Attribute).c_str(), 131 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_Disc).c_str(), 145 /* Specify either the disc and optionally tracks, or a file */, StringUtils::ltos(iEK_File).c_str());
		}
	};

	class SCREEN_Game_Monitor_Mode : public PreformedCommand
	{
	public:
		SCREEN_Game_Monitor_Mode(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "292" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Monitor_Mode_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Monitor_Mode_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "292" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Monitor_Mode_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Monitor_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "292" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Monitor_Mode_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Monitor_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "292" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Monitor_Mode : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Monitor_Mode(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "293" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Monitor_Mode_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Monitor_Mode_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "293" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Monitor_Mode_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Monitor_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "293" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Monitor_Mode_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Monitor_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "293" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "294" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "294" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "294" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "294" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600 : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "295" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "295" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "295" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "295" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "296" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "296" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "296" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "296" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Controller_Options : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Controller_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "297" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Controller_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Controller_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "297" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Controller_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Controller_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "297" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_2600_Controller_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_2600_Controller_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "297" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_BluRay_Remote : public PreformedCommand
	{
	public:
		SCREEN_Generic_BluRay_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "298" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_BluRay_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Generic_BluRay_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "298" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_BluRay_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Generic_BluRay_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "298" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Generic_BluRay_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Generic_BluRay_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "298" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "299" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "299" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "299" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "299" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "300" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "300" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "300" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_NonPluto_BluRay_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_NonPluto_BluRay_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "300" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_3_Remote : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_3_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "301" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_3_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_3_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "301" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_3_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_3_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "301" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_3_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_3_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "301" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_2_Remote : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_2_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "302" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_2_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_2_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "302" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_2_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_2_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "302" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Tivo_Series_2_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_Tivo_Series_2_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "302" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Now : public PreformedCommand
	{
	public:
		SCREEN_Weather_Now(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "303" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Now_DL : public PreformedCommand
	{
	public:
		SCREEN_Weather_Now_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "303" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Now_DT : public PreformedCommand
	{
	public:
		SCREEN_Weather_Now_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "303" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Now_Cat : public PreformedCommand
	{
	public:
		SCREEN_Weather_Now_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "303" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Forecast : public PreformedCommand
	{
	public:
		SCREEN_Weather_Forecast(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "304" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Forecast_DL : public PreformedCommand
	{
	public:
		SCREEN_Weather_Forecast_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "304" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Forecast_DT : public PreformedCommand
	{
	public:
		SCREEN_Weather_Forecast_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "304" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Forecast_Cat : public PreformedCommand
	{
	public:
		SCREEN_Weather_Forecast_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "304" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_3day : public PreformedCommand
	{
	public:
		SCREEN_Weather_3day(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "305" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_3day_DL : public PreformedCommand
	{
	public:
		SCREEN_Weather_3day_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "305" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_3day_DT : public PreformedCommand
	{
	public:
		SCREEN_Weather_3day_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "305" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_3day_Cat : public PreformedCommand
	{
	public:
		SCREEN_Weather_3day_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "305" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_6day : public PreformedCommand
	{
	public:
		SCREEN_Weather_6day(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "306" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_6day_DL : public PreformedCommand
	{
	public:
		SCREEN_Weather_6day_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "306" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_6day_DT : public PreformedCommand
	{
	public:
		SCREEN_Weather_6day_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "306" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_6day_Cat : public PreformedCommand
	{
	public:
		SCREEN_Weather_6day_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "306" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Radar : public PreformedCommand
	{
	public:
		SCREEN_Weather_Radar(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "307" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Radar_DL : public PreformedCommand
	{
	public:
		SCREEN_Weather_Radar_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "307" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Radar_DT : public PreformedCommand
	{
	public:
		SCREEN_Weather_Radar_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "307" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Weather_Radar_Cat : public PreformedCommand
	{
	public:
		SCREEN_Weather_Radar_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "307" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WD_HDTVLive_Remote : public PreformedCommand
	{
	public:
		SCREEN_WD_HDTVLive_Remote(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "308" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WD_HDTVLive_Remote_DL : public PreformedCommand
	{
	public:
		SCREEN_WD_HDTVLive_Remote_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "308" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WD_HDTVLive_Remote_DT : public PreformedCommand
	{
	public:
		SCREEN_WD_HDTVLive_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "308" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_WD_HDTVLive_Remote_Cat : public PreformedCommand
	{
	public:
		SCREEN_WD_HDTVLive_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "308" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Num2 : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Num2(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "309" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Num2_DL : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Num2_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "309" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Num2_DT : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Num2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "309" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_XBOX_360_Num2_Cat : public PreformedCommand
	{
	public:
		SCREEN_XBOX_360_Num2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "309" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "310" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS_DL : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "310" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS_DT : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "310" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS_Cat : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "310" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS_Options : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS_Options(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "311" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "311" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "311" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_TI_99_FS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_TI_99_FS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "311" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800 : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "312" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "312" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "312" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "312" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800_Menu_Visible : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800_Menu_Visible(long DeviceIDFrom, long DeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "313" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800_Menu_Visible_DL : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800_Menu_Visible_DL(long DeviceIDFrom, string sDeviceIDTo,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "313" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800_Menu_Visible_DT : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800_Menu_Visible_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "313" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};

	class SCREEN_Game_Atari_800_Menu_Visible_Cat : public PreformedCommand
	{
	public:
		SCREEN_Game_Atari_800_Menu_Visible_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,eInterruption _eInterruption=interuptAlways,bool bTurnOnMonitor=false,bool bQueueIfIgnored=false)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "313" /* screen ID */
				,COMMANDPARAMETER_Turn_On_CONST, bTurnOnMonitor ? "1" : "0" /* turn on monitor */
				,COMMANDPARAMETER_Interruption_CONST, StringUtils::itos(_eInterruption).c_str() /* interruption */
				,COMMANDPARAMETER_Queue_CONST, bQueueIfIgnored ? "1" : "0" /* queue the message if it's ignored */);
		}
	};


	class ScreenHandlerBase
	{
	protected:
		map<int,int> *m_p_MapDesignObj;

	public:
		int m_p_MapDesignObj_Find(int PK_Screen) { map<int,int>::iterator it = m_p_MapDesignObj->find(PK_Screen); return it==m_p_MapDesignObj->end() ? 0 : (*it).second; }
		ScreenHandlerBase(map<int,int> *p_MapDesignObj) { m_p_MapDesignObj=p_MapDesignObj; }
		virtual ~ScreenHandlerBase() {}

		virtual void ResetCallBacks() {}

		virtual void GotoDesignObj(int PK_DesignObj,string sID="",bool bStore_Variables=false,bool bCant_Go_Back=false)=0;
		virtual void GotoScreen(int PK_Screen,string sID="",bool bStore_Variables=false,bool bCant_Go_Back=false)
		{
			int PK_DesignObj = m_p_MapDesignObj_Find(PK_Screen);
			if( PK_DesignObj )
				GotoDesignObj(PK_DesignObj,sID,bStore_Variables,bCant_Go_Back);
			else
				BadGotoScreen(PK_Screen);
		}

		virtual void BadGotoScreen(int PK_Screen) {}

		/*  Display a message for the user */
		virtual void SCREEN_Main(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Lights(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Media(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Climate(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Security(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Telephony(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Misc(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_UserStatus(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallPhonebook(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallFavorites(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallDialNumber(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallPlutoUser(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SecurityPanel(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SecurityStatusReport(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SingleCameraViewOnly(long PK_Screen, int iPK_Device){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Intercom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_QuadViewCameras(long PK_Screen, string sList_PK_Device){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideoCallInProgress(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileSave(long PK_Screen, int iPK_MediaType, int iEK_Disc, string sCaption, string sCommand, bool bAdvanced_options){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ManagePlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallDevice(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallPhonebook2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallIntercom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CurrentUser(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CDTrackCopy(long PK_Screen, int iPK_Users, string sFormat, string sName, int iEK_Disc, int iSlot_Number, int iDriveID){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CDRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_XBOX_360_Main(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TransferAllUsers(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Sleeping(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ControllerStatus(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_OtherFolders(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ConfirmDelete(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_WebURL(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Browser(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SensorsNotReady(long PK_Screen, string sDescription){ GotoScreen(PK_Screen); }
		virtual void SCREEN_RecordedTVShows(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DeviceControl(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Computing(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Playstation_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_BrowserConsole(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Playstation_Remote2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TI994A_Keyboard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AdvancedOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakingCall(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakingLinPhoneBroadcast(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_Music_Movies_Video(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVD_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDMenu(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapMedia(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AdvancedMediaOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PopupMessage(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MusicRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_LinPhones(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Orbiters(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Phones(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AnswertoLinPhones(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_UserPinCode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PreviewOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TransferOutside(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SatelliteCableBox(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CurrentLocation(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MediaAttributeSearchInput(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MediaAttributeSearchResult(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NewPhoneDetected(long PK_Screen, string sMac_address, string sDescription, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FirstTime(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_PlayLists(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideosRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_Pictures_Docs(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericTwoButtonQuestion(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TryCodes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ScreenSaver(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Pick_Room_For_Device(long PK_Screen, int iPK_Device, string sDescription, string ssComments){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Power(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Generic_DVD_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ViewPlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_withPictures_Mobil(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericDvdMenu(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ViewAttributes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PendingTasks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AddAttribute(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_RadioRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericCDRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_Popup_Container(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CreateViewBookmarks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_BrowseBookmarks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericAppController(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_BookmarksByMediaType(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MythTvRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Myth_all_commands(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Mytv_watching_tv(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Myth_live_tv_browse_mode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Myth_playback_recording(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericKeyboard(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PingPong(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVEPG1(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVEPG2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVEPG3(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ShowInfo(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CreateViewBookmarksTV(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_WebSites(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_WhatModelMobileOrbiter(long PK_Screen, int iPK_Users, string sMac_address){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Filelist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDmenu(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Attributes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDoptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AdvRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Playlist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudioOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Subtitles(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Angles(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NewPnpDevice(long PK_Screen, string sDescription, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CdRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PowerFile(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DevCallInProgress(long PK_Screen, string sPhoneCallerID, string sPhoneCallID, string sChannel){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DevIncomingCall(long PK_Screen, string sPhoneNumber, string sSource_Channel, string sDestination_Channel, string sSource_Caller_ID, string sDestination_Caller_ID){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DevMoveCallsInProgress(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapSecurity(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapLighting(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapClimate(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapTelecom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NavMode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TI_Keyboard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Help01(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Help02(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Help03(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv01(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv02(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv03(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv04(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_Full_Screen_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideosFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideosFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericAppFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDMenuFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDMenuFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VlcFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VlcFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Commodore_Remote_Alt(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_rokuRemote(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_rokuKBD(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari800KBD(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Intellivision(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_popOthers(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_5200(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_5200_Monitor_Mode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_5200_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_5200_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_SG1000_SMS_NES_Famicom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_SG1000_SMS_NES_Famicom_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Menu_Onscreen_NonOSD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Hulu_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Hulu_OSD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Hulu_OSD_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Get_Capture_Card_Audio_Port(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Apple_2_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Apple_2_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Commodore_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AirTunes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AirTunes_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AirTunes_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Commodore_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Commodore_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CleanScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideoWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogRippingInProgress(long PK_Screen, string sPK_Device_From, string sPK_Device_Ripping){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogCheckingDrive(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogUnableToSavePlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogPlaylistSaved(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogUnableToLoadPlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogRippingError(long PK_Screen, string sDescription, string sTimeout){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogRippingInstructions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogGenericError(long PK_Screen, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogCannotBookmark(long PK_Screen, string sErrors){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogAskToResume(long PK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogPhoneInstructions(long PK_Screen, string sDescription, string sPhoneName){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Legacy_PVR_Cable_Box(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_RoomsWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_UsersWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CountryWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TV_Manufacturer(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Receiver(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_LightsSetup(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AlarmPanel(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VOIP_Provider(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AV_Devices(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NAS_Options(long PK_Screen, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Need_Reload_Router(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Need_Regen_Orbiter(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Sensors_Viewed_By_Camera(long PK_Screen, string sOptions, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_OSD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_Recordings(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Demo_Media(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Download_are_ready_to_install(long PK_Screen, int iPK_Device, string sPK_Device_AppServer){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuAmbiance(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVConfirmInputsToggle(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_OSD_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_Recordings_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CurrentlyActiveRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuLights(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuPlaybackControl(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuSpeedControl(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuVolume(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Media_Tracks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Which_Wizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_This_Room(long PK_Screen, bool bAlways){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NAS_Options_when_Mounting_device(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_New_Pnp_Device_One_Possibility(long PK_Screen, int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Wizard_Done(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_House_Setup_Popup_Message(long PK_Screen, string sText, string sCommand_Line, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Media_Player_Setup_Popup_Message(long PK_Screen, string sText, string sCommand_Line, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Add_Software(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Choose_Folder(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Choose_Drive(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TV_Channels(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CD_Full_Screen_OSD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Music_Full_Screen_OSD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Choose_Provider_for_Device(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Get_Username_Password_For_Devices(long PK_Screen, bool bAlready_processed, string sDescription, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Get_Capture_Card_Port(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Floorplan(long PK_Screen, string sPK_DesignObj){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Halt_System(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_Software(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Final_House_Setup(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ExternalDeviceFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ExternalDeviceFullScreen_Opts(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Remote_Assistance(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Legacy_AV_Speed_Control(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Thumbnail(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Scanning_Progress(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Manage_Drives(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Jukebox_Manager(long PK_Screen, int iPK_Device){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NAS_Manager(long PK_Screen, int iPK_Device){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AutoConfigure_TV(long PK_Screen, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AdjustScreenSettings(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PNP_Generic_Options(long PK_Screen, string sOptions, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Zoom__Aspect(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Legacy_PVR_Cable_Box2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Active_Calls(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Call_Dropped(long PK_Screen, string sReason){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Assisted_Transfer_In_Progress(long PK_Screen, bool bTrueFalse, string sTask){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Static_IP_settings(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Network_Settings(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR_Recordings_Full_Screen_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_Recordings_Full_Screen_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_OSD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_Recordings_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_OSD_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_Full_Screen_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VDR_Recordings(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Simple_TV_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Non_Pluto_TV_Full_screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Non_Pluto_TV_Full_screen_options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Cannot_Reload_Router(long PK_Screen, string sDescription){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Rip_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Rip_format_options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Privacy_options_for_rip(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Screen_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Screen_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Current_Disc_Contents(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Edit_Attributes(long PK_Screen, string sTracks, int iEK_Attribute, int iEK_Disc, int iEK_File){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Monitor_Mode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_2600_Monitor_Mode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_2600_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_2600(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_2600_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_2600_Controller_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Generic_BluRay_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NonPluto_BluRay_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NonPluto_BluRay_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Tivo_Series_3_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Tivo_Series_2_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Weather_Now(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Weather_Forecast(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Weather_3day(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Weather_6day(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Weather_Radar(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_WD_HDTVLive_Remote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_XBOX_360_Num2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TI_99_FS(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TI_99_FS_Options(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_800(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Game_Atari_800_Menu_Visible(long PK_Screen){ GotoScreen(PK_Screen); }

		virtual void ReceivedGotoScreenMessage(int nPK_Screen, Message *pMessage)
		{
			switch(nPK_Screen) 
			{
				case 1:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Main(nPK_Screen, sLocation);
					break;
				}
				case 2:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Lights(nPK_Screen, sLocation);
					break;
				}
				case 3:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Media(nPK_Screen, sLocation);
					break;
				}
				case 4:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Climate(nPK_Screen, sLocation);
					break;
				}
				case 5:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Security(nPK_Screen, sLocation);
					break;
				}
				case 6:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Telephony(nPK_Screen, sLocation);
					break;
				}
				case 7:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Misc(nPK_Screen, sLocation);
					break;
				}
				case 8:
				{
					ResetCallBacks();
					SCREEN_UserStatus(nPK_Screen);
					break;
				}
				case 9:
				{
					ResetCallBacks();
					SCREEN_MakeCallPhonebook(nPK_Screen);
					break;
				}
				case 10:
				{
					ResetCallBacks();
					SCREEN_MakeCallFavorites(nPK_Screen);
					break;
				}
				case 11:
				{
					ResetCallBacks();
					SCREEN_MakeCallDialNumber(nPK_Screen);
					break;
				}
				case 12:
				{
					ResetCallBacks();
					SCREEN_MakeCallPlutoUser(nPK_Screen);
					break;
				}
				case 13:
				{
					ResetCallBacks();
					SCREEN_SecurityPanel(nPK_Screen);
					break;
				}
				case 14:
				{
					ResetCallBacks();
					SCREEN_SecurityStatusReport(nPK_Screen);
					break;
				}
				case 15:
				{
					ResetCallBacks();
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					SCREEN_SingleCameraViewOnly(nPK_Screen, iPK_Device);
					break;
				}
				case 16:
				{
					ResetCallBacks();
					SCREEN_Intercom(nPK_Screen);
					break;
				}
				case 17:
				{
					ResetCallBacks();
					string sList_PK_Device = pMessage->m_mapParameters[103];
					SCREEN_QuadViewCameras(nPK_Screen, sList_PK_Device);
					break;
				}
				case 18:
				{
					ResetCallBacks();
					SCREEN_VideoCallInProgress(nPK_Screen);
					break;
				}
				case 19:
				{
					ResetCallBacks();
					int iPK_MediaType = atoi(pMessage->m_mapParameters[29].c_str());
					int iEK_Disc = atoi(pMessage->m_mapParameters[131].c_str());
					string sCaption = pMessage->m_mapParameters[162];
					string sCommand = pMessage->m_mapParameters[236];
					bool bAdvanced_options = pMessage->m_mapParameters[237] == "1";
					SCREEN_FileSave(nPK_Screen, iPK_MediaType, iEK_Disc, sCaption, sCommand, bAdvanced_options);
					break;
				}
				case 20:
				{
					ResetCallBacks();
					SCREEN_ManagePlaylist(nPK_Screen);
					break;
				}
				case 21:
				{
					ResetCallBacks();
					SCREEN_MakeCallDevice(nPK_Screen);
					break;
				}
				case 22:
				{
					ResetCallBacks();
					SCREEN_MakeCallPhonebook2(nPK_Screen);
					break;
				}
				case 23:
				{
					ResetCallBacks();
					SCREEN_MakeCallIntercom(nPK_Screen);
					break;
				}
				case 24:
				{
					ResetCallBacks();
					SCREEN_CurrentUser(nPK_Screen);
					break;
				}
				case 25:
				{
					ResetCallBacks();
					int iPK_Users = atoi(pMessage->m_mapParameters[17].c_str());
					string sFormat = pMessage->m_mapParameters[20];
					string sName = pMessage->m_mapParameters[50];
					int iEK_Disc = atoi(pMessage->m_mapParameters[131].c_str());
					int iSlot_Number = atoi(pMessage->m_mapParameters[151].c_str());
					int iDriveID = atoi(pMessage->m_mapParameters[233].c_str());
					SCREEN_CDTrackCopy(nPK_Screen, iPK_Users, sFormat, sName, iEK_Disc, iSlot_Number, iDriveID);
					break;
				}
				case 26:
				{
					ResetCallBacks();
					SCREEN_CDRemote(nPK_Screen);
					break;
				}
				case 27:
				{
					ResetCallBacks();
					SCREEN_XBOX_360_Main(nPK_Screen);
					break;
				}
				case 28:
				{
					ResetCallBacks();
					SCREEN_TransferAllUsers(nPK_Screen);
					break;
				}
				case 29:
				{
					ResetCallBacks();
					SCREEN_Sleeping(nPK_Screen);
					break;
				}
				case 30:
				{
					ResetCallBacks();
					SCREEN_ControllerStatus(nPK_Screen);
					break;
				}
				case 31:
				{
					ResetCallBacks();
					SCREEN_OtherFolders(nPK_Screen);
					break;
				}
				case 32:
				{
					ResetCallBacks();
					SCREEN_ConfirmDelete(nPK_Screen);
					break;
				}
				case 33:
				{
					ResetCallBacks();
					SCREEN_WebURL(nPK_Screen);
					break;
				}
				case 34:
				{
					ResetCallBacks();
					SCREEN_Browser(nPK_Screen);
					break;
				}
				case 35:
				{
					ResetCallBacks();
					string sPK_HouseMode = pMessage->m_mapParameters[165];
					string sHouseModeTime = pMessage->m_mapParameters[166];
					string sExitDelay = pMessage->m_mapParameters[167];
					string sAlerts = pMessage->m_mapParameters[168];
					SCREEN_ModeChanged(nPK_Screen, sPK_HouseMode, sHouseModeTime, sExitDelay, sAlerts);
					break;
				}
				case 36:
				{
					ResetCallBacks();
					string sDescription = pMessage->m_mapParameters[163];
					SCREEN_SensorsNotReady(nPK_Screen, sDescription);
					break;
				}
				case 37:
				{
					ResetCallBacks();
					SCREEN_RecordedTVShows(nPK_Screen);
					break;
				}
				case 38:
				{
					ResetCallBacks();
					SCREEN_DeviceControl(nPK_Screen);
					break;
				}
				case 39:
				{
					ResetCallBacks();
					SCREEN_Computing(nPK_Screen);
					break;
				}
				case 40:
				{
					ResetCallBacks();
					SCREEN_Playstation_Remote(nPK_Screen);
					break;
				}
				case 41:
				{
					ResetCallBacks();
					SCREEN_BrowserConsole(nPK_Screen);
					break;
				}
				case 42:
				{
					ResetCallBacks();
					SCREEN_Playstation_Remote2(nPK_Screen);
					break;
				}
				case 43:
				{
					ResetCallBacks();
					SCREEN_TI994A_Keyboard(nPK_Screen);
					break;
				}
				case 44:
				{
					ResetCallBacks();
					SCREEN_AdvancedOptions(nPK_Screen);
					break;
				}
				case 45:
				{
					ResetCallBacks();
					SCREEN_MakingCall(nPK_Screen);
					break;
				}
				case 46:
				{
					ResetCallBacks();
					SCREEN_MakingLinPhoneBroadcast(nPK_Screen);
					break;
				}
				case 47:
				{
					ResetCallBacks();
					SCREEN_FileList_Music_Movies_Video(nPK_Screen);
					break;
				}
				case 48:
				{
					ResetCallBacks();
					SCREEN_PVR_Remote(nPK_Screen);
					break;
				}
				case 49:
				{
					ResetCallBacks();
					SCREEN_DVD_Remote(nPK_Screen);
					break;
				}
				case 50:
				{
					ResetCallBacks();
					SCREEN_DVDMenu(nPK_Screen);
					break;
				}
				case 51:
				{
					ResetCallBacks();
					SCREEN_MapMedia(nPK_Screen);
					break;
				}
				case 52:
				{
					ResetCallBacks();
					SCREEN_AdvancedMediaOptions(nPK_Screen);
					break;
				}
				case 53:
				{
					string sText = pMessage->m_mapParameters[9];
					string sCommand_Line = pMessage->m_mapParameters[137];
					string sDescription = pMessage->m_mapParameters[163];
					string sPromptToResetRouter = pMessage->m_mapParameters[181];
					string sTimeout = pMessage->m_mapParameters[182];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_PopupMessage(nPK_Screen, sText, sCommand_Line, sDescription, sPromptToResetRouter, sTimeout, sCannotGoBack);
					break;
				}
				case 54:
				{
					ResetCallBacks();
					SCREEN_MusicRemote(nPK_Screen);
					break;
				}
				case 55:
				{
					ResetCallBacks();
					SCREEN_LinPhones(nPK_Screen);
					break;
				}
				case 56:
				{
					ResetCallBacks();
					SCREEN_Orbiters(nPK_Screen);
					break;
				}
				case 57:
				{
					ResetCallBacks();
					SCREEN_Phones(nPK_Screen);
					break;
				}
				case 58:
				{
					ResetCallBacks();
					SCREEN_DVDOptions(nPK_Screen);
					break;
				}
				case 59:
				{
					ResetCallBacks();
					SCREEN_AnswertoLinPhones(nPK_Screen);
					break;
				}
				case 60:
				{
					ResetCallBacks();
					SCREEN_UserPinCode(nPK_Screen);
					break;
				}
				case 61:
				{
					ResetCallBacks();
					SCREEN_PreviewOptions(nPK_Screen);
					break;
				}
				case 62:
				{
					ResetCallBacks();
					SCREEN_TransferOutside(nPK_Screen);
					break;
				}
				case 63:
				{
					ResetCallBacks();
					SCREEN_SatelliteCableBox(nPK_Screen);
					break;
				}
				case 64:
				{
					ResetCallBacks();
					SCREEN_CurrentLocation(nPK_Screen);
					break;
				}
				case 65:
				{
					ResetCallBacks();
					SCREEN_MediaAttributeSearchInput(nPK_Screen);
					break;
				}
				case 66:
				{
					ResetCallBacks();
					SCREEN_MediaAttributeSearchResult(nPK_Screen);
					break;
				}
				case 67:
				{
					ResetCallBacks();
					string sMac_address = pMessage->m_mapParameters[47];
					string sDescription = pMessage->m_mapParameters[163];
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_NewPhoneDetected(nPK_Screen, sMac_address, sDescription, iPK_PnpQueue);
					break;
				}
				case 68:
				{
					ResetCallBacks();
					SCREEN_FirstTime(nPK_Screen);
					break;
				}
				case 69:
				{
					ResetCallBacks();
					SCREEN_FileList_PlayLists(nPK_Screen);
					break;
				}
				case 70:
				{
					ResetCallBacks();
					SCREEN_VideosRemote(nPK_Screen);
					break;
				}
				case 71:
				{
					ResetCallBacks();
					SCREEN_FileList_Pictures_Docs(nPK_Screen);
					break;
				}
				case 72:
				{
					ResetCallBacks();
					SCREEN_GenericTwoButtonQuestion(nPK_Screen);
					break;
				}
				case 73:
				{
					ResetCallBacks();
					SCREEN_TryCodes(nPK_Screen);
					break;
				}
				case 74:
				{
					ResetCallBacks();
					SCREEN_ScreenSaver(nPK_Screen);
					break;
				}
				case 75:
				{
					ResetCallBacks();
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					string sDescription = pMessage->m_mapParameters[163];
					string ssComments = pMessage->m_mapParameters[164];
					SCREEN_Pick_Room_For_Device(nPK_Screen, iPK_Device, sDescription, ssComments);
					break;
				}
				case 76:
				{
					ResetCallBacks();
					SCREEN_Power(nPK_Screen);
					break;
				}
				case 77:
				{
					ResetCallBacks();
					SCREEN_Generic_DVD_Remote(nPK_Screen);
					break;
				}
				case 78:
				{
					ResetCallBacks();
					SCREEN_ViewPlaylist(nPK_Screen);
					break;
				}
				case 79:
				{
					ResetCallBacks();
					SCREEN_FileList_withPictures_Mobil(nPK_Screen);
					break;
				}
				case 80:
				{
					ResetCallBacks();
					SCREEN_GenericDvdMenu(nPK_Screen);
					break;
				}
				case 81:
				{
					ResetCallBacks();
					SCREEN_ViewAttributes(nPK_Screen);
					break;
				}
				case 82:
				{
					ResetCallBacks();
					SCREEN_PendingTasks(nPK_Screen);
					break;
				}
				case 83:
				{
					ResetCallBacks();
					SCREEN_AddAttribute(nPK_Screen);
					break;
				}
				case 84:
				{
					ResetCallBacks();
					SCREEN_RadioRemote(nPK_Screen);
					break;
				}
				case 85:
				{
					ResetCallBacks();
					SCREEN_GenericCDRemote(nPK_Screen);
					break;
				}
				case 86:
				{
					ResetCallBacks();
					SCREEN_FileList_Popup_Container(nPK_Screen);
					break;
				}
				case 87:
				{
					ResetCallBacks();
					SCREEN_CreateViewBookmarks(nPK_Screen);
					break;
				}
				case 88:
				{
					ResetCallBacks();
					SCREEN_BrowseBookmarks(nPK_Screen);
					break;
				}
				case 89:
				{
					ResetCallBacks();
					SCREEN_GenericAppController(nPK_Screen);
					break;
				}
				case 90:
				{
					ResetCallBacks();
					SCREEN_BookmarksByMediaType(nPK_Screen);
					break;
				}
				case 92:
				{
					ResetCallBacks();
					SCREEN_MythTvRemote(nPK_Screen);
					break;
				}
				case 93:
				{
					ResetCallBacks();
					SCREEN_Myth_all_commands(nPK_Screen);
					break;
				}
				case 94:
				{
					ResetCallBacks();
					SCREEN_Mytv_watching_tv(nPK_Screen);
					break;
				}
				case 95:
				{
					ResetCallBacks();
					SCREEN_Myth_live_tv_browse_mode(nPK_Screen);
					break;
				}
				case 96:
				{
					ResetCallBacks();
					SCREEN_Myth_playback_recording(nPK_Screen);
					break;
				}
				case 97:
				{
					ResetCallBacks();
					string sText = pMessage->m_mapParameters[9];
					string sCommand_Line = pMessage->m_mapParameters[137];
					string sDescription = pMessage->m_mapParameters[163];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_GenericKeyboard(nPK_Screen, sText, sCommand_Line, sDescription, sCannotGoBack);
					break;
				}
				case 98:
				{
					ResetCallBacks();
					SCREEN_PingPong(nPK_Screen);
					break;
				}
				case 99:
				{
					ResetCallBacks();
					SCREEN_TVEPG1(nPK_Screen);
					break;
				}
				case 100:
				{
					ResetCallBacks();
					SCREEN_TVEPG2(nPK_Screen);
					break;
				}
				case 101:
				{
					ResetCallBacks();
					SCREEN_TVEPG3(nPK_Screen);
					break;
				}
				case 102:
				{
					ResetCallBacks();
					SCREEN_ShowInfo(nPK_Screen);
					break;
				}
				case 103:
				{
					ResetCallBacks();
					SCREEN_CreateViewBookmarksTV(nPK_Screen);
					break;
				}
				case 105:
				{
					ResetCallBacks();
					SCREEN_WebSites(nPK_Screen);
					break;
				}
				case 106:
				{
					ResetCallBacks();
					int iPK_Users = atoi(pMessage->m_mapParameters[17].c_str());
					string sMac_address = pMessage->m_mapParameters[47];
					SCREEN_WhatModelMobileOrbiter(nPK_Screen, iPK_Users, sMac_address);
					break;
				}
				case 107:
				{
					ResetCallBacks();
					SCREEN_Filelist(nPK_Screen);
					break;
				}
				case 108:
				{
					ResetCallBacks();
					SCREEN_DVDmenu(nPK_Screen);
					break;
				}
				case 109:
				{
					ResetCallBacks();
					SCREEN_Attributes(nPK_Screen);
					break;
				}
				case 110:
				{
					ResetCallBacks();
					SCREEN_DVDoptions(nPK_Screen);
					break;
				}
				case 111:
				{
					ResetCallBacks();
					SCREEN_AdvRemote(nPK_Screen);
					break;
				}
				case 112:
				{
					ResetCallBacks();
					SCREEN_Playlist(nPK_Screen);
					break;
				}
				case 113:
				{
					ResetCallBacks();
					SCREEN_AudioOptions(nPK_Screen);
					break;
				}
				case 114:
				{
					ResetCallBacks();
					SCREEN_Subtitles(nPK_Screen);
					break;
				}
				case 115:
				{
					ResetCallBacks();
					SCREEN_Angles(nPK_Screen);
					break;
				}
				case 116:
				{
					ResetCallBacks();
					string sDescription = pMessage->m_mapParameters[163];
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_NewPnpDevice(nPK_Screen, sDescription, iPK_PnpQueue);
					break;
				}
				case 117:
				{
					ResetCallBacks();
					SCREEN_VDR_Remote(nPK_Screen);
					break;
				}
				case 118:
				{
					ResetCallBacks();
					SCREEN_CdRemote(nPK_Screen);
					break;
				}
				case 119:
				{
					ResetCallBacks();
					SCREEN_PowerFile(nPK_Screen);
					break;
				}
				case 120:
				{
					ResetCallBacks();
					string sPhoneCallerID = pMessage->m_mapParameters[84];
					string sPhoneCallID = pMessage->m_mapParameters[87];
					string sChannel = pMessage->m_mapParameters[264];
					SCREEN_DevCallInProgress(nPK_Screen, sPhoneCallerID, sPhoneCallID, sChannel);
					break;
				}
				case 121:
				{
					ResetCallBacks();
					string sPhoneNumber = pMessage->m_mapParameters[75];
					string sSource_Channel = pMessage->m_mapParameters[269];
					string sDestination_Channel = pMessage->m_mapParameters[270];
					string sSource_Caller_ID = pMessage->m_mapParameters[271];
					string sDestination_Caller_ID = pMessage->m_mapParameters[272];
					SCREEN_DevIncomingCall(nPK_Screen, sPhoneNumber, sSource_Channel, sDestination_Channel, sSource_Caller_ID, sDestination_Caller_ID);
					break;
				}
				case 122:
				{
					ResetCallBacks();
					SCREEN_DevMoveCallsInProgress(nPK_Screen);
					break;
				}
				case 123:
				{
					ResetCallBacks();
					SCREEN_MapSecurity(nPK_Screen);
					break;
				}
				case 124:
				{
					ResetCallBacks();
					SCREEN_MapLighting(nPK_Screen);
					break;
				}
				case 125:
				{
					ResetCallBacks();
					SCREEN_MapClimate(nPK_Screen);
					break;
				}
				case 126:
				{
					ResetCallBacks();
					SCREEN_MapTelecom(nPK_Screen);
					break;
				}
				case 127:
				{
					ResetCallBacks();
					SCREEN_NavMode(nPK_Screen);
					break;
				}
				case 128:
				{
					ResetCallBacks();
					SCREEN_PVR_Full_Screen(nPK_Screen);
					break;
				}
				case 129:
				{
					ResetCallBacks();
					SCREEN_TI_Keyboard(nPK_Screen);
					break;
				}
				case 130:
				{
					ResetCallBacks();
					SCREEN_Help01(nPK_Screen);
					break;
				}
				case 131:
				{
					ResetCallBacks();
					SCREEN_Help02(nPK_Screen);
					break;
				}
				case 132:
				{
					ResetCallBacks();
					SCREEN_Help03(nPK_Screen);
					break;
				}
				case 133:
				{
					ResetCallBacks();
					SCREEN_SetupMythTv01(nPK_Screen);
					break;
				}
				case 134:
				{
					ResetCallBacks();
					SCREEN_SetupMythTv02(nPK_Screen);
					break;
				}
				case 135:
				{
					ResetCallBacks();
					SCREEN_SetupMythTv03(nPK_Screen);
					break;
				}
				case 136:
				{
					ResetCallBacks();
					SCREEN_SetupMythTv04(nPK_Screen);
					break;
				}
				case 137:
				{
					ResetCallBacks();
					SCREEN_PVR_Full_Screen_Options(nPK_Screen);
					break;
				}
				case 138:
				{
					ResetCallBacks();
					SCREEN_DVDFullScreen(nPK_Screen);
					break;
				}
				case 139:
				{
					ResetCallBacks();
					SCREEN_DVDFullScreenOpt(nPK_Screen);
					break;
				}
				case 140:
				{
					ResetCallBacks();
					SCREEN_VideosFullScreen(nPK_Screen);
					break;
				}
				case 141:
				{
					ResetCallBacks();
					SCREEN_VideosFullScreenOpt(nPK_Screen);
					break;
				}
				case 142:
				{
					ResetCallBacks();
					SCREEN_GenericAppFullScreen(nPK_Screen);
					break;
				}
				case 143:
				{
					ResetCallBacks();
					SCREEN_DVDMenuFullScreen(nPK_Screen);
					break;
				}
				case 144:
				{
					ResetCallBacks();
					SCREEN_DVDMenuFullScreenOpt(nPK_Screen);
					break;
				}
				case 145:
				{
					ResetCallBacks();
					SCREEN_VlcFullScreen(nPK_Screen);
					break;
				}
				case 146:
				{
					ResetCallBacks();
					SCREEN_VlcFullScreenOpt(nPK_Screen);
					break;
				}
				case 147:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Game_Commodore_Remote_Alt(nPK_Screen, sLocation);
					break;
				}
				case 148:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_rokuRemote(nPK_Screen, sLocation);
					break;
				}
				case 149:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_rokuKBD(nPK_Screen, sLocation);
					break;
				}
				case 150:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Game_Atari800KBD(nPK_Screen, sLocation);
					break;
				}
				case 151:
				{
					ResetCallBacks();
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Game_Intellivision(nPK_Screen, sLocation);
					break;
				}
				case 152:
				{
					ResetCallBacks();
					SCREEN_popOthers(nPK_Screen);
					break;
				}
				case 153:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_5200(nPK_Screen);
					break;
				}
				case 154:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_5200_Monitor_Mode(nPK_Screen);
					break;
				}
				case 155:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_5200_FS(nPK_Screen);
					break;
				}
				case 156:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_5200_FS_Options(nPK_Screen);
					break;
				}
				case 157:
				{
					ResetCallBacks();
					SCREEN_Game_SG1000_SMS_NES_Famicom(nPK_Screen);
					break;
				}
				case 158:
				{
					ResetCallBacks();
					SCREEN_Game_SG1000_SMS_NES_Famicom_Monitor_Mode(nPK_Screen);
					break;
				}
				case 159:
				{
					ResetCallBacks();
					SCREEN_Game_SG1000_SMS_NES_Famicom_FS(nPK_Screen);
					break;
				}
				case 160:
				{
					ResetCallBacks();
					SCREEN_Game_SG1000_SMS_NES_Famicom_FS_Options(nPK_Screen);
					break;
				}
				case 161:
				{
					ResetCallBacks();
					SCREEN_Game_Menu_Onscreen_NonOSD(nPK_Screen);
					break;
				}
				case 162:
				{
					ResetCallBacks();
					SCREEN_Hulu_Remote(nPK_Screen);
					break;
				}
				case 163:
				{
					ResetCallBacks();
					SCREEN_Hulu_OSD(nPK_Screen);
					break;
				}
				case 164:
				{
					ResetCallBacks();
					SCREEN_Hulu_OSD_Options(nPK_Screen);
					break;
				}
				case 165:
				{
					ResetCallBacks();
					SCREEN_Get_Capture_Card_Audio_Port(nPK_Screen);
					break;
				}
				case 166:
				{
					ResetCallBacks();
					SCREEN_Apple_2_Remote(nPK_Screen);
					break;
				}
				case 167:
				{
					ResetCallBacks();
					SCREEN_Apple_2_FS(nPK_Screen);
					break;
				}
				case 168:
				{
					ResetCallBacks();
					SCREEN_Commodore_Remote(nPK_Screen);
					break;
				}
				case 169:
				{
					ResetCallBacks();
					SCREEN_AirTunes(nPK_Screen);
					break;
				}
				case 170:
				{
					ResetCallBacks();
					SCREEN_AirTunes_FS(nPK_Screen);
					break;
				}
				case 171:
				{
					ResetCallBacks();
					SCREEN_AirTunes_FS_Options(nPK_Screen);
					break;
				}
				case 172:
				{
					ResetCallBacks();
					SCREEN_Commodore_FS(nPK_Screen);
					break;
				}
				case 173:
				{
					ResetCallBacks();
					SCREEN_Commodore_FS_Options(nPK_Screen);
					break;
				}
				case 174:
				{
					ResetCallBacks();
					SCREEN_CleanScreen(nPK_Screen);
					break;
				}
				case 175:
				{
					ResetCallBacks();
					SCREEN_VideoWizard(nPK_Screen);
					break;
				}
				case 176:
				{
					string sErrors = pMessage->m_mapParameters[18];
					SCREEN_DialogCannotPlayMedia(nPK_Screen, sErrors);
					break;
				}
				case 178:
				{
					string sPK_Device_From = pMessage->m_mapParameters[184];
					string sPK_Device_Ripping = pMessage->m_mapParameters[185];
					SCREEN_DialogRippingInProgress(nPK_Screen, sPK_Device_From, sPK_Device_Ripping);
					break;
				}
				case 179:
				{
					SCREEN_DialogCheckingDrive(nPK_Screen);
					break;
				}
				case 180:
				{
					SCREEN_DialogUnableToSavePlaylist(nPK_Screen);
					break;
				}
				case 181:
				{
					SCREEN_DialogPlaylistSaved(nPK_Screen);
					break;
				}
				case 182:
				{
					SCREEN_DialogUnableToLoadPlaylist(nPK_Screen);
					break;
				}
				case 183:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sTimeout = pMessage->m_mapParameters[182];
					SCREEN_DialogRippingError(nPK_Screen, sDescription, sTimeout);
					break;
				}
				case 184:
				{
					SCREEN_DialogRippingInstructions(nPK_Screen);
					break;
				}
				case 185:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sPromptToResetRouter = pMessage->m_mapParameters[181];
					string sTimeout = pMessage->m_mapParameters[182];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_DialogGenericError(nPK_Screen, sDescription, sPromptToResetRouter, sTimeout, sCannotGoBack);
					break;
				}
				case 186:
				{
					string sErrors = pMessage->m_mapParameters[18];
					SCREEN_DialogCannotBookmark(nPK_Screen, sErrors);
					break;
				}
				case 187:
				{
					string sPK_Device_From = pMessage->m_mapParameters[184];
					string sPK_Device_MediaSource = pMessage->m_mapParameters[186];
					string sStreamID_String = pMessage->m_mapParameters[187];
					string sPosition = pMessage->m_mapParameters[188];
					string sUsers = pMessage->m_mapParameters[189];
					string sPK_MediaType_String = pMessage->m_mapParameters[190];
					int iPK_Screen_GoTo = atoi(pMessage->m_mapParameters[226].c_str());
					SCREEN_DialogAskToResume(nPK_Screen, sPK_Device_From, sPK_Device_MediaSource, sStreamID_String, sPosition, sUsers, sPK_MediaType_String, iPK_Screen_GoTo);
					break;
				}
				case 188:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_DialogGC100Error(nPK_Screen, sDescription, sCannotGoBack);
					break;
				}
				case 189:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sPhoneName = pMessage->m_mapParameters[191];
					SCREEN_DialogPhoneInstructions(nPK_Screen, sDescription, sPhoneName);
					break;
				}
				case 190:
				{
					string sMac_address = pMessage->m_mapParameters[47];
					string sCommand_Line = pMessage->m_mapParameters[137];
					string sPK_Device_From = pMessage->m_mapParameters[184];
					string sPhoneName = pMessage->m_mapParameters[191];
					string sPK_Device_AppServer = pMessage->m_mapParameters[192];
					SCREEN_DialogSendFileToPhoneFailed(nPK_Screen, sMac_address, sCommand_Line, sPK_Device_From, sPhoneName, sPK_Device_AppServer);
					break;
				}
				case 191:
				{
					ResetCallBacks();
					SCREEN_Legacy_PVR_Cable_Box(nPK_Screen);
					break;
				}
				case 192:
				{
					ResetCallBacks();
					SCREEN_RoomsWizard(nPK_Screen);
					break;
				}
				case 193:
				{
					ResetCallBacks();
					SCREEN_UsersWizard(nPK_Screen);
					break;
				}
				case 194:
				{
					ResetCallBacks();
					SCREEN_CountryWizard(nPK_Screen);
					break;
				}
				case 196:
				{
					ResetCallBacks();
					SCREEN_TV_Manufacturer(nPK_Screen);
					break;
				}
				case 197:
				{
					ResetCallBacks();
					SCREEN_Receiver(nPK_Screen);
					break;
				}
				case 198:
				{
					ResetCallBacks();
					SCREEN_LightsSetup(nPK_Screen);
					break;
				}
				case 199:
				{
					ResetCallBacks();
					SCREEN_AlarmPanel(nPK_Screen);
					break;
				}
				case 200:
				{
					ResetCallBacks();
					SCREEN_VOIP_Provider(nPK_Screen);
					break;
				}
				case 201:
				{
					ResetCallBacks();
					SCREEN_AV_Devices(nPK_Screen);
					break;
				}
				case 202:
				{
					ResetCallBacks();
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_NAS_Options(nPK_Screen, iPK_PnpQueue);
					break;
				}
				case 204:
				{
					ResetCallBacks();
					SCREEN_Need_Reload_Router(nPK_Screen);
					break;
				}
				case 205:
				{
					ResetCallBacks();
					SCREEN_Need_Regen_Orbiter(nPK_Screen);
					break;
				}
				case 206:
				{
					ResetCallBacks();
					string sOptions = pMessage->m_mapParameters[39];
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_Sensors_Viewed_By_Camera(nPK_Screen, sOptions, iPK_PnpQueue);
					break;
				}
				case 207:
				{
					ResetCallBacks();
					SCREEN_PVR_OSD(nPK_Screen);
					break;
				}
				case 208:
				{
					ResetCallBacks();
					SCREEN_PVR_Recordings(nPK_Screen);
					break;
				}
				case 209:
				{
					ResetCallBacks();
					SCREEN_Demo_Media(nPK_Screen);
					break;
				}
				case 210:
				{
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					string sPK_Device_AppServer = pMessage->m_mapParameters[192];
					SCREEN_Download_are_ready_to_install(nPK_Screen, iPK_Device, sPK_Device_AppServer);
					break;
				}
				case 212:
				{
					ResetCallBacks();
					SCREEN_mnuAmbiance(nPK_Screen);
					break;
				}
				case 216:
				{
					ResetCallBacks();
					SCREEN_TVConfirmInputsToggle(nPK_Screen);
					break;
				}
				case 218:
				{
					ResetCallBacks();
					SCREEN_PVR_OSD_Full_Screen(nPK_Screen);
					break;
				}
				case 219:
				{
					ResetCallBacks();
					SCREEN_PVR_Recordings_Full_Screen(nPK_Screen);
					break;
				}
				case 220:
				{
					ResetCallBacks();
					SCREEN_CurrentlyActiveRemote(nPK_Screen);
					break;
				}
				case 222:
				{
					ResetCallBacks();
					SCREEN_mnuLights(nPK_Screen);
					break;
				}
				case 223:
				{
					ResetCallBacks();
					SCREEN_mnuPlaybackControl(nPK_Screen);
					break;
				}
				case 224:
				{
					ResetCallBacks();
					SCREEN_mnuSpeedControl(nPK_Screen);
					break;
				}
				case 225:
				{
					ResetCallBacks();
					SCREEN_mnuVolume(nPK_Screen);
					break;
				}
				case 230:
				{
					ResetCallBacks();
					SCREEN_Media_Tracks(nPK_Screen);
					break;
				}
				case 231:
				{
					ResetCallBacks();
					SCREEN_Which_Wizard(nPK_Screen);
					break;
				}
				case 232:
				{
					ResetCallBacks();
					bool bAlways = pMessage->m_mapParameters[225] == "1";
					SCREEN_This_Room(nPK_Screen, bAlways);
					break;
				}
				case 234:
				{
					ResetCallBacks();
					SCREEN_NAS_Options_when_Mounting_device(nPK_Screen);
					break;
				}
				case 235:
				{
					int iPK_Room = atoi(pMessage->m_mapParameters[57].c_str());
					int iPK_DHCPDevice = atoi(pMessage->m_mapParameters[150].c_str());
					string sDescription = pMessage->m_mapParameters[163];
					string ssComments = pMessage->m_mapParameters[164];
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_New_Pnp_Device_One_Possibility(nPK_Screen, iPK_Room, iPK_DHCPDevice, sDescription, ssComments, iPK_PnpQueue);
					break;
				}
				case 236:
				{
					ResetCallBacks();
					SCREEN_Wizard_Done(nPK_Screen);
					break;
				}
				case 237:
				{
					string sText = pMessage->m_mapParameters[9];
					string sCommand_Line = pMessage->m_mapParameters[137];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_House_Setup_Popup_Message(nPK_Screen, sText, sCommand_Line, sCannotGoBack);
					break;
				}
				case 238:
				{
					string sText = pMessage->m_mapParameters[9];
					string sCommand_Line = pMessage->m_mapParameters[137];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_Media_Player_Setup_Popup_Message(nPK_Screen, sText, sCommand_Line, sCannotGoBack);
					break;
				}
				case 239:
				{
					ResetCallBacks();
					SCREEN_Add_Software(nPK_Screen);
					break;
				}
				case 240:
				{
					ResetCallBacks();
					SCREEN_Choose_Folder(nPK_Screen);
					break;
				}
				case 241:
				{
					ResetCallBacks();
					SCREEN_Choose_Drive(nPK_Screen);
					break;
				}
				case 242:
				{
					ResetCallBacks();
					SCREEN_TV_Channels(nPK_Screen);
					break;
				}
				case 243:
				{
					ResetCallBacks();
					SCREEN_CD_Full_Screen_OSD(nPK_Screen);
					break;
				}
				case 244:
				{
					ResetCallBacks();
					SCREEN_Music_Full_Screen_OSD(nPK_Screen);
					break;
				}
				case 245:
				{
					ResetCallBacks();
					SCREEN_Choose_Provider_for_Device(nPK_Screen);
					break;
				}
				case 246:
				{
					ResetCallBacks();
					bool bAlready_processed = pMessage->m_mapParameters[125] == "1";
					string sDescription = pMessage->m_mapParameters[163];
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_Get_Username_Password_For_Devices(nPK_Screen, bAlready_processed, sDescription, iPK_PnpQueue);
					break;
				}
				case 247:
				{
					ResetCallBacks();
					SCREEN_Get_Capture_Card_Port(nPK_Screen);
					break;
				}
				case 248:
				{
					ResetCallBacks();
					string sPK_DesignObj = pMessage->m_mapParameters[3];
					SCREEN_Floorplan(nPK_Screen, sPK_DesignObj);
					break;
				}
				case 249:
				{
					ResetCallBacks();
					SCREEN_Halt_System(nPK_Screen);
					break;
				}
				case 250:
				{
					ResetCallBacks();
					SCREEN_PVR_Software(nPK_Screen);
					break;
				}
				case 251:
				{
					ResetCallBacks();
					SCREEN_Final_House_Setup(nPK_Screen);
					break;
				}
				case 252:
				{
					ResetCallBacks();
					SCREEN_ExternalDeviceFullScreen(nPK_Screen);
					break;
				}
				case 253:
				{
					ResetCallBacks();
					SCREEN_ExternalDeviceFullScreen_Opts(nPK_Screen);
					break;
				}
				case 255:
				{
					ResetCallBacks();
					SCREEN_Remote_Assistance(nPK_Screen);
					break;
				}
				case 256:
				{
					ResetCallBacks();
					SCREEN_Legacy_AV_Speed_Control(nPK_Screen);
					break;
				}
				case 257:
				{
					ResetCallBacks();
					SCREEN_Thumbnail(nPK_Screen);
					break;
				}
				case 258:
				{
					ResetCallBacks();
					SCREEN_Scanning_Progress(nPK_Screen);
					break;
				}
				case 259:
				{
					ResetCallBacks();
					SCREEN_Manage_Drives(nPK_Screen);
					break;
				}
				case 260:
				{
					ResetCallBacks();
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					SCREEN_Jukebox_Manager(nPK_Screen, iPK_Device);
					break;
				}
				case 261:
				{
					ResetCallBacks();
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					SCREEN_NAS_Manager(nPK_Screen, iPK_Device);
					break;
				}
				case 262:
				{
					ResetCallBacks();
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_AutoConfigure_TV(nPK_Screen, iPK_PnpQueue);
					break;
				}
				case 263:
				{
					ResetCallBacks();
					SCREEN_AdjustScreenSettings(nPK_Screen);
					break;
				}
				case 264:
				{
					ResetCallBacks();
					string sOptions = pMessage->m_mapParameters[39];
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_PNP_Generic_Options(nPK_Screen, sOptions, iPK_PnpQueue);
					break;
				}
				case 265:
				{
					ResetCallBacks();
					SCREEN_Zoom__Aspect(nPK_Screen);
					break;
				}
				case 266:
				{
					ResetCallBacks();
					SCREEN_Legacy_PVR_Cable_Box2(nPK_Screen);
					break;
				}
				case 267:
				{
					ResetCallBacks();
					SCREEN_Active_Calls(nPK_Screen);
					break;
				}
				case 268:
				{
					ResetCallBacks();
					string sReason = pMessage->m_mapParameters[273];
					SCREEN_Call_Dropped(nPK_Screen, sReason);
					break;
				}
				case 269:
				{
					ResetCallBacks();
					bool bTrueFalse = pMessage->m_mapParameters[119] == "1";
					string sTask = pMessage->m_mapParameters[257];
					SCREEN_Assisted_Transfer_In_Progress(nPK_Screen, bTrueFalse, sTask);
					break;
				}
				case 270:
				{
					ResetCallBacks();
					SCREEN_Static_IP_settings(nPK_Screen);
					break;
				}
				case 271:
				{
					ResetCallBacks();
					SCREEN_Network_Settings(nPK_Screen);
					break;
				}
				case 272:
				{
					ResetCallBacks();
					SCREEN_PVR_Recordings_Full_Screen_Options(nPK_Screen);
					break;
				}
				case 273:
				{
					ResetCallBacks();
					SCREEN_VDR_Recordings_Full_Screen_Options(nPK_Screen);
					break;
				}
				case 274:
				{
					ResetCallBacks();
					SCREEN_VDR_OSD(nPK_Screen);
					break;
				}
				case 275:
				{
					ResetCallBacks();
					SCREEN_VDR_Recordings_Full_Screen(nPK_Screen);
					break;
				}
				case 276:
				{
					ResetCallBacks();
					SCREEN_VDR_OSD_Full_Screen(nPK_Screen);
					break;
				}
				case 277:
				{
					ResetCallBacks();
					SCREEN_VDR_Full_Screen_Options(nPK_Screen);
					break;
				}
				case 278:
				{
					ResetCallBacks();
					SCREEN_VDR_Full_Screen(nPK_Screen);
					break;
				}
				case 279:
				{
					ResetCallBacks();
					SCREEN_VDR_Recordings(nPK_Screen);
					break;
				}
				case 280:
				{
					ResetCallBacks();
					SCREEN_Simple_TV_Remote(nPK_Screen);
					break;
				}
				case 281:
				{
					ResetCallBacks();
					SCREEN_Non_Pluto_TV_Full_screen(nPK_Screen);
					break;
				}
				case 282:
				{
					ResetCallBacks();
					SCREEN_Non_Pluto_TV_Full_screen_options(nPK_Screen);
					break;
				}
				case 283:
				{
					ResetCallBacks();
					string sDescription = pMessage->m_mapParameters[163];
					SCREEN_Cannot_Reload_Router(nPK_Screen, sDescription);
					break;
				}
				case 284:
				{
					ResetCallBacks();
					SCREEN_Game(nPK_Screen);
					break;
				}
				case 285:
				{
					ResetCallBacks();
					SCREEN_Rip_Options(nPK_Screen);
					break;
				}
				case 286:
				{
					ResetCallBacks();
					SCREEN_Rip_format_options(nPK_Screen);
					break;
				}
				case 287:
				{
					ResetCallBacks();
					SCREEN_Privacy_options_for_rip(nPK_Screen);
					break;
				}
				case 288:
				{
					ResetCallBacks();
					SCREEN_Game_Screen_FS(nPK_Screen);
					break;
				}
				case 289:
				{
					ResetCallBacks();
					SCREEN_Game_Screen_FS_Options(nPK_Screen);
					break;
				}
				case 290:
				{
					ResetCallBacks();
					SCREEN_Current_Disc_Contents(nPK_Screen);
					break;
				}
				case 291:
				{
					ResetCallBacks();
					string sTracks = pMessage->m_mapParameters[121];
					int iEK_Attribute = atoi(pMessage->m_mapParameters[123].c_str());
					int iEK_Disc = atoi(pMessage->m_mapParameters[131].c_str());
					int iEK_File = atoi(pMessage->m_mapParameters[145].c_str());
					SCREEN_Edit_Attributes(nPK_Screen, sTracks, iEK_Attribute, iEK_Disc, iEK_File);
					break;
				}
				case 292:
				{
					ResetCallBacks();
					SCREEN_Game_Monitor_Mode(nPK_Screen);
					break;
				}
				case 293:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_2600_Monitor_Mode(nPK_Screen);
					break;
				}
				case 294:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_2600_FS_Options(nPK_Screen);
					break;
				}
				case 295:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_2600(nPK_Screen);
					break;
				}
				case 296:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_2600_FS(nPK_Screen);
					break;
				}
				case 297:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_2600_Controller_Options(nPK_Screen);
					break;
				}
				case 298:
				{
					ResetCallBacks();
					SCREEN_Generic_BluRay_Remote(nPK_Screen);
					break;
				}
				case 299:
				{
					ResetCallBacks();
					SCREEN_NonPluto_BluRay_FS(nPK_Screen);
					break;
				}
				case 300:
				{
					ResetCallBacks();
					SCREEN_NonPluto_BluRay_FS_Options(nPK_Screen);
					break;
				}
				case 301:
				{
					ResetCallBacks();
					SCREEN_Tivo_Series_3_Remote(nPK_Screen);
					break;
				}
				case 302:
				{
					ResetCallBacks();
					SCREEN_Tivo_Series_2_Remote(nPK_Screen);
					break;
				}
				case 303:
				{
					ResetCallBacks();
					SCREEN_Weather_Now(nPK_Screen);
					break;
				}
				case 304:
				{
					ResetCallBacks();
					SCREEN_Weather_Forecast(nPK_Screen);
					break;
				}
				case 305:
				{
					ResetCallBacks();
					SCREEN_Weather_3day(nPK_Screen);
					break;
				}
				case 306:
				{
					ResetCallBacks();
					SCREEN_Weather_6day(nPK_Screen);
					break;
				}
				case 307:
				{
					ResetCallBacks();
					SCREEN_Weather_Radar(nPK_Screen);
					break;
				}
				case 308:
				{
					ResetCallBacks();
					SCREEN_WD_HDTVLive_Remote(nPK_Screen);
					break;
				}
				case 309:
				{
					ResetCallBacks();
					SCREEN_XBOX_360_Num2(nPK_Screen);
					break;
				}
				case 310:
				{
					ResetCallBacks();
					SCREEN_TI_99_FS(nPK_Screen);
					break;
				}
				case 311:
				{
					ResetCallBacks();
					SCREEN_TI_99_FS_Options(nPK_Screen);
					break;
				}
				case 312:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_800(nPK_Screen);
					break;
				}
				case 313:
				{
					ResetCallBacks();
					SCREEN_Game_Atari_800_Menu_Visible(nPK_Screen);
					break;
				}

			default:
				BadGotoScreen(nPK_Screen);
				break;
			}
		}
	};
}

#endif
