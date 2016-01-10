/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "XineStream.h"

XineStream::XineStream()
{ 
	m_pStream = NULL;
}

XineStream::XineStream(xine_stream_t* pStream)
{ 
	m_pStream = pStream; 
}

XineStream::~XineStream()
{
	if( m_pStream );
	// dispose here the stream?
}

void XineStream::operator =(xine_stream_t *pStream)
{
	if( !m_pStream )
		m_pStream = pStream;
}

void  XineStream::ReadInfo()
{
	if( !m_pStream )
		return;
	
	BitRate         =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_BITRATE           );
	SeekAble        =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_SEEKABLE          );
	VideoWidth      =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_WIDTH       );
	VideoHeight     =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_HEIGHT      );
	VideoRatio      =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_RATIO       );
	VideoChannels   =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_CHANNELS    );
	VideoStreams    =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_STREAMS     );
	VideoBitRate    =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_BITRATE     );
	VideoFOURCC     =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_FOURCC      );
	VideoHandled    =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_VIDEO_HANDLED     );
	FrameDuration   =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_FRAME_DURATION    );
	AudioChanenels  =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_AUDIO_CHANNELS    );
	AudioBits       =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_AUDIO_BITS        );
	AudioSampleRate =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_AUDIO_SAMPLERATE  );
	AudioBitRate    =  xine_get_stream_info( m_pStream, XINE_STREAM_INFO_AUDIO_BITRATE     );
}


void XineStream::ScheduleOsdObject(XineOsdObject *pOsd, int timeOffsetShow, int timeOffsetHide)
{
	if( !pOsd || !m_pStream )
		return;
		
	xine_osd_show( *pOsd, timeOffsetShow);
	if (timeOffsetHide!=-1)
		xine_osd_hide(	*pOsd, timeOffsetHide);
}
