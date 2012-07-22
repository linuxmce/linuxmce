/*
 *  libpluto_telecom.cp
 *  libpluto_telecom
 *
 *  Created by Serge Wagener on 7/16/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "libpluto_telecom.h"
#include "libpluto_telecomPriv.h"

void libpluto_telecom::HelloWorld(const char * s)
{
	 libpluto_telecomPriv *theObj = new libpluto_telecomPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void libpluto_telecomPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

