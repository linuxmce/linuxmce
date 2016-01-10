/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef IRReceiverBase_h
#define IRReceiverBase_h

/* Since we will have multiple IRReceiver devices, put all the common functionality regarding looking
up codes in a shared base class */

#include "DCE/Command_Impl.h"
#include "DCE/Virtual_Device_Translator.h"

typedef map< pair<char,char>,Message *> MapKeysToMessages;  // ScreenType,RemoteLayout

namespace DCE
{
	class IRReceiverBase
	{
	protected:
		map<int,char> m_mapRemoteLayout;
		Command_Impl *m_pCommand_Impl;
		Virtual_Device_Translator m_Virtual_Device_Translator;
		map<string,MapKeysToMessages *> m_mapKeyMapping;
		char m_cCurrentScreen;
 		std::map<std::string, std::string> m_mapAVWCommands;

		pluto_pthread_mutex_t m_RepeatThreadMutex;  // Needed so we can have a timed cond wait
		pthread_cond_t m_RepeatThreadCond;
		bool m_bRepeatKey;  // This is true when we're repeating a key
		bool m_bIgnore;  // If true, ignore this i/r receiver.  Usually this happens where there are multiple i/r receivers in an m/d
		pthread_t m_pt_Repeat; // The thread that we're using to do the repeating
		string m_sRepeatCode; // The code that we're repeating
		int m_PK_Device_Remote; // The remote corresponding to the repeat code

	public:
		IRReceiverBase(Command_Impl *pCommand_Impl = NULL);
		~IRReceiverBase();

		void Setup(Command_Impl *pCommand_Impl) { m_pCommand_Impl = pCommand_Impl; }
        
		void ReceivedCode(int PK_Device_Remote,const char *pCode,const char *pRepeat=NULL,int iRepeat=0);  // if Repeat>0 this will be in COMMANDPARAMETER_Repeat_Command_CONST, which is used by i/r devices
		void GetConfig(DeviceData_Impl *pData);
		void StopRepeatCode();  // Stop any code that's repeating

#ifndef WIN32
		void ForceKeystroke(string sCommand, string sAVWHost, int iAVWPort);
		bool SendSocketText(int Socket, std::string Text) const;
		std::string GetSocketText(int Socket) const;
#endif
	public:
		void RepeatThread();  // The loop to handle repeating codes

	};
}

#endif
