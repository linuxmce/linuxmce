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
$GEN_CLASSES$
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
$GEN_METHODS$
		virtual void ReceivedGotoScreenMessage(int nPK_Screen, Message *pMessage)
		{
			switch(nPK_Screen) 
			{
$GEN_SWITCH_BLOCK$
			default:
				BadGotoScreen(nPK_Screen);
				break;
			}
		}
	};
}

#endif
