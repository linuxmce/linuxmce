/*
 *  libDCECommon.cp
 *  libDCECommon
 *
 *  Created by Serge Wagener on 11/6/11.
 *  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "libDCECommon.h"
#include "libDCECommonPriv.h"

void libDCECommon::HelloWorld(const char * s)
{
	 libDCECommonPriv *theObj = new libDCECommonPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void libDCECommonPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

