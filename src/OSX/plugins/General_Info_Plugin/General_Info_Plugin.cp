/*
 *  General_Info_Plugin.cp
 *  General_Info_Plugin
 *
 *  Created by Serge Wagener on 7/12/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "General_Info_Plugin.h"
#include "General_Info_PluginPriv.h"

void General_Info_Plugin::HelloWorld(const char * s)
{
	 General_Info_PluginPriv *theObj = new General_Info_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void General_Info_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

