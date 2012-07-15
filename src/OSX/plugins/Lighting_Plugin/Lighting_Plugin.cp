/*
 *  Lighting_Plugin.cp
 *  Lighting_Plugin
 *
 *  Created by Serge Wagener on 7/15/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Lighting_Plugin.h"
#include "Lighting_PluginPriv.h"

void Lighting_Plugin::HelloWorld(const char * s)
{
	 Lighting_PluginPriv *theObj = new Lighting_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Lighting_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

