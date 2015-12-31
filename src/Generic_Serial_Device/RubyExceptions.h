/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyExceptions.h
For ?
*/
#ifndef EMBRUBYRUBYEXCEPTIONS_H
#define EMBRUBYRUBYEXCEPTIONS_H

#include <string>

/** @namespace EMBRUBY
Embedded Ruby ???
*/
namespace EMBRUBY {

/** @class RubyException
For ???
*/
class RubyException {
public:
    /** Constructor.
    */
	RubyException();

    /** Constructor.
    @param msg is 'C' style string message.
    */
    RubyException(const char* msg);

    /** Constructor.
    @param msg is the 'C++' style string message.
    */
    RubyException(const std::string& msg);

    /** Destructor.
    */
    ~RubyException();

public:

    /** @Returns the message.
    */
	const char* getMessage() {
		return msg_.c_str();
	}

private:
	std::string msg_; /**< Exception message storage area. */
};

typedef RubyException* RubyExceptionPtr;

};

#endif
