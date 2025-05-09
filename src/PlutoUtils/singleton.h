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
 * @file singleton.h
 Header file for ??? .
 */
#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

#include "typetraits.h"
#include "threads.h"


/**
@namespace cpp
Generic namespace for all libraries from PlutoUtils.
*/
namespace cpp {

///////////////////////////////////////////////////////////////////////////
//	class NoCreation
///////////////////////////////////////////////////////////////////////////

    /** @class NoCreation
    Disables creation of object.
    */
	template<typename T>
	class NoCreation {
	protected :
		inline explicit NoCreation() {}
		inline ~NoCreation() {}

		//	creation/destruction/storage issues
		inline static T* CreateInstance() { throw std::logic_error("NoCreation::CreateInstance called"); }
		inline static void DestroyInstance(T*) { throw std::logic_error("NoCreation::DestroyInstance called"); }

	private :
		inline explicit NoCreation(NoCreation const&) {}
		inline NoCreation& operator=(NoCreation const&) { return *this; }
	};	//	end of class NoCreation

///////////////////////////////////////////////////////////////////////////
//	class CreateUsingNew
///////////////////////////////////////////////////////////////////////////

    /** @class CreateUsingNew
	 Creates the object with a normal new invocation
    */
	template<typename T>
	class CreateUsingNew {
	protected :
		inline explicit CreateUsingNew() {}
		inline ~CreateUsingNew() {}

		//	creation/destruction/storage issues
		inline static T* CreateInstance() { return new T(); }
		inline static void DestroyInstance(T* t) { delete t; }

	private :
		inline explicit CreateUsingNew(CreateUsingNew const&) {}
		inline CreateUsingNew& operator=(CreateUsingNew const&) { return *this; }
	};	//	end of class CreateUsingNew

///////////////////////////////////////////////////////////////////////////
//	class CreateUsingStatic
///////////////////////////////////////////////////////////////////////////

    /** @class CreateUsingStatic
	 Creates the object with a static memory allocation but not static lifespan.
    */
	template<typename T>
	class CreateUsingStatic {
	protected :
		inline explicit CreateUsingStatic() {}
		inline ~CreateUsingStatic() {}

		//	creation/destruction/storage issues
		inline static T* CreateInstance() {
			new(t_) T();
			return reinterpret_cast<T*>(t_);
		}
		inline static void DestroyInstance(T* t) { t->~T(); }

	private :
		static unsigned char t_[];

	private :
		inline explicit CreateUsingStatic(CreateUsingStatic const&) {}
		inline CreateUsingStatic& operator=(CreateUsingStatic const&) { return *this; }
	};	//	end of class CreateUsingStatic

	template<typename T>
	unsigned char CreateUsingStatic<T>::t_[sizeof(T)];

///////////////////////////////////////////////////////////////////////////
//	class DefaultLifetime
///////////////////////////////////////////////////////////////////////////

    /** @class DefaultLifetime
		dictates a default/normal life time of LIFO destruction.
		uses atexit.
		dead reference throws an exception.
    */
	template<typename T>
	class DefaultLifetime {
	protected :
		inline explicit DefaultLifetime() {}
		inline ~DefaultLifetime() {}

		//	dead reference issues
		inline static void OnDeadReference() { throw string("Dead Reference Detected"); }
		inline static void ScheduleForDestruction(void (*pFun)()) { std::atexit(pFun); }

	private :
		inline explicit DefaultLifetime(DefaultLifetime const&) {}
		inline DefaultLifetime& operator=(DefaultLifetime const&) { return *this; }
	};	//	end of class DefaultLifetime

///////////////////////////////////////////////////////////////////////////
//	class PhoenixSingleton
///////////////////////////////////////////////////////////////////////////

    /** @class PhoenixSingleton
	 Allows recurring singleton.
    */
	template<typename T>
	class PhoenixSingleton {
	protected :
		inline explicit PhoenixSingleton() {}
		inline ~PhoenixSingleton() {}

		//	dead reference issues
		inline static void OnDeadReference() {}
		inline static void ScheduleForDestruction(void (*pFun)()) { std::atexit(pFun); }

	private :
		inline explicit PhoenixSingleton(PhoenixSingleton const&) {}
		inline PhoenixSingleton& operator=(PhoenixSingleton const&) { return *this; }
	};	//	end of class PhoenixSingleton

///////////////////////////////////////////////////////////////////////////
//	class PhoenixSingleton
///////////////////////////////////////////////////////////////////////////

