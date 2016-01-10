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
#ifndef VFD_LCD_Base_h
#define VFD_LCD_Base_h

#define VL_MSGTYPE_STARTUP					1
#define VL_MSGTYPE_STATUS					2
#define VL_MSGTYPE_NOW_PLAYING_MAIN			3
#define VL_MSGTYPE_NOW_PLAYING_SECTION		4
#define VL_MSGTYPE_NOW_PLAYING_TIME_CODE	5
#define VL_MSGTYPE_NOW_PLAYING_SPEED		6
#define VL_MSGTYPE_RIPPING_NAME				7
#define VL_MSGTYPE_RIPPING_PROGRESS			8
#define VL_MSGTYPE_RUNTIME_ERRORS			9
#define VL_MSGTYPE_RUNTIME_NOTICES			10

#include "MenuStructure.h"

class VFD_LCD_Message
{
public:
	string m_sMessage;
	time_t m_tExpireTime,m_tDisplayTime,m_tCreateTime;
	int m_iMessageType;

	VFD_LCD_Message(int iMessageType,string sMessage,int ExpireTime)
	{
		m_iMessageType=iMessageType;
		m_sMessage=sMessage;
		m_tCreateTime=time(NULL);
		m_tExpireTime = m_tCreateTime+ExpireTime;
		m_tDisplayTime=0;
	}
};

typedef map<string,VFD_LCD_Message *> MapMessages;

namespace DCE
{
	class VFD_LCD_Base
	{
	public:
		bool m_bVL_ThreadRunning;
		void RunThread();
		void WakeUpThread();
		int iNumLines_get() { return m_iNumLines; }
		int iNumVisibleColums_get() { return m_iNumVisibleColumns; }
		void SetMenuNode_Current(class MenuNode *pMenuNode) { m_pMenuNode_Current=pMenuNode; if( m_pMenuNode_Current ) m_pMenuNode_Current->PrepareNodes(); }

	protected:
		pluto_pthread_mutex_t m_VL_MessageMutex;  // This will also protect the callback map
		pthread_cond_t m_VL_MessageCond;
		pthread_t m_ptVL_Thread;
		bool m_bQuit_VL;
		int m_iNumColumns,m_iNumLines,m_iNumVisibleColumns;
		VFD_LCD_Message *m_pVFD_LCD_Message_new;
		class MenuStructure *m_pMenuStructure;
		class MenuNode *m_pMenuNode_Current;

		map<int,MapMessages *> m_mapMessages;  // All active messages, by type
		map<int,string> m_mapLastMessageByType; // The last message shown per type/name

		MapMessages *m_mapMessages_Find(int iMessageType) { map<int,MapMessages *>::iterator it = m_mapMessages.find(iMessageType); return it==m_mapMessages.end() ? NULL : (*it).second; }
		MapMessages *m_mapMessages_Get(int iMessageType) { 
			MapMessages *pMapMessages = m_mapMessages_Find(iMessageType); 
			if( !pMapMessages )
			{
				pMapMessages = new MapMessages();
				m_mapMessages[iMessageType]=pMapMessages;
			}
			return pMapMessages;
		}
		bool ContainsMessagesOfType(int iMessageType) { MapMessages *p = m_mapMessages_Find(iMessageType); return p ? p->size()>0 : false; }

		VFD_LCD_Base(int iNumColumns,int iNumLines,int iNumVisibleColumns);
		virtual ~VFD_LCD_Base();

		void SetMenuStructure(MenuStructure *pMenuStructure) { delete m_pMenuStructure; m_pMenuStructure=pMenuStructure; }

		virtual void NewMessage(int iMessageType,string sName,string sMessage,int ExpiresSeconds);
		virtual int UpdateDisplay();
		virtual void DisplayMessage(VFD_LCD_Message *pVFD_LCD_Message);

		virtual int DisplayErrorMessages();
		virtual int DisplayNoticesMessages();
		virtual int DisplayNowPlayingRippingMessages();
		virtual int DisplayStatusMessages();
		virtual void DisplayDate();
		virtual int DisplayStandardMessages(int iMessageType);

		virtual void GetNowPlaying(vector<string> *vectString,int iNumLines);
		virtual void GetRipping(vector<string> *vectString,int iNumLines);

		virtual void DoUpdateDisplay(vector<string> *vectString) {} // A derived class is expected to implement this
		virtual bool CheckActivateMenu();

		virtual void Up();
		virtual void Down();
		virtual void Left();
		virtual void Right();
		virtual void Enter();
		virtual void Back();
		virtual void Home();
	};

}
#endif
