/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEAVMESSAGETRANSLATION_H
#define DCEAVMESSAGETRANSLATION_H

#include "DefaultMessageTranslation.h"

#include <time.h>

namespace DCE {

class AVMessageTranslator : public DefaultMessageTranslator {
public:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);
	virtual bool SetDelays(MessageReplicator& inrepl, long devtemplid=-1);
	virtual bool InitDelaysMap(long devtemplid);

public:
	map<int, bool> lastcmdwaspause_; // So if we get 2 pauses in a row, we convert the 2nd one to a play
	map<int, bool> laststatus_power_;
	map<int, int> laststatus_input_;
	map<int, int> input_commands_;
	map<int, vector<int> *> device_input_command_order_;
	
private:
	map<int,int> map_ModeDelay;
	map<int,int> map_TogglePower;
	map<int,int> map_ToggleInput;
	map<int, int> map_DigitDelay;
	map<int, string> map_NumericEntry;
	map<int, time_t> map_LastTime;
};

class AVMessageDispatcher : public DefaultMessageDispatcher {
public:
	/*to be continued...*/
};

typedef DefaultMessageProcessor<AVMessageTranslator, AVMessageDispatcher> AVMessageProcessor;

};

#endif
