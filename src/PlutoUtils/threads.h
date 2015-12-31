/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file threads.h
 Header file for ??? .
 */
#ifndef __THREADS_HPP__
#define __THREADS_HPP__

#include "MultiThreadIncludes.h"

/**
@namespace cpp
Generic namespace for all libraries from PlutoUtils.
*/
namespace cpp {

/**
@namespace Threading
Utilities for threading.
*/
	namespace Threading {

///////////////////////////////////////////////////////////////////////////
//	class SingleThreaded
///////////////////////////////////////////////////////////////////////////

		//	Defines the locking mechanisim for single threaded model
		//	which is actually empty
		template<typename T>
		class SingleThreaded {
		public :
			typedef T VolatileType;

		protected :
			inline explicit SingleThreaded() {}
			inline ~SingleThreaded() {}

		protected :
			//	Perform thread locks
			class LockThread {
			public :
				inline explicit LockThread() { SingleThreaded::Lock(); }
	            inline ~LockThread() { SingleThreaded::Unlock(); }

			private :
				inline explicit LockThread(LockThread const&) {}
				inline LockThread& operator=(LockThread const&) { return *this; }
			};	//	end of class LockThread

		private :
			//	thread synchronization helps
			friend class cpp::Threading::SingleThreaded<T>::LockThread;

			inline static void Lock() {}
			inline static void Unlock() {}

		private :
			inline explicit SingleThreaded(SingleThreaded const&) {}
			inline SingleThreaded& operator=(SingleThreaded const&) { return *this; }
		};	//	end of class SingleThreaded


///////////////////////////////////////////////////////////////////////////
//	class PlutoMultiThreaded
///////////////////////////////////////////////////////////////////////////

		//	Defines the locking mechanisim for single threaded model
		//
		template<typename T>
		class MultiThreaded {
		public :
			typedef T VolatileType;

		protected :
			inline explicit MultiThreaded() { m_Mutex.Init(NULL); }
			inline ~MultiThreaded() { pthread_mutex_destroy(&m_Mutex.mutex);}

		protected :
			//	Perform thread locks
			class LockThread {
			public :
				inline explicit LockThread() { MultiThreaded::Lock(); }
	            inline ~LockThread() { MultiThreaded::Unlock(); }

			private :
				inline explicit LockThread(LockThread const&) {}
				inline LockThread& operator=(LockThread const&) { return *this; }
			};	//	end of class LockThread

		private :
			//	thread synchronization helps
			friend class cpp::Threading::MultiThreaded<T>::LockThread;
			static pluto_pthread_mutex_t m_Mutex;
			//static PlutoLock m_PlutoLock;

			inline static void Lock() {pthread_mutex_lock(&m_Mutex.mutex);}
			inline static void Unlock() {pthread_mutex_unlock(&m_Mutex.mutex);}

		private :
			inline explicit MultiThreaded(MultiThreaded const&) {}
			inline MultiThreaded& operator=(MultiThreaded const&) { return *this; }
		};	//	end of class MultiThreaded

		template<typename T> pluto_pthread_mutex_t MultiThreaded<T>::m_Mutex("mutex");


	};	//	end of namespace Threading

};	//	end of namespace cpp

#endif
