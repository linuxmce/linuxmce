/*
 *  Datagrid_Plugin.cp
 *  Datagrid_Plugin
 *
 *  Created by Serge Wagener on 7/11/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "Datagrid_Plugin.h"
#include "Datagrid_PluginPriv.h"

void Datagrid_Plugin::HelloWorld(const char * s)
{
	 Datagrid_PluginPriv *theObj = new Datagrid_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void Datagrid_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

