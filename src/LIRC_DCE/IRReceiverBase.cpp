/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "IRReceiverBase.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/PlutoDefs.h"

#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"

#include <iostream>
using namespace std;
using namespace DCE;

void *RepeatThread(void *p)
{
	IRReceiverBase* pIRReceiverBase = (IRReceiverBase *)p;
	pIRReceiverBase->RepeatThread();
	return NULL;
}

IRReceiverBase::IRReceiverBase(Command_Impl *pCommand_Impl)
: m_Virtual_Device_Translator(), m_RepeatThreadMutex("repeat")
{
	m_pCommand_Impl=pCommand_Impl;
	m_cCurrentScreen='M';

	int iResult=pthread_cond_init(&m_RepeatThreadCond, NULL);
	int iResult2=m_RepeatThreadMutex.Init(NULL,&m_RepeatThreadCond);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::IRReceiverBase r1 %d r2 %d cond %p",
		iResult,iResult2,&m_RepeatThreadCond);
#endif

	m_bRepeatKey=false;
#ifdef PTHREAD2
	m_pt_Repeat.p = 0;
#else
	m_pt_Repeat=0;
#endif

	m_mapAVWCommands["ok"] = "return";
	m_mapAVWCommands["volup"] = "plus";
	m_mapAVWCommands["voldn"] = "minus";
	m_mapAVWCommands["chup"] = "plus";
	m_mapAVWCommands["chdown"] = "minus";
	m_mapAVWCommands["back"] = "escape";
	m_mapAVWCommands["stop"] = "escape";
	m_mapAVWCommands["power"] = "escape";
}

void IRReceiverBase::GetConfig(DeviceData_Impl *pData)
{
	m_bIgnore = pData->m_mapParameters_Find(DEVICEDATA_Ignore_CONST)=="1";
	if( m_bIgnore )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::GetConfig all codes will be ignored");
		return;
	}
	string sMapping;
	DCE::CMD_Get_Remote_Control_Mapping_DT CMD_Get_Remote_Control_Mapping_DT(pData->m_dwPK_Device, DEVICETEMPLATE_Infrared_Plugin_CONST,
		BL_SameHouse, &sMapping);
	m_pCommand_Impl->SendCommand(CMD_Get_Remote_Control_Mapping_DT);  // Get the codes from I/R Plugin

	string::size_type posTab=0;
	while(posTab<sMapping.size())
	{
		char cScreenType = StringUtils::Tokenize(sMapping,"\t",posTab)[0];
		char cRemoteLayout = StringUtils::Tokenize(sMapping,"\t",posTab)[0];
		string sThisMapping = StringUtils::Tokenize(sMapping,"\t",posTab);
		vector<string> vectButtons;
		StringUtils::Tokenize(sThisMapping,"\r\n",vectButtons);
		for(size_t s=0;s<vectButtons.size();++s)
		{
			string::size_type pos=0;
			// We got ourselves a valid one
			string::size_type posSlash=vectButtons[s].find('|');
			while( pos<vectButtons[s].size() )
			{
				string sToken = StringUtils::Tokenize(vectButtons[s],";|",pos);
				if( posSlash!=string::npos )  // Should always be true unless data is malformed
				{
					int iNumberOfArguments;
					char **pArgs = StringUtils::ConvertStringToArgs(vectButtons[s].substr(posSlash+1),iNumberOfArguments);
					if( iNumberOfArguments )  // Should always be true
					{
						//LoggerWrapper::GetInstance()->Write(LV_STATUS,"Screen %c, Layout %c, LIRCBtn %s fires: %s",cScreenType ? cScreenType : '*', cRemoteLayout ? cRemoteLayout : '*', sToken.c_str(),vectButtons[s].substr(posSlash+1).c_str());
						Message *pMessage = new Message(iNumberOfArguments,pArgs,pData->m_dwPK_Device);
						PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(pArgs, iNumberOfArguments);
						m_Virtual_Device_Translator.TranslateVirtualDevice(pMessage->m_dwPK_Device_To,pMessage->m_dwPK_Device_To);
						for(size_t s=0;s<pMessage->m_vectExtraMessages.size();++s)
							m_Virtual_Device_Translator.TranslateVirtualDevice(pMessage->m_vectExtraMessages[s]->m_dwPK_Device_To,pMessage->m_vectExtraMessages[s]->m_dwPK_Device_To);

						MapKeysToMessages *pMapKeysToMessages = m_mapKeyMapping[StringUtils::ToUpper(sToken)];
						if( !pMapKeysToMessages )
						{
							pMapKeysToMessages = new MapKeysToMessages();
							m_mapKeyMapping[StringUtils::ToUpper(sToken)] = pMapKeysToMessages;
						}

#ifdef WIN32
						(*pMapKeysToMessages)[make_pair (cRemoteLayout, cScreenType)] = pMessage;
#else
						(*pMapKeysToMessages)[ make_pair<char,char> (cRemoteLayout,cScreenType) ] = pMessage;
#endif
					}
					if( posSlash<=pos )
						break; // We already parsed the last one
				}
			}
		}
	}
}

