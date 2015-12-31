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
#ifndef __MouseIterator_H__
#define __MouseIterator_H__
//-----------------------------------------------------------------------------------------------------

#include "PlutoUtils/MultiThreadIncludes.h"

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Limits mouse movements to only 1 move per x ms to prevent extreme numbers of messages burdening the system
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseIterator
	{
	private:
		friend class MouseBehavior;
		friend class VolumeMouseHandler;
		friend class LightMouseHandler;
		friend class MediaMouseHandler;
		friend class KeyboardMouseHandler;
		friend class DatagridMouseHandlerHelper;
		friend class MediaBrowserMouseHandler;

		class MouseBehavior *m_pMouseBehavior;
		class MouseHandler *m_pMouseHandler;
		class Orbiter *m_pOrbiter;
		bool m_bQuit,m_bThreadRunning;
		pluto_pthread_mutex_t m_IteratorMutex;
		pthread_cond_t m_IteratorCond;

		// Pointers to keep track of states
		enum EIteratorFunction { if_None, if_Volume, if_Light, if_MediaTracks, if_Keyboard, if_MediaGrid } m_EIteratorFunction;
		int m_dwParm; // Parm for the function
		int m_dwFrequency_Ms;  // Frequency for doing the iteration in milliseconds
		int m_dwTime_Last_Iterated;  // When we last did our iteration
		string m_sParm;

	public:
		MouseIterator(MouseBehavior *pMouseBehavior);
		virtual ~MouseIterator();

		void Run(); // Runs in a separate thread
		void SetIterator(EIteratorFunction eIteratorFunction,int dwParm,string sParm,int dwFrequency_Ms,MouseHandler *pMouseHandler);
		void DoIteration();
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
