/*
 *  Security_Plugin.cp
 *  Security_Plugin
 *
 *  Created by Serge Wagener on 7/16/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Security_Plugin.h"
#include "Security_PluginPriv.h"

void Security_Plugin::HelloWorld(const char * s)
{
	 Security_PluginPriv *theObj = new Security_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Security_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

