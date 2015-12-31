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
#include "FadeFactory.h"

#include "FadeBlending.h"

FadeFactory* FadeFactory::Instance_ = NULL;

FadeFactory* FadeFactory::Instance(void)
{
	if(NULL == Instance_)
		Instance_ = new FadeFactory();
	return Instance_;
}

FadeFactory::FadeFactory(void)
{
}

FadeFactory::~FadeFactory(void)
{
}


FadeBase* FadeFactory::CreateFadeMode(int Mode, MeshFrame* AfterFrame, MeshFrame*BeforeFrame, 
										int StartTime, int TimeToFade)
{
	return new FadeBlending(AfterFrame, BeforeFrame, StartTime, TimeToFade);
}