/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file TestRubyEmbederCodeSupplier.h
Header for ?
*/
#ifndef EMBRUBYTESTRUBYEMBEDERCODESUPPLIER_H
#define EMBRUBYTESTRUBYEMBEDERCODESUPPLIER_H

#include "RubyEmbeder.h"

/** @namespace EMBRUBY
Embedded Ruby ???
*/
namespace EMBRUBY {

/** @class TestRubyEmbederCodeSupplier
Test cass ?
*/
class TestRubyEmbederCodeSupplier : public RubyEmbederCodeSupplier {
public:

    /** Get the ruby code.
    */
	const char* getRubyCode();
};

};

#endif
