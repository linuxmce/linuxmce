/*
 *  Telecom_Plugin.cp
 *  Telecom_Plugin
 *
 *  Created by Serge Wagener on 7/16/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Telecom_Plugin.h"
#include "Telecom_PluginPriv.h"

void Telecom_Plugin::HelloWorld(const char * s)
{
	 Telecom_PluginPriv *theObj = new Telecom_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Telecom_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

