/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "BackEndFactory.h"

#include <stdlib.h>

#include "SDLFrontEnd.h"
#include "OpenGLFrontEnd.h"

/*static*/ BackEndFactory* BackEndFactory::Instance = NULL;

BackEndFactory::BackEndFactory(void)
{
	CurrentBackEnd = NULL;
}

BackEndFactory::~BackEndFactory()
{
}


BackEndFactory* BackEndFactory::GetInstance()
{
	if(!Instance)
		Instance = new BackEndFactory();
	return Instance;
}

void BackEndFactory::CleanUp()
{
	delete Instance;
	Instance = NULL;
}

GenericBackEnd* BackEndFactory::CreateBackEnd(int Type)
{
	switch(Type) {
	case Default:
		return new OpenGLFrontEnd();
	case Simple:
		return new SDLFrontEnd();
		break;
	}
	return new SDLFrontEnd();
}
