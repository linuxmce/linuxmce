/*
 *  Orbiter_Plugin.cp
 *  Orbiter_Plugin
 *
 *  Created by Serge Wagener on 7/10/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Orbiter_Plugin.h"
#include "Orbiter_PluginPriv.h"

void Orbiter_Plugin::HelloWorld(const char * s)
{
	 Orbiter_PluginPriv *theObj = new Orbiter_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Orbiter_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

