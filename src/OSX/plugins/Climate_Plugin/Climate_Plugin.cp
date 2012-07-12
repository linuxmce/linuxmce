/*
 *  Climate_Plugin.cp
 *  Climate_Plugin
 *
 *  Created by Serge Wagener on 7/12/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Climate_Plugin.h"
#include "Climate_PluginPriv.h"

void Climate_Plugin::HelloWorld(const char * s)
{
	 Climate_PluginPriv *theObj = new Climate_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Climate_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

