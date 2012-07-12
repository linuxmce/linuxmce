/*
 *  File_Grids_Plugin.cp
 *  File_Grids_Plugin
 *
 *  Created by Serge Wagener on 7/12/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "File_Grids_Plugin.h"
#include "File_Grids_PluginPriv.h"

void File_Grids_Plugin::HelloWorld(const char * s)
{
	 File_Grids_PluginPriv *theObj = new File_Grids_PluginPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void File_Grids_PluginPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

