/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Interface: XinePlayer
//
// Description: 
//
//
// Author: Radu Leca <radu@radul>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef XinePlayer_h
#define XinePlayer_h

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <xine.h>
#include <xine/xineutils.h>

#include  <list>
#include <string>

#include "XineOsdObject.h"
#include "XineStream.h"

class XineOsdObject;
using namespace std;

class XinePlayer
{
private:
	xine_t              *m_Xine;
	//XineStream          *m_Stream;
	xine_stream_t       *m_Stream;
	xine_video_port_t   *m_VideoPort;
	xine_audio_port_t   *m_AudioPort;
	xine_event_queue_t  *m_EventQueue;
	
	list<XineOsdObject *> m_OSDList;
	
	string             m_VideoDriverName, m_AudioDriverName;
	string             m_ActualMrl;
	
	int                m_nLengthTime;   //in milisecond

	Display             *m_Display;
	Window               m_Window;
	x11_visual_t         m_Visual;
	
	int                  m_nScreen;
	int                  m_nXPos, m_nYPos, m_nWidth, m_nHeight;
	double               m_dPixelAspect;
	int                  m_nRunning;
	
	xine_post_t *m_pPostPlugin;
public:
	XinePlayer();
	~XinePlayer();
	
	// Post init of xine	
	bool SetOutput(Display *display,Window *window,int nScreen);

	bool LoadPostPlugin(string pluginName);
	bool SetPluginParam(string name,string value);
	
	void GetInfo();

	//Osd function	

	// video function
	bool Open(char mrl[]);
	bool Play();
	void Pause();
	void Resume();
	
	void JumpAtPosition( int nSecond );
	void ChangePlaybackSpeed(int nSpeed );
	
	//audio function
	void SetAudioVolume(int nVolume);	// 0-100
	void SetAudioMute();
	int  GetAudioVolume();
	
	//xine message
	void XineEvent(void *user_data, const xine_event_t *event);
	void OnResize(void *data, int video_width, int video_height, double video_pixel_aspect,
			 int *dest_width, int *dest_height, double *dest_pixel_aspect);
	void FrameOutputCb(void *data, int video_width, int video_height,
			    double video_pixel_aspect, int *dest_x, int *dest_y,
			    int *dest_width, int *dest_height, 
			    double *dest_pixel_aspect, int *win_x, int *win_y);
				
				
	friend void XineEvent2(void *user_data, const xine_event_t *event);

	friend void OnResize2(void *data, int video_width, int video_height, double video_pixel_aspect,
			 int *dest_width, int *dest_height, double *dest_pixel_aspect);
	friend void FrameOutputCb2(void *data, int video_width, int video_height,
			    double video_pixel_aspect, int *dest_x, int *dest_y,
			    int *dest_width, int *dest_height, 
			    double *dest_pixel_aspect, int *win_x, int *win_y);

};




#endif

