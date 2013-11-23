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
// C++ Implementation: XinePlayer 
//
// Description: XinePlayer - a minimalist sound player
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "XinePlayer.h"

#include <stdio.h>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

#include "SafetyLock.h"

/*static*/ XinePlayer* XinePlayer::Instance = NULL;

XinePlayer::XinePlayer()
	: xine(NULL),
	  stream(NULL),
	  ao_port(NULL),
	  event_queue(NULL),
	  tid(0)
{
#ifdef DEBUG
	std::cout<<"XinePlayer: Starting player!"<<std::endl;
#endif
	NeedToReplay = false;
	this->Running = false;
	pthread_mutex_init(&lockmutex, NULL);
}

XinePlayer* XinePlayer::GetInstance()
{
	if (Instance == NULL)
		Instance = new XinePlayer();
	return Instance;
}

XinePlayer::~XinePlayer()
{
}

void XinePlayer::InitPlayerEngine(std::string ConfigName, std::string FileName)
{
#ifdef DEBUG
	std::cout<<"XinePlayer: InitPlayerEngine() with Xine config: "<<ConfigName<<" and filename " <<FileName<<std::endl;
#endif

	FILE* f = fopen(FileName.c_str(), "r");
	if(!f)
	{
		std::cout<<"Warning, no file on disk to play: "<<FileName<<std::endl;
		return;
	}
	else
		fclose(f);

	{
		SafetyLock Lock(&lockmutex);
		Running = true;
		
		xine = xine_new();
		std::cout << FileName <<std::endl;
		xine_config_load(xine, ConfigName.c_str());
		xine_init(xine);
		ao_port     = xine_open_audio_driver(xine , "auto", NULL);
		stream      = xine_stream_new(xine, ao_port, NULL);
		event_queue = xine_event_new_queue(stream);
		xine_event_create_listener_thread(event_queue, XineEventFunction, this);

		this->FileName = FileName;
	}


	if(!StartPlayingFile())
		return;
	if(tid == 0)
		pthread_create(&tid, NULL, XinePlayerThread,this);
	
}

void XinePlayer::StopPlayerEngine()
{
	if(!xine)
		return;

#ifdef DEBUG
	std::cout<<"XinePlayer: StopPlayerEngine()"<<std::endl;
#endif

	Running = false;
	pthread_join(tid, NULL);
	tid = 0;

	SafetyLock Lock(&lockmutex);
	
	xine_stop(stream);
	xine_close(stream);
	xine_event_dispose_queue(event_queue);
	xine_dispose(stream);
	if(ao_port)
		xine_close_audio_driver(xine, ao_port);  
	xine_exit(xine);
	xine = NULL;
}

bool XinePlayer::StartPlayingFile()
{
	SafetyLock Lock(&lockmutex);

	xine_close(stream);

#ifdef DEBUG
	std::cout<<"XinePlayer: StartPlayingFile()"<<std::endl;
#endif
	NeedToReplay = false;
	if((!xine_open(stream, FileName.c_str())) || (!xine_play(stream, 0, 0))) 
	{
		std::cout << "Unable to open file: " <<  FileName << std::endl;

		return false;
	}
	xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, 100);

	std::cout<<"XinePlayer: Playing: "<<FileName<<std::endl;

	return true;
}

void XinePlayer::Replay()
{
#ifdef DEBUG
	std::cout<<"XinePlayer: Replay()"<<std::endl;
#endif

	this->NeedToReplay = true;
}

std::string XinePlayer::GetFileName()
{
	return FileName;
}

void XineEventFunction(void *XinePlayerPtr, const xine_event_t *XineEvent)
{
	XinePlayer* Player = (XinePlayer*)XinePlayerPtr;

#ifdef DEBUG
	std::cout<<"XineEvent"<<std::endl;
#endif
	switch(XineEvent->type) { 
	case XINE_EVENT_UI_PLAYBACK_FINISHED:
#ifdef DEBUG
	std::cout<<"XinePlayer: Playback finished()"<<std::endl;
#endif
		Player->Replay();
		break;
	}
}

void* XinePlayerThread(void* XinePlayerPtr)
{
	XinePlayer* Player = (XinePlayer*)XinePlayerPtr;

	while(Player->Running) 
	{
		usleep(10000);
		if (Player->NeedToReplay)
		{
			Player->StartPlayingFile();
		}
	}

	//Player->StopPlayerEngine();
	return NULL;
}
