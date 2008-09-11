/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file MessageBuffer.h
 Header file for the Command_Impl class.
 @todo notcommented
 */


#ifndef MessageBuffer_h
#define MessageBuffer_h

#include "Message.h"

namespace DCE
{
    /**
    @class MessageBuffer
    This class ???
     */
	class MessageBuffer
	{
		pthread_cond_t m_MessageBufferCond; /**< condition for the messages in the queue */
		pluto_pthread_mutex_t m_MessageBufferMutex; /**< for protecin the access to the MessageBuffer */
		class Command_Impl *m_pCommand_Impl;
		list<Message *> m_listMessage;
		Message *m_pMessage_In_Process;

	public:

		bool m_bThreadRunning;

		MessageBuffer(class Command_Impl *pCommand_Impl);
		bool BufferMessage(Message *pMessage);
		void Run();
		bool MessagesConflict(Message *pMessage1,Message *pMessage2);
	};
}

#endif
