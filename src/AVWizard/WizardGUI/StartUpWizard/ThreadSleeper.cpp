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
//
// C++ Implementation: ThreadSleeper
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@ciplogic-desktop>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ThreadSleeper.h"

#include "GUIWizardUtils.h"

#include "WizardWidgetLabel.h"
#include "Wizard.h"
#include "SafetyLock.h"

#include <SDL.h>
#include <iostream>

void* SleeperThreadFunc(void* Instance);

ThreadSleeper::ThreadSleeper()
: Label(NULL)
{
	tid = 0;
	pthread_mutex_init(&LockMutex, NULL);
}

void ThreadSleeper::Init(int NoSeconds, bool RefreshEvent /*=true*/)
{
	std::cout << "1 tid: " << tid << std::endl;
	m_RefreshEvent = RefreshEvent;

	if(tid != 0)
	{
		std::cout<<"ThreadSleeper::Init-> Race condition,should not happend"<<std::endl;
		return;
	}
	this->TickRemaining = NoSeconds*1000;
	std::cout<<"ThreadSleeper::Init"<<std::endl;
	LastTime = SDL_GetTicks();
	bQuit = false;
	pthread_create(&tid, NULL, SleeperThreadFunc,this);

	std::cout << "2 tid: " << tid << std::endl;
}

int ThreadSleeper::GetSecondRemaining()
{
	SafetyLock Lock(&LockMutex);
	int CurrentTime = SDL_GetTicks();
	TickRemaining = TickRemaining - (CurrentTime - LastTime);
	LastTime = CurrentTime;
	int Result = 0;
	if ((TickRemaining + 999) / 1000 > 0)
		Result = (TickRemaining + 999) / 1000;
	return Result;
}

void ThreadSleeper::Quit()
{
	bQuit = true;

	if(tid)
		pthread_join(tid, NULL);
	tid = 0;
}

void ThreadSleeper::SecondTick()
{
#ifdef DEBUG
	//std::cout<< "VideoResolutionSecondSleeper::SecondTick()" << std::endl;
#endif
	if(!Label)
	{
		std::cout<<"VideoResolutionSecondSleeper::SecondTick Warning! No label = nothing to draw";
		return;
	}
	int Seconds = GetSecondRemaining();

	std::string LabelCaption = Utils::Int32ToString(Seconds);
	{
		SafetyLock Lock(&LockMutex);
		Label->SetCaption(LabelCaption);
	}
	WM_Event Event;
	std::cout<<"Seconds: "<<Seconds<<std::endl;
	if (Seconds > 0 && m_RefreshEvent)
		Event.DownKey();
	Wizard::GetInstance()->GenerateCustomEvent(Event);
}

/*virtual*/ void ThreadSleeper::SetLabel(WizardWidgetLabel* Label)
{
	SafetyLock Lock(&LockMutex);
	this->Label = Label;
}

ThreadSleeper* ThreadSleeper::Instance_ = NULL;
ThreadSleeper* ThreadSleeper::Instance()
{
	if(!Instance_)
		Instance_ = new ThreadSleeper();
	return Instance_;
}

void* SleeperThreadFunc(void* Instance)
{
	ThreadSleeper* ThreadPtr= (ThreadSleeper*) Instance;
	ThreadPtr->SecondTick();
	int Seconds1 = ThreadPtr->GetSecondRemaining();
	std::cout<<"SleeperThreadFunc..."<<std::endl;

	int Remaining;
	while(!ThreadPtr->bQuit && (Remaining = ThreadPtr->GetSecondRemaining()) > 0)
	{
		wizSleep(50);
		int Seconds2 = Remaining;

		if (Seconds2 != Seconds1)
		{
			Seconds1 = Seconds2;
			ThreadPtr->SecondTick();
		}
	}

	std::cout << "Countdown remaining: " << Remaining << std::endl;
	if (Remaining <= 0)
	{
		WM_Event Event;
		Event.EscapeKey();
		Wizard::GetInstance()->GenerateCustomEvent(Event);
	}

	std::cout<<"ThreadQuit"<<std::endl;
	ThreadPtr->Quit();
	return NULL;
}