    /** @class NoDestruction
	 No destruction calls, unlimited lifetime.
    */
	template<typename T>
	class NoDestruction {
	protected :
		inline explicit NoDestruction() {}
		inline ~NoDestruction() {}

		//	dead reference issues
		inline static void OnDeadReference() {}
		inline static void ScheduleForDestruction(void (*)()) {}

	private :
		inline explicit NoDestruction(NoDestruction const&) {}
		inline NoDestruction& operator=(NoDestruction const&) { return *this; }
	};	//	end of class NoDestruction

///////////////////////////////////////////////////////////////////////////
//	class Singleton
///////////////////////////////////////////////////////////////////////////

    /** @class Singleton
	Gives a class a singleton property
	protected access of ctor and dtor allows creation of subclass of singleton
	to acheieve the effect of YourSingleton::Instance()
	extends the typetraits just to take advantage of the basic typedefs
    */
	template<typename T, typename CreationPolicy = CreateUsingNew<T>, template <typename> class LifetimePolicy = DefaultLifetime, template <typename> class ThreadingModel = cpp::Threading::SingleThreaded>
	class Singleton : public cpp::Traits::TypeTraits<typename ThreadingModel<T>::VolatileType>, public CreationPolicy, public LifetimePolicy<T>, public ThreadingModel<T> {
	public :
		//	grabs the singleton's instance, if any
		//	otherwise creates an instance
		static typename cpp::Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::RefType Instance();
		//	performs cleanup on the singleton
		static void Destroy();
		//	resets the singleton instance with a new user created instance
		static void Reset(typename cpp::Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::PointerType, void (*pFun)(T*));

	protected :
		inline explicit Singleton() { Singleton::instance_ = static_cast<typename cpp::Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::PointerType>(this); Singleton::destroyed_ = false; Singleton::pFun_ = 0; }
		inline ~Singleton() { Singleton::instance_ = 0; Singleton::destroyed_ = true; Singleton::pFun_ = 0; }

	private :
		typedef void (*UserSuppliedDestroy)(T*);

	private :
		static typename cpp::Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::PointerType instance_;
		static bool destroyed_;
		static UserSuppliedDestroy pFun_;

	private :
		inline explicit Singleton(Singleton const&) {}
		inline Singleton& operator=(Singleton const&) { return *this; }
	};	//	end of class Singleton

	template<typename T, typename CreationPolicy, template <typename> class LifetimePolicy, template <typename> class ThreadingModel>
	typename Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::RefType Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::Instance() {
		if ( Singleton::instance_ == 0 ) {
			class LockThread lock;
			if ( Singleton::instance_ == 0 ) {
				Singleton::pFun_ = 0;
				if ( Singleton::destroyed_ ) {
					OnDeadReference();
					Singleton::destroyed_ = false;
				}
				Singleton::instance_ = CreateInstance();
				try {
					ScheduleForDestruction(Singleton::Destroy);
				} catch(...) {
					DestroyInstance(Singleton::instance_);
				}
			}
		}
		return *(Singleton::instance_);
	}

	template<typename T, typename CreationPolicy, template <typename> class LifetimePolicy, template <typename> class ThreadingModel>
	void Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::Destroy() {
		if ( Singleton::instance_ != 0 ) {
			class LockThread lock;
			if ( Singleton::instance_ != 0 ) {
				if (  Singleton::pFun_ != 0 ) {
					Singleton::pFun_(Singleton::instance_);
				} else {
					DestroyInstance(Singleton::instance_);
				}
				Singleton::instance_ = 0;
				Singleton::destroyed_ = true;
			}
		}
	}

	template<typename T, typename CreationPolicy, template <typename> class LifetimePolicy, template <typename> class ThreadingModel>
	void Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::Reset(typename Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::PointerType p, UserSuppliedDestroy pFun) {
		class LockThread lock;
		if ( Singleton::instance_ != 0 ) {
			DestroyInstance(Singleton::instance_);
		} else if ( p != 0 ) {
			ScheduleForDestruction(Singleton::Destroy);
		}
		Singleton::instance_ = p;
		Singleton::pFun_ = pFun;
	}

	template<typename T, typename CreationPolicy, template <typename> class LifetimePolicy, template <typename> class ThreadingModel>
	typename Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::PointerType Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::instance_ = 0;
	template<typename T, typename CreationPolicy, template <typename> class LifetimePolicy, template <typename> class ThreadingModel>
	bool Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::destroyed_ = false;
	template<typename T, typename CreationPolicy, template <typename> class LifetimePolicy, template <typename> class ThreadingModel>
	typename Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::UserSuppliedDestroy Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::pFun_ = 0;

};	//	end of namespace cpp

#endif