IRReceiverBase::~IRReceiverBase()
{
#ifdef PTHREAD2
	if (m_pt_Repeat.p)
#else
	if( m_pt_Repeat )
#endif
	{
		m_bRepeatKey = false;
		pthread_cond_broadcast(&m_RepeatThreadCond);
		pthread_join(m_pt_Repeat,NULL);
#ifdef PTHREAD2
		m_pt_Repeat.p = 0;
#else
		m_pt_Repeat=0;
#endif
	}

	for(map<string,MapKeysToMessages *>::iterator it=m_mapKeyMapping.begin();it!=m_mapKeyMapping.end();++it)
	{
		MapKeysToMessages *pMapKeysToMessages = it->second;
		if(NULL != pMapKeysToMessages)
		{
			for(MapKeysToMessages::iterator it_message = pMapKeysToMessages->begin(); it_message != pMapKeysToMessages->end(); ++it_message)
			{
				delete it_message->second;
			}

			pMapKeysToMessages->clear();
			delete pMapKeysToMessages;
		}
	}
	m_mapKeyMapping.clear();
}

void IRReceiverBase::ReceivedCode(int PK_Device_Remote,const char *pCode,const char *pRepeat, int iRepeat)
{
	if( m_bIgnore )
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::ReceivedCode device %d code %s repeat %s/%d",
		PK_Device_Remote,pCode,pRepeat,iRepeat);
#endif

	char cRemoteLayout = m_mapRemoteLayout[PK_Device_Remote];
	map<string,MapKeysToMessages *>::iterator it = m_mapKeyMapping.find(StringUtils::ToUpper(pCode));
	if( it!=m_mapKeyMapping.end() )
	{
		MapKeysToMessages *pMapKeysToMessages = it->second;
		MapKeysToMessages::iterator itMessage;
		
#ifdef WIN32
		itMessage = pMapKeysToMessages->find(make_pair (cRemoteLayout, m_cCurrentScreen));
#else
		itMessage = pMapKeysToMessages->find(make_pair<char,char> (cRemoteLayout,m_cCurrentScreen) );
#endif
		if (itMessage == pMapKeysToMessages->end())
		{
#ifdef WIN32
			itMessage = pMapKeysToMessages->find(make_pair (cRemoteLayout, 0));
#else
			itMessage = pMapKeysToMessages->find(make_pair<char, char>(cRemoteLayout, 0));
#endif
		}
		if (itMessage == pMapKeysToMessages->end())
		{
#ifdef WIN32
			itMessage = pMapKeysToMessages->find(make_pair (0, m_cCurrentScreen));
#else
			itMessage = pMapKeysToMessages->find(make_pair<char, char>(0, m_cCurrentScreen));
#endif
		}
		if (itMessage == pMapKeysToMessages->end())
		{
#ifdef WIN32
			itMessage = pMapKeysToMessages->find(make_pair (0, 0));
#else
			itMessage = pMapKeysToMessages->find(make_pair<char, char>(0, 0));
#endif
		}

		if( itMessage!=pMapKeysToMessages->end() )
		{
			Message *pm = itMessage->second;
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::ReceivedCode Sending Message Type %d ID %d by  code: %s device %d (screen %c remote layout %c) repeat %s/%d",
					pm->m_dwMessage_Type,pm->m_dwID,pCode,PK_Device_Remote,m_cCurrentScreen,cRemoteLayout,pRepeat,iRepeat);

			Message *pMessage = new Message(pm);
			if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && iRepeat )
				pMessage->m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST] = StringUtils::itos(iRepeat);

			if( pMessage->m_dwPK_Device_To == m_pCommand_Impl->m_dwPK_Device )
			{
				m_pCommand_Impl->ReceivedMessage(pMessage);
				delete pMessage;
			}
			else
				m_pCommand_Impl->QueueMessageToRouter(pMessage);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"No mapping for code: %s device %d",pCode,PK_Device_Remote);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Cannot find code %s device %d",pCode,PK_Device_Remote);

	// See if we've got a code to repeat
	if( pRepeat && *pRepeat )
	{
#ifdef PTHREAD2
		if (m_pt_Repeat.p)
#else
		if (m_pt_Repeat)
#endif
		{
			StopRepeatCode();
		}

		m_bRepeatKey=true;
		m_sRepeatCode=pRepeat;
		m_PK_Device_Remote=PK_Device_Remote;
		pthread_create(&m_pt_Repeat, NULL, ::RepeatThread, (void*)this);
	}
}

