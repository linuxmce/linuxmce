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
#include "FadeBlending.h"

#include "SDL.h"

FadeBlending::FadeBlending(Frame* AfterFrame, Frame*BeforeFrame, int StartTime, int TimeToFade)
: FadeBase(AfterFrame, BeforeFrame, StartTime, TimeToFade)
{
}

FadeBlending::~FadeBlending(void)
{
}

bool FadeBlending::Update(int Time)
{
	bool Result = false;
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int ProgressTime = Time - StartTime;
	float Progress = (float) ProgressTime / TimeToFade;
	if (Progress > 1.0f)
	{
		Result = true;
		Progress = 1.0f;
	}

	AfterFrame->SetAlpha(Progress);
	BeforeFrame->SetAlpha(1-Progress);
	return Result;
}
