/*
 *  Event_Plugin.cp
 *  Event_Plugin
 *
 *  Created by Serge Wagener on 7/9/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Event_Plugin.h"
#include "Event_PluginPriv.h"

void Event_Plugin::HelloWorld(const char * s)
{
	 Event_PluginPriv *theObj = new Event_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Event_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

