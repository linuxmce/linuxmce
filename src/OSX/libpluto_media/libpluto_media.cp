/*
 *  libpluto_media.cp
 *  libpluto_media
 *
 *  Created by Serge Wagener on 7/10/12.
 *  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "libpluto_media.h"
#include "libpluto_mediaPriv.h"

void libpluto_media::HelloWorld(const char * s)
{
	 libpluto_mediaPriv *theObj = new libpluto_mediaPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void libpluto_mediaPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

