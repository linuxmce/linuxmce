/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file GSDMessageProcessing.h
Message processing classes
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
#ifndef DCEGSDMESSAGEPROCESSING_H
#define DCEGSDMESSAGEPROCESSING_H

#include "AVMessageTranslation.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class GSDMessageTranslator
A class for translating messages.
*/
class GSDMessageTranslator : public AVMessageTranslator {
public:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);
};

/** @class GSDMessageDispatcher
A class for dispatching messages.

Incomplete!
*/
class GSDMessageDispatcher : public AVMessageDispatcher {
public:
	/*to be continued...*/
};

typedef DefaultMessageProcessor<GSDMessageTranslator, GSDMessageDispatcher> GSDMessageProcessor;

};

#endif
