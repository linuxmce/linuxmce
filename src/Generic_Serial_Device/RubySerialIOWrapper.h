/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubySerialIOWrapper.h
For ?
*/
#ifndef DCERUBYSERIALIOWRAPPER_H
#define DCERUBYSERIALIOWRAPPER_H

#include "RubySerialWrapper.h"
#include <list>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class RubySerialIOWrapper
For ?
*/
class RubySerialIOWrapper : public RubySerialWrapper {
	friend class RubySerialIOPool;
public:
    RubySerialIOWrapper();
    ~RubySerialIOWrapper();

	/*accessed from our code*/
public:
	void setConn(const RubySerialIOConnectionWrapper& conn) {
		conn_ = conn;
	}

	RubySerialIOConnectionWrapper& getConn() {
		return conn_;
	}

	/*accessed from ruby code*/
public:
	RubySerialIOConnectionWrapper conn_; /*connection*/
};

};

#endif
