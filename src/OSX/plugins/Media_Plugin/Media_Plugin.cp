/*
 *  Media_Plugin.cp
 *  Media_Plugin
 *
 *  Created by Serge Wagener on 7/16/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Media_Plugin.h"
#include "Media_PluginPriv.h"

void Media_Plugin::HelloWorld(const char * s)
{
	 Media_PluginPriv *theObj = new Media_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Media_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