void IRReceiverBase::StopRepeatCode()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::StopRepeatCode");
#endif
	m_bRepeatKey=false;
#ifdef PTHREAD2
	if (m_pt_Repeat.p)
#else
	if( m_pt_Repeat )
#endif
	{
		pthread_cond_broadcast(&m_RepeatThreadCond);
		pthread_join(m_pt_Repeat,NULL);
#ifdef PTHREAD2
		m_pt_Repeat.p = 0;
#else
		m_pt_Repeat=0;
#endif
	}
}

void IRReceiverBase::RepeatThread()
{
	PLUTO_SAFETY_LOCK(rm,m_RepeatThreadMutex);
	rm.TimedCondWait(0,500 * 1000000);

	if( !m_bRepeatKey || m_sRepeatCode.empty() )
		return;

	int iRepeat=0;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::RepeatThread device %d code %s/%d",
			m_PK_Device_Remote,m_sRepeatCode.c_str(),iRepeat);
#endif
	ReceivedCode(m_PK_Device_Remote,m_sRepeatCode.c_str());

	while( m_bRepeatKey )
	{
		rm.TimedCondWait(0,250 * 1000000);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::RepeatThread_while device %d code %s/%d",
			m_PK_Device_Remote,m_sRepeatCode.c_str(),iRepeat);
#endif
		if( !m_bRepeatKey )
			return;
		ReceivedCode(m_PK_Device_Remote,m_sRepeatCode.c_str(),NULL,iRepeat++);
	}
}

#ifndef WIN32
std::string IRReceiverBase::GetSocketText(int Socket) const
{
	int Len = 0;
	int iRecv = recv(Socket, &Len, sizeof(Len), 0);
	if( iRecv != sizeof(Len) || Len <= 0 )
	{
		return std::string();
	}
	
	std::string Buffer;
	Buffer.reserve(Len);
	Buffer.insert(0, Len, 0);
	iRecv = recv(Socket, const_cast<char*>(Buffer.data()), Len, 0);
	if( iRecv != Len )
	{
		return std::string();
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Buffer %d = %s", Buffer.length(), Buffer.c_str());
	
	return Buffer;
}

bool IRReceiverBase::SendSocketText(int Socket, std::string Text) const
{
	int Len = Text.length();
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SendSocketText %d = %s", Len, Text.c_str());
	
	if(  0 > write(Socket, &Len, sizeof(Len)) )
	{
		return false;
	}
	
	if( Len != write(Socket, Text.c_str(), Len) )
	{
		return false;
	}
	
	return true;
}

void IRReceiverBase::ForceKeystroke(string sCommand, string sAVWHost, int iAVWPort)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRReceiverBase::ForceKeystroke %s",sCommand.c_str());
	
	map<string, string>::iterator it = m_mapAVWCommands.find(sCommand);
	if(it != m_mapAVWCommands.end())
		sCommand = it->second;
	
	int sockfd = 0;
	struct hostent *he = 0;
	struct sockaddr_in their_addr; // connector's address information 

	if ((he=gethostbyname(sAVWHost.c_str())) == NULL) {  // get the host info 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed on gethostbyname");
		return;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to open a new socket");
		return;
	}

	bzero(&their_addr, sizeof(their_addr));
	their_addr.sin_family = AF_INET;    // host byte order 
	their_addr.sin_port = htons(iAVWPort);  // short, network byte order 
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);

	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to connect on AVW");
		close(sockfd);
		return;
	}

	std::string Text = GetSocketText(sockfd);
	if( Text == "OPEN" )
	{
		if( !SendSocketText(sockfd, sCommand) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to send to AVW: %s", sCommand.c_str());
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received from AVW : %s", Text.c_str());
	}
	
	close(sockfd);
}
#endif

