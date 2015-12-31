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
#ifndef __MouseGovernor_H__
#define __MouseGovernor_H__
//-----------------------------------------------------------------------------------------------------

#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCE/Message.h"

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Limits mouse movements to only 1 move per x ms to prevent extreme numbers of messages burdening the system
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseGovernor
	{
	private:
		class MouseBehavior *m_pMouseBehavior;
		bool m_bQuit,m_bThreadRunning;
		pluto_pthread_mutex_t m_GovernorMutex;
		pthread_cond_t m_GovernorCond;

		// Pointers to keep track of states
		int m_dwBufferMs; // Number of milliseconds to wait between messages
		unsigned long m_dwTime_Last_SentMessage;
		Message *m_pMessage;

	public:
		MouseGovernor(MouseBehavior *pMouseBehavior);
		virtual ~MouseGovernor();

		void Run(); // Runs in a separate thread
		void SendMessage(Message *pMessage);  // Called when there's a message to send which may need to be blocked
		void DoSendMessage(Message *pMessage);  // Actually send the message
		void SetBuffer(int MS);
		void Purge();
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
