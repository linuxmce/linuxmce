/*
 *  libPlutoUtils.cp
 *  libPlutoUtils
 *
 *  Created by Serge Wagener on 7/15/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "libPlutoUtils.h"
#include "libPlutoUtilsPriv.h"

void libPlutoUtils::HelloWorld(const char * s)
{
	 libPlutoUtilsPriv *theObj = new libPlutoUtilsPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void libPlutoUtilsPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

