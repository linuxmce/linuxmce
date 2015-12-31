/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef  XineStream_h
#define  XineStream_h

#include <xine.h>

#include "XineOsdObject.h"

class XineStream
{	
private:
	xine_stream_t *m_pStream;
public:
	int BitRate;           
	int SeekAble;          
	int VideoWidth;       
	int VideoHeight;      
	int VideoRatio;     
	int VideoChannels;    
	int VideoStreams;     
	int VideoBitRate;     
	int VideoFOURCC;      
	int VideoHandled;    
	int FrameDuration;    
	int AudioChanenels;    
	int AudioBits;        
	int AudioSampleRate;  
	int AudioBitRate;     
	
	XineStream();
	XineStream(xine_stream_t* pStream);
	~XineStream();
	
	void ReadInfo();
	
	operator xine_stream_t*()
	{
		return m_pStream;
	}
	
	void operator =(xine_stream_t *t);
	
	void ScheduleOsdObject(XineOsdObject *pOsd, int timeOffsetShow, int timeOffsetHide=-1);
};


#endif

