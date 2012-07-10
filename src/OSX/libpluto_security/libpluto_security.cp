/*
 *  libpluto_security.cp
 *  libpluto_security
 *
 *  Created by Serge Wagener on 7/10/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "libpluto_security.h"
#include "libpluto_securityPriv.h"

void libpluto_security::HelloWorld(const char * s)
{
	 libpluto_securityPriv *theObj = new libpluto_securityPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void libpluto_securityPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

