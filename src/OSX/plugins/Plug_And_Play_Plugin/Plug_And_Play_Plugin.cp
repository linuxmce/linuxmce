/*
 *  Plug_And_Play_Plugin.cp
 *  Plug_And_Play_Plugin
 *
 *  Created by Serge Wagener on 7/18/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Plug_And_Play_Plugin.h"
#include "Plug_And_Play_PluginPriv.h"

void Plug_And_Play_Plugin::HelloWorld(const char * s)
{
	 Plug_And_Play_PluginPriv *theObj = new Plug_And_Play_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Plug_And_Play_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

