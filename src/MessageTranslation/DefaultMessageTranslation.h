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
#ifndef DCEDEFAULTMESSAGETRANSLATION_H
#define DCEDEFAULTMESSAGETRANSLATION_H

#include "MessageTranslation.h"

namespace DCE {


/*****************************************************************
DefaultMessageTranslator
*****************************************************************/

class DefaultMessageTranslator : public MessageTranslator {
protected:
	DefaultMessageTranslator();
	virtual ~DefaultMessageTranslator();

protected:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);
	virtual bool InitDelaysMap(long devtemplid);

protected:
	std::map<int,int> map_PowerDelay;
private:

};

/*****************************************************************
DefaultMessageDispatcher
*****************************************************************/

class DefaultMessageDispatcher : public MessageDispatcher {
protected:
	virtual void DispatchMessage(Message* pmsg) = 0;
	
protected:
	virtual void DispatchMessage(MessageReplicator& inrepl);
};

/*****************************************************************
DefaultMessageProcessor
*****************************************************************/

template <class T = DefaultMessageTranslator, class D = DefaultMessageDispatcher>
class DefaultMessageProcessor : public MessageProcessor<T, D>  {
public:
	DefaultMessageProcessor() 
		: pAllDevices_(NULL) {}	
public:
	AllDevices *getAllDevices() {
		return pAllDevices_;
	}
	void setAllDevices(AllDevices *pAllDevices) {
		pAllDevices_ = pAllDevices;
	}

private:
	AllDevices *pAllDevices_;
};

};

#endif
