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
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef XinePlayer_H_
#define XinePlayer_H_

#include <xine.h>
#include <xine/xineutils.h>

#include <pthread.h>

#include <string>

class XinePlayer
{

	xine_t              *xine;
	xine_stream_t       *stream;
	xine_audio_port_t   *ao_port;
	xine_event_queue_t  *event_queue;
	std::string FileName;

	pthread_t tid;
	pthread_mutex_t lockmutex;
	XinePlayer();

public:
	static XinePlayer* Instance;

	bool NeedToReplay;
	bool Running;

	static XinePlayer* GetInstance();

	~XinePlayer();

	void InitPlayerEngine(std::string ConfigName, std::string FileName);
	void StopPlayerEngine();

	bool StartPlayingFile();

	std::string GetFileName();
	void Replay();
};

void XineEventFunction(void *XinePlayerPtr, const xine_event_t *XineEvent);
void* XinePlayerThread(void* XinePlayerPtr);

#endif
