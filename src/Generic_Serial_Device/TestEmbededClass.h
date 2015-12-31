/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file TestEmbededClass.h
Class ?
*/
#ifndef EMBRUBYTESTEMBEDEDCLASS_H
#define EMBRUBYTESTEMBEDEDCLASS_H


#include "RubyEmbededClass.h"

/** @namespace EMBRUBY
Embedded Ruby ???
*/
namespace EMBRUBY {

/** @class TestEmbededClass
For ?
*/
class TestEmbededClass : public RubyEmbededClassImpl<TestEmbededClass> {
public:
	DEFINE_CLASS_NAME("Test");

	TestEmbededClass();
    ~TestEmbededClass();

public:
	void TestMtd();
	void On();
};

};

#endif
