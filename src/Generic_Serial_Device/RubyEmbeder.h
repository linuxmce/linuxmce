/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyEmbeder.h
Load ruby code files.
*/
#ifndef RUBYEMBEDER_H
#define RUBYEMBEDER_H

#include "RubyExceptions.h"

#include <ruby.h>

/** @namespace EMBRUBY
Embedded Ruby ???
*/
namespace EMBRUBY {

class RubyEmbederCodeSupplier;

/** @class RubyEmbeder
Base class for ?
*/
class RubyEmbeder {
public:
    RubyEmbeder();
    ~RubyEmbeder();

public:
	void loadCode(RubyEmbederCodeSupplier *psup) throw(RubyException);

public:
	static RubyEmbeder* getInstance();

private:
	static VALUE _loadcode(VALUE arg);

private:
	static RubyEmbeder* s_instance_;

private:
	std::string addLineNumbers(std::string str);
};


/** @class RubyEmbederCodeSupplier
Base class for ?
*/
class RubyEmbederCodeSupplier {
	friend class RubyEmbeder;
protected:
    /** pure virtual to get ruby code.
    */
	virtual const char* getRubyCode() = 0;
};

}

#endif
