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
#ifndef FadeBase_H_
#define FadeBase_H_

#include "Frame.h"

class FadeBase
{
protected:
	int StartTime;
	int TimeToFade;
	Frame* AfterFrame;
	Frame*BeforeFrame;
public:
	FadeBase(Frame* AfterFrame, Frame*BeforeFrame, int StartTime, int TimeToFade);
	virtual bool Update(int Time) = 0;
	virtual ~FadeBase(void);
};

#endif

