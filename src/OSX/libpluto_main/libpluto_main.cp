/*
 *  libpluto_main.cp
 *  libpluto_main
 *
 *  Created by Serge Wagener on 7/8/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "libpluto_main.h"
#include "libpluto_mainPriv.h"

void libpluto_main::HelloWorld(const char * s)
{
	 libpluto_mainPriv *theObj = new libpluto_mainPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void libpluto_mainPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

