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
#ifndef IRBASE
#define IRBASE

#include "DCE/Command_Impl.h"
#include "MessageTranslation/AVMessageTranslation.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/Other.h"

using namespace DCE;

string ConvertRC5_6(string sCode);  // From GenerateRcX

class IRBase : public AVMessageProcessor {
	Command_Impl *m_pCommand_Impl;
public:
	IRBase()
		: m_pCommand_Impl(NULL),
		  m_bMustConvertRC5_6(false),
		  m_bQuit_(false)
	{};
	
	virtual ~IRBase() { m_bQuit_ = true; };

public:
	typedef pair<long, long> longPair;
	map<longPair, std::string>& getCodeMap() {
		return codemap_;
	}
	
		
protected:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);
	virtual void DispatchMessage(Message* pmsg);
	Command_Impl *getCommandImpl() { return m_pCommand_Impl; };
	void setCommandImpl(Command_Impl *pCommand_Impl) { m_pCommand_Impl=pCommand_Impl; };
	
protected:
	/*exposed methods*/
	virtual void SendIR(string Port, string IRCode,int iRepeat) = 0;

	virtual void handleStart();
	virtual void handleStart(Command_Impl *pCommand_Impl);
	virtual void handleStop();

private:
	void PushSendCodeMessage(Message* porigmsg, MessageReplicatorList& outrepls, 
				long cmdid, const std::string& port, int count = 1, int delay = 0);
	
private:
	map<longPair, std::string> codemap_;		// maps device,command pair to IR code
	map<long,pair<int,int> > m_mapDevice_IRRepeat; // Map the device to the i/r repeat for normal commands and volume commands

protected:
	bool m_bMustConvertRC5_6;
	bool m_bQuit_;
};

#endif
