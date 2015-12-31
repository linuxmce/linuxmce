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
//
// C++ Interface: takenstatemachine
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKTAKENSTATEMACHINE_H
#define ASTERISKTAKENSTATEMACHINE_H

#include "thread.h"
#include "tokenpool.h"
#include "threadsafeobj.h"

namespace ASTERISK {


/**
@author 
*/

template <class T>
class TokenSMPtr {
public:
	TokenSMPtr() throw() {
		p = 0;
	}
	TokenSMPtr(T* lp) throw() {
		p = lp;
		if(p != NULL) {
			p->AddRef();
		}
	}
	TokenSMPtr(const TokenSMPtr<T>& lp) throw() {
		p = lp.p;
		if(p != 0) {
			p->AddRef();
		}
	}
	~TokenSMPtr() throw() {
		if(p) {
			p->Release();
		}
	}
	T* operator->() const throw() {
		return p;
	}

private:
	T* p;
};


template <class T> 
class TokenManager : public TokenPool {
	friend class TokenSMPtr<T>;
protected:
    TokenManager()
			: refs(0) 
	{};
    virtual ~TokenManager() {};

public:
	static TokenSMPtr<T> getInstance() {
			return TokenSMPtr<T>(new T());
	}
	
protected:
	void AddRef() {
		++refs;
	}
	void Release() {
		refs--;
		if(refs == 0) {
			delete this;
		}
	}
	int refs;
};

};

#endif
