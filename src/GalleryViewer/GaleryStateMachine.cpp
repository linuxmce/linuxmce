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
#include "GaleryStateMachine.h"

GaleryStateMachine::GaleryStateMachine(void)
	: Status(STATUS_STARTUP)
{
}

GaleryStateMachine::~GaleryStateMachine(void)
{
}

int GaleryStateMachine::GetStatus(void)
{
	return Status;
}

void GaleryStateMachine::LoadedImages(void)
{
	Status = STATUS_LOADED_IMAGES;
}

void GaleryStateMachine::AnimateCurrentImage(void)
{
	Status = STATUS_ANIMATE_CURRENT_IMAGE;
}

void GaleryStateMachine::FadeImages(void)
{
	Status = STATUS_FADE_IMAGES;
}
