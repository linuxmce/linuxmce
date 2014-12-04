/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include <math.h>

#include <xine.h>
#include <xine/xineutils.h>
#include <xine/video_out.h>
#include <xine/audio_out.h>

#include "DCE/Logger.h"

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// watchdog
namespace DCE
{
extern	int  (*custom_xine_seek) (xine_stream_t *stream, int start_pos, int start_time);
extern	int  (*custom_xine_start_trick_play)(xine_stream_t *stream, int trick_speed);
extern	int  (*custom_xine_stop_trick_play)(xine_stream_t *stream);
extern	bool g_bXINE_HAS_TRICKPLAY_SUPPORT;
}

bool bStreamWatchDogFlag=false;
int iStreamWatchDogCounter=0;

int _defaultWindowHandler(Display *disp, XErrorEvent *err)
{
  DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS,"_defaultWindowHandler - we got called.");
  return 0;
}

void* StreamWatchDogRoutine(void* param)
{
	int iLocalCounter=iStreamWatchDogCounter;
        DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS,"Started libxine watchdog routine\n");
        usleep(10000000);
	if (bStreamWatchDogFlag&&(iStreamWatchDogCounter==iLocalCounter))
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Terminating Xine_Player: watchdog detected libxine deadlock\n");

#ifdef DEBUG
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Asking for coredump...\n");
#endif
		
		DCE::LoggerWrapper::GetInstance()->Flush();
		fflush(stdout);
		
#ifdef DEBUG
		kill(getpid(), SIGSEGV);
#else
		kill(getpid(), SIGKILL);
#endif
	}
		
	return NULL;
}

// HACK we have to link against local copy of libdvdnav.a - otherwise MPEG2 playback gets crazy
/*declarations copied from xine-customized version of libdvd-nav	*/
extern "C"
{
typedef struct dvdnav_s dvdnav_t;

typedef int32_t dvdnav_status_t;

#define DVDNAV_STATUS_ERR 0
#define DVDNAV_STATUS_OK  1

dvdnav_status_t dvdnav_open(dvdnav_t **dest, const char *path);

dvdnav_status_t dvdnav_close(dvdnav_t *self);

dvdnav_status_t dvdnav_get_number_of_titles(dvdnav_t *self, int32_t *titles);

dvdnav_status_t dvdnav_get_number_of_parts(dvdnav_t *self, int32_t title, int32_t *parts);
}
/*	end of copied declarations	*/

extern "C"
{
#include <cdda_interface.h>
}

#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"

#include "Xine_Stream.h"
#include "Xine_Player.h"
#include "Colorspace_Utils.h"
#include "JpegEncoderDecoder.h"

#include "pluto_main/Define_Button.h"
#include "Gen_Devices/AllCommandsRequests.h"

#define POINTER_HIDE_SECONDS 2

/** Defined this here to avoid some dependency. */
typedef struct
{
	uint32_t flags;
	uint32_t functions;
	uint32_t decorations;
	int32_t input_mode;
	uint32_t status;
}
MWMHints;

#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask | PropertyChangeMask | PointerMotionMask)

static const char noCursorDataDescription[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

namespace DCE
{ // DCE namespace begin

Xine_Stream::Xine_Stream(Xine_Stream_Factory* pFactory, xine_t *pXineLibrary, int ID, int iTimeCodeReportFrequency, int iRequestingObject, bool bBroadcastOnly) :
		m_streamMutex("xine-stream-access-mutex"),
		m_xine_osd_t(NULL)
{
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init( &mutexAttr );
	pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_streamMutex.Init( &mutexAttr );	
	
	windows[0] = 0;
	windows[1] = 0;
	
	cursors[0] = 0;
	cursors[1] = 0;
	
	m_pFactory = pFactory;
	m_pXineLibrary = pXineLibrary;
	m_bBroadcastOnly = bBroadcastOnly;
	m_iBroadcastPort=0;
	
	if (!bBroadcastOnly)
	{
		m_sXineAudioDriverName = m_pFactory->GetAudioDriver();
		m_sXineVideoDriverName = m_pFactory->GetVideoDriver();
	}
	else
	{
		m_sXineAudioDriverName = "none";
		m_sXineVideoDriverName = "none";
	}
	
	m_pXineVisualizationPlugin = NULL;
	m_pXineDeinterlacePlugin = NULL;

	m_bInitialized = false;
	m_bIsRendering = false;
	
	m_pXineAudioOutput = NULL;
	m_pXineVideoOutput = NULL;
	
	m_iCurrentScreen = 0;
	m_iCurrentWindow = 0;
	
	m_iMenuButtons = 0;
	
	m_bDontReportCompletion = false;
	
	m_iSpecialSeekSpeed = 0;
	m_iSpecialOneTimeSeek = 0;
	m_iPrebuffer = 90000;
	
	m_iCachedStreamPosition = 0;
	m_iCachedStreamLength = 0;
	
	m_iTimeCodeReportFrequency = iTimeCodeReportFrequency;
	
	m_bUseDeinterlacing = false;
	
	m_bExitThread = false;
	m_bTrickModeActive = false;
	m_iStreamID = ID;
	m_iRequestingObject = iRequestingObject;
	
	threadEventLoop = 0;
	m_pXineStreamEventQueue = NULL;

	m_iZoomLevel = 100;
 
	m_isSlimClient = false;

        m_sMediaType = "N";
        m_iMediaID = -1;
	
	m_iMRLPos = 0;

	// VPTS stuff
	m_iStartVPTS = 0;
	m_iCurrentVPTS = 0;
	m_iStartVPTS_MS = -1;
	m_bTimecodeIsPrecise = true;
	
	m_pDynamic_Pointer = NULL;
	
	// possible extensions of subtitles
	m_SubtitlesExtensions.push_back("asc");
	m_SubtitlesExtensions.push_back("txt");
	m_SubtitlesExtensions.push_back("sub");
	m_SubtitlesExtensions.push_back("srt");
	m_SubtitlesExtensions.push_back("smi");
	m_SubtitlesExtensions.push_back("ssa");

	
	if (!bBroadcastOnly)
	{
		m_sWindowTitle = "pluto-xine-playback-window";
		// binding to window
		if ( !CreateWindows() )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Stream output window init failed");
		}
	}
	else
	{
		m_sWindowTitle = "pluto-xine-broadcaster-window";
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::Xine_Stream m_iStreamID %d",m_iStreamID);

	XSetErrorHandler(_defaultWindowHandler);

}

Xine_Stream::~Xine_Stream()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::~Xine_Stream m_iStreamID %d",m_iStreamID);

	ShutdownStream();
	
	if (!m_bBroadcastOnly)
	{
		DestroyWindows();
	}
}

// prepare stream for usage
bool Xine_Stream::StartupStream()
{
	if (m_bInitialized)
	{
		return false;
	}
	
	if ( !InitXineAVOutput() )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Stream audio/video initialization failed");
		return false;
	}

	m_iTitle=m_iChapter=-1;
	
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	m_iPrebuffer = xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER );
	m_iSeekMuteStatus = xine_get_param(m_pXineStream, XINE_PARAM_AUDIO_MUTE);
	xine_set_param(m_pXineStream, XINE_PARAM_AUDIO_VOLUME, 100);
	
	m_bInitialized = true;
	
	return true;
}

// deinitialize stream
bool Xine_Stream::ShutdownStream()
{
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if (!m_bInitialized)
			return false;
	}
	
	if (m_iBroadcastPort!=0)
	{
		DisableBroadcast();
	}
	
	playbackCompleted(false );
	
	DisableDeinterlacing();
	DisableVisualizing();
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_bInitialized = false; 
	}
	
	// disabling OSD
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		
		if (m_xine_osd_t)
		{
			xine_osd_free(m_xine_osd_t);
			m_xine_osd_t = NULL;
		}
	}
	
	// stop the event thread first
	if ( threadEventLoop )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stopping event processor." );
		
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bExitThread = true;
		}

		pthread_join( threadEventLoop, NULL );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Done." );
		threadEventLoop = 0;
	}
	
	if ( m_pXineStreamEventQueue )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Disposing the event queue" );
		xine_event_dispose_queue( m_pXineStreamEventQueue );
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineStreamEventQueue = NULL;
	}

	// xine_stop can lock up, as can xine_close_xxxx_driver
	pthread_t watchdog_thread;
	bStreamWatchDogFlag = true;
	iStreamWatchDogCounter++;
	pthread_create(&watchdog_thread, NULL,StreamWatchDogRoutine, NULL);
	pthread_detach(watchdog_thread);

	if ( m_pXineStream )
	{	
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_stop for stream with id: %d", m_iStreamID );
		xine_stop( m_pXineStream );

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_close for stream with id: %d", m_iStreamID );
		xine_close( m_pXineStream );

		
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_dispose for stream with id: %d", m_iStreamID );
		xine_dispose( m_pXineStream );
		
		m_pXineStream = NULL;
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Going to call a %p and v %p", m_pXineAudioOutput, m_pXineVideoOutput );

	if ( m_pXineAudioOutput )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_close_audio_driver for stream with id: %d", m_iStreamID );
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		xine_close_audio_driver( m_pXineLibrary, m_pXineAudioOutput );
		m_pXineAudioOutput = NULL;
	}
	
	 // Simulate deadlock
	 //usleep(100000000);

	if ( m_pXineVideoOutput )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_close_video_driver for stream with id: %d", m_iStreamID );
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		xine_close_video_driver( m_pXineLibrary, m_pXineVideoOutput );
		m_pXineVideoOutput = NULL;
	}
	
	bStreamWatchDogFlag = false;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cleanup completed" );
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_sCurrentFile = "";
		m_iTitle=m_iChapter=-1;		
	}
	
	return true;
}

// moving windows to other screen
void Xine_Stream::hideWindows()
{
}

// creates stream windows
bool Xine_Stream::CreateWindows()
{
	XColor black;

	XLockDisplay( m_pFactory->m_pXDisplay );

	windows[ 0 ] = m_pFactory->windows[ 0 ];
	windows[ 1 ] = m_pFactory->windows[ 1 ];

	noCursor = XCreateBitmapFromData( m_pFactory->m_pXDisplay, ( DefaultRootWindow( m_pFactory->m_pXDisplay ) ), noCursorDataDescription, 8, 8 );
	cursors[0] = XCreateFontCursor(m_pFactory->m_pXDisplay, XC_left_ptr);
	cursors[1] = XCreatePixmapCursor(m_pFactory->m_pXDisplay, noCursor, noCursor, &black, &black, 0, 0);
	
	if ( m_pDynamic_Pointer )
		delete m_pDynamic_Pointer;
	m_pDynamic_Pointer = new Dynamic_Pointer(this, &cursors[0], &cursors[1]);

	XFreePixmap( m_pFactory->m_pXDisplay, noCursor );

	XDefineCursor( m_pFactory->m_pXDisplay, windows[ m_iCurrentWindow ], cursors[ m_iCurrentWindow ] );

	if ( m_pDynamic_Pointer )
		m_pDynamic_Pointer->pointer_hide();

	//XSync( m_pXDisplay, True );
	XUnlockDisplay( m_pFactory->m_pXDisplay );

	return true;
}

// initializes audion/video output ports, prepares stream for open
bool Xine_Stream::InitXineAVOutput()
{
	// mapping window and raising it
// 	int xcode = XMapRaised( m_pXDisplay, windows[ 0 ] );
//	LoggerWrapper::GetInstance()->Write( LV_WARNING, "XMapWindow returned: %i", xcode);

	if (!m_bBroadcastOnly)
	{
		// init visual for xine video
		m_x11Visual.display = m_pFactory->m_pXDisplay;
		m_x11Visual.screen = m_iCurrentScreen;
		m_x11Visual.d = windows[ m_iCurrentWindow ];
		
		m_x11Visual.dest_size_cb = &destinationSizeCallback;
		m_x11Visual.frame_output_cb = &frameOutputCallback;
		
		m_x11Visual.user_data = this;
	
		// init video output
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Opening Video Driver" );
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			if ( ( m_pXineVideoOutput = xine_open_video_driver( m_pXineLibrary, m_sXineVideoDriverName.c_str(), XINE_VISUAL_TYPE_X11, ( void * ) & m_x11Visual ) ) == NULL )
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "I'm unable to initialize m_pXine's '%s' video driver. Falling to 'xshm'.", m_sXineVideoDriverName.c_str() );
				if ( ( m_pXineVideoOutput = xine_open_video_driver( m_pXineLibrary, "xshm", XINE_VISUAL_TYPE_X11, ( void * ) & m_x11Visual ) ) == NULL )
				{
					LoggerWrapper::GetInstance()->Write( LV_WARNING, "I'm unable to initialize m_pXine's 'xshm' video driver. Giving up." );
					return false;
				}
				else
				{
					m_sUsedVideoDriverName = "xshm";
				}
			}
			else
			{
				m_sUsedVideoDriverName = m_sXineVideoDriverName;
			}
		}
	}
	else
	{
		if ( ( m_pXineVideoOutput = xine_open_video_driver( m_pXineLibrary, m_sXineVideoDriverName.c_str(), XINE_VISUAL_TYPE_NONE, NULL ) ) == NULL )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "I'm unable to initialize m_pXine's '%s' video driver. Falling to 'xshm'.", m_sXineVideoDriverName.c_str() );
			return false;
		}
	}
	
	// init audio output
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Opening Audio Driver" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if ( ( m_pXineAudioOutput = xine_open_audio_driver( m_pXineLibrary, m_sXineAudioDriverName.c_str(), NULL ) ) == NULL )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "I'm unable to initialize m_pXine's '%s' audio driver.", m_sXineAudioDriverName.c_str() );
			xine_close_video_driver(m_pXineLibrary, m_pXineVideoOutput);
			return false;
		}
	}

	// init xine stream
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_stream_new" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if ( ( m_pXineStream = xine_stream_new( m_pXineLibrary, m_pXineAudioOutput, m_pXineVideoOutput ) ) == NULL )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Could not create stream" );
			xine_close_audio_driver(m_pXineLibrary, m_pXineAudioOutput);
			xine_close_video_driver(m_pXineLibrary, m_pXineVideoOutput);
			return false;
		}
	}
	
	
	
	// creating new queue
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_event_new_queue" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_pXineStreamEventQueue = xine_event_new_queue( m_pXineStream );
	}
	xine_event_create_listener_thread( m_pXineStreamEventQueue, XineStreamEventListener, this );

	if (!m_bBroadcastOnly)
	{
		xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 0 );
		xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] );
		xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );
	}

	// creating new osd panel
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_osd_new" );
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_xine_osd_t = xine_osd_new( m_pXineStream, 0, 0, 1000, 100 );
		xine_osd_set_font( m_xine_osd_t, "sans", 20 );
		xine_osd_set_text_palette( m_xine_osd_t, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1 );		
	}	
	
	return true;
}

bool Xine_Stream::CloseMedia()
{
	if ( m_pXineStream )
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_stop for stream with id: %d", m_iStreamID );
		xine_stop( m_pXineStream );

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_close for stream with id: %d", m_iStreamID );
		xine_close( m_pXineStream );
		
		m_bIsRendering = false;
		m_iTitle=m_iChapter=-1;

		m_vectMRLs.clear();
		m_iMRLPos = 0;
	}
	
	return true;
}

bool Xine_Stream::OpenMedia(string fileName, string &sMediaInfo, string sMediaPosition)
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Open media called on non-initialized stream - aborting command");
		return false;
	}
        
	// resetting info about opened stream
	m_iCachedStreamPosition = 0;
	m_iCachedStreamLength = 0;

	// Reset internal MRL list
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_vectMRLs.clear();
		m_iMRLPos = 0;
	}

	// by default, believe that everything except MPEG files has precise timecodes
	m_bTimecodeIsPrecise = ! ( StringUtils::EndsWith(fileName, ".mpg", true) ||
			StringUtils::EndsWith(fileName, ".mpeg", true) );
	
	setDebuggingLevel(true );	
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::OpenMedia Attempting to open media for %s (%s)", fileName.c_str(), sMediaPosition.c_str() );
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling xine_open" );
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_bIsVDR = fileName.substr( 0, 4 ) == "vdr:";
	}
	
	// reading info - # of titles/chapters/tracks
	sMediaInfo = readMediaInfo();
	
	// getting possible suffix for media: chapters and titles
	string sURLsuffix;
	CalculatePosition( sMediaPosition, &sURLsuffix, NULL, NULL, NULL); 
		
	bool mediaOpened = false;
	m_bIsRendering = false;
	
	// detecting external subtitles file if present
	// TODO support multiple external subtitles files 
	// TODO allow manage subtitles via UI, and store last used subtitles
	string sSubtitlesSuffix = "";
	string fileNameWithoutExtension = "";
	fileNameWithoutExtension = FileUtils::FileWithoutExtension(fileName);

	for (vector<string>::iterator it=m_SubtitlesExtensions.begin(); it!=m_SubtitlesExtensions.end(); ++it)
	{
		if (FileUtils::FileExists(fileName + "." + *it))
		{
			LoggerWrapper::GetInstance()->Write(
				LV_WARNING, "Found subtitles file: %s ",
				(fileName + "." + *it).c_str() );
			sSubtitlesSuffix = "#subtitle:" + fileName + "." + *it;
			break;
		}
		// Check for subtitle files that are named filename-without-extension.subtitle-extension
		if (FileUtils::FileExists(fileNameWithoutExtension + "." + *it))
                {
                        LoggerWrapper::GetInstance()->Write(
                                LV_WARNING, "Found subtitles file: %s ",
                                (fileNameWithoutExtension + "." + *it).c_str() );
                        sSubtitlesSuffix = "#subtitle:" + fileNameWithoutExtension + "." + *it;
                        break;
		}
	}
		
	bool is_url = ( fileName.find(":/") != string::npos );
	string tmp = (is_url) ? "" : "file://";
	if (sURLsuffix!="")
	{
		// NOTE: seek doesn't work with subtitles... or file:// url
		// HACK BEGIN
		tmp = "";
		m_sSubtitlesSuffix = sSubtitlesSuffix; // Store it temporarily. 
		// sSubtitlesSuffix = "";
		// HACK END

		tmp = tmp + fileName + sURLsuffix + sSubtitlesSuffix;

		LoggerWrapper::GetInstance()->Write(
			LV_WARNING, "Opening media with chapters/title position: '%s' ",
			tmp.c_str() );

		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		mediaOpened = xine_open( m_pXineStream, tmp.c_str() );
		if (!mediaOpened)
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Opening media FAILED");
	}

	if (!mediaOpened)
	{
		tmp = (is_url) ? "" : "file://";
		tmp = tmp + fileName + sSubtitlesSuffix;

		LoggerWrapper::GetInstance()->Write(
			LV_WARNING, "Opening media without chapters/title position: '%s' ",
			tmp.c_str() );

		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		mediaOpened = xine_open( m_pXineStream, tmp.c_str() );
		if (!mediaOpened)
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Opening media FAILED");
	}
	
	
	//setDebuggingLevel(true );	
	
	// opening media
	if ( mediaOpened )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media opened " );

		//setDebuggingLevel( false );
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bHasAudio = xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_HAS_AUDIO );
			m_bHasVideo = xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_HAS_VIDEO );

//			if ( m_iImgWidth == 0 ) m_iImgWidth++;
//			if ( m_iImgHeight == 0 ) m_iImgHeight++;
		}

		// depending on video availability, enabling deinterlacing plugin or visualizing
		if ( m_bHasVideo )
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media has video - enabling deinterlacing plugin" );
			EnableDeinterlacing();
			
		}
		else
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media doesn't have video -  enabling visualizing plugin" );
			
			if ( EnableVisualizing() )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Visualizing plugin enabled" );
				PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
//				m_iImgWidth = 100;
//				m_iImgHeight = 100;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Visualizing plugin not enabled" );
			}
		}
		
		// reporting about image
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Output window dimensions: %dx%d", m_pFactory->m_iImgWidth, m_pFactory->m_iImgHeight );

		if (!m_bBroadcastOnly)
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_DRAWABLE_CHANGED, ( void * ) windows[ m_iCurrentWindow ] );
			xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_VIDEOWIN_VISIBLE, ( void * ) 1 );
		}
		
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bIsRendering = true;
			m_bExitThread = false;
		}
		
		
		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			if (!threadEventLoop)
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Creating event processor" );
				pthread_create( &threadEventLoop, NULL, EventProcessingLoop, this );
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Event processor started" );
			}
		}

		if ( ! m_bHasVideo )
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stream is seekable: %d", xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_SEEKABLE ) );
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			xine_set_param( m_pXineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
		}

		UpdateTitleChapterInfo();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::OpenMedia failed! Aborting!" );
		return false;
	}
	
	return true;
}

bool Xine_Stream::EnableDeinterlacing()
{
//	m_sDeinterlacingConfig is a 
// 	filtername:parameter=value,parameter=value...

	if (!m_bUseDeinterlacing)
		return false;

	string::size_type tokenPos = 0;
	string sConfig = m_sDeinterlacingConfig;
	string sFilter;

	if (!m_pXineDeinterlacePlugin)
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Enabling deinterlacing" );
		
		sFilter = StringUtils::Tokenize(sConfig, string(":"), tokenPos);
		if (sFilter!="tvtime")
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Currently we support only 'tvtime' filter" );
			return false;
		} 
		
		// we need a NULL-terminated list of output ports
		xine_video_port_t **volist;
		volist = (xine_video_port_t **)malloc(2 * sizeof(xine_video_port_t*));
		
		volist[0] = m_pXineVideoOutput;
		volist[1] = NULL;

		{
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_pXineDeinterlacePlugin = xine_post_init(m_pXineLibrary, sFilter.c_str(), 0, NULL, &m_pXineVideoOutput);
		}
		
		free(volist);
	}
	
	
	//setting parameters
	map <string,string> mSettings;
	sConfig = StringUtils::Tokenize(sConfig, string(":"), tokenPos);
	tokenPos=0;
	string sPV = StringUtils::Tokenize(sConfig, string(","), tokenPos);
	while(sPV!="")
	{
		string::size_type paramPos=0;
		string sParam = StringUtils::Tokenize(sPV,string("="), paramPos);
		string sValue = StringUtils::Tokenize(sPV,string("="), paramPos);
		mSettings[sParam] = sValue;
		cout << sParam << "=" << sValue << endl;
		sPV = StringUtils::Tokenize(sConfig, string(","), tokenPos);
	}
	
	
	bool rewiringResult = false;
	
	if (m_pXineDeinterlacePlugin)
	{
		xine_post_in_t *input_api;
		xine_post_api_t *api;
		xine_post_api_descr_t *desc;
		xine_post_api_parameter_t *parm;
		char *data;
			
		input_api = (xine_post_in_t *)xine_post_input(m_pXineDeinterlacePlugin, "parameters");
		if (!input_api) {
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Failed to setup post plugin" );
			return false;
		}
			
		api = (xine_post_api_t *)input_api->data;
		desc = api->get_param_descr();
		parm = desc->parameter;
		data = (char *)malloc(desc->struct_size);
		api->get_parameters(m_pXineDeinterlacePlugin, (void *)data);
			
		while (parm->type != POST_PARAM_TYPE_LAST)
		{
			string sType = "UNKNOWN";
			switch ( parm->type )
			{
			case POST_PARAM_TYPE_LAST:
				sType = "LAST";
				break;
			case POST_PARAM_TYPE_INT:
				sType = "INT";
				break;
			case POST_PARAM_TYPE_DOUBLE:
				sType = "DOUBLE";
				break;
			case POST_PARAM_TYPE_CHAR:
				sType = "CHAR";
				break;
			case POST_PARAM_TYPE_STRING:
				sType = "STRING";
				break;
			case POST_PARAM_TYPE_STRINGLIST:
				sType = "STRINGLIST";
				break;
			case POST_PARAM_TYPE_BOOL:
				sType = "BOOL";
				break;
			default:
				break;
			}
			
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Parameter: <%s>, type=%s, \"%s\"", parm->name, sType.c_str(), parm->description);
			int i = 0;
			if(parm->enum_values != NULL)
				while(parm->enum_values[i] != NULL)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Value[%d]: %s", i, parm->enum_values[i]);
					i++;
				}
			
			string sP = parm->name;
			map<string,string>::iterator p=mSettings.find(sP);
			bool paramFound = false;
			
			if ( p!=mSettings.end() )
			{
				switch ( parm->type )
				{
				case POST_PARAM_TYPE_BOOL:
					*(int *)(data + parm->offset) = atoi( (*p).second.c_str() );
					paramFound = true;
					break;
				
				case POST_PARAM_TYPE_INT:
					{
						int i = 0;
						if(parm->enum_values != NULL)
							while(parm->enum_values[i] != NULL)
							{
								if ( (*p).second == parm->enum_values[i])
								{
									*(int *)(data + parm->offset) = i;
									paramFound = true;
									break;
								} 
								i++;
							}
					}
					break;
				
				default:
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Cannot convert value %s to required type %s", (*p).second.c_str(), sType.c_str() );
				}
			}
			
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Using [%d]%s\n", *(int *)(data + parm->offset), paramFound?"":" - default" );
			
			parm++;
		}

		api->set_parameters(m_pXineDeinterlacePlugin, (void *)data);
		free(data);
		
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		rewiringResult = xine_post_wire_video_port(xine_get_video_source(m_pXineStream), m_pXineDeinterlacePlugin->video_input[0]);
		if (!rewiringResult)
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "post-processing plugin rewiring failed");
		}
		
		return rewiringResult;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "tvtime failed to load");
		return false;
	}
}

void Xine_Stream::DisableDeinterlacing()
{
	if (!m_bUseDeinterlacing)
		return;
	
	if (m_pXineDeinterlacePlugin)
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Disabling deinterlacing" );
		
		xine_post_wire_video_port( xine_get_video_source( m_pXineStream ), m_pXineVideoOutput );
		xine_post_dispose( m_pXineLibrary, m_pXineDeinterlacePlugin );
		
		m_pXineDeinterlacePlugin = NULL;
	}
}

bool Xine_Stream::EnableVisualizing()
{
	return false;
	
	if ( ! m_pXineVisualizationPlugin )
	{	
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		m_pXineVisualizationPlugin = xine_post_init( m_pXineLibrary, "goom", 0, &m_pXineAudioOutput, &m_pXineVideoOutput );
		if (!m_pXineVisualizationPlugin)
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "xine_post_init call failed for visualization plugin" );
		}
	}
	
	bool rewiringResult = false;

	if (m_pXineVisualizationPlugin)
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		rewiringResult = xine_post_wire(xine_get_audio_source(m_pXineStream), xine_post_input(m_pXineVisualizationPlugin, "audio in"));
		//rewiringResult = xine_post_wire_audio_port(xine_get_audio_source(m_pXineStream), m_pXineVisualizationPlugin->audio_input[0]);
		
		if (!rewiringResult)
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "post-processing plugin rewiring failed");
		}
	}
	
	return rewiringResult;
}

void Xine_Stream::DisableVisualizing()
{
	if ( m_pXineVisualizationPlugin )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Disabling visualization" );
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		
		xine_post_wire_audio_port( xine_get_audio_source( m_pXineStream ), m_pXineAudioOutput );
		xine_post_dispose( m_pXineLibrary, m_pXineVisualizationPlugin );
		
		m_pXineVisualizationPlugin = NULL;
	}
}

void *Xine_Stream::EventProcessingLoop( void *arguments )
{
	Xine_Stream * pStream = ( Xine_Stream* ) arguments;

	Bool checkResult;

	// counter for timecode report
	int iCounter_TimeCode = 0;
	
	// 1/10th second interval counter
	int iCounter = 0, jCounter = 0;
	
	XEvent event;
	while ( ! pStream->m_bExitThread )
	{
		//reading and process X-events
		if (!pStream->m_bBroadcastOnly)
		{
			if ( pStream->m_bIsRendering )
			{
				do
				{
					XLockDisplay( pStream->m_pFactory->m_pXDisplay );
					checkResult = XCheckWindowEvent( pStream->m_pFactory->m_pXDisplay, pStream->windows[ pStream->m_iCurrentWindow ], INPUT_MOTION, &event );
					XUnlockDisplay( pStream->m_pFactory->m_pXDisplay );
	
					if ( checkResult == True )
						pStream->XServerEventProcessor( event );
				}
				while ( checkResult == True );
			}
		}

		// updating every second - position
		if ( ++iCounter >= 10 )
		{
			if (pStream->m_bInitialized)
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "[ID: %d] %s (seek %d) t.c. ctr %d freq %d,", pStream->m_iStreamID, pStream->GetPosition().c_str(), pStream->m_iSpecialSeekSpeed, iCounter_TimeCode, pStream->m_iTimeCodeReportFrequency );
			}
			iCounter = 0;
			
			//if it is a time - reporting our timecode to player object
			if ( pStream->m_iTimeCodeReportFrequency && ++iCounter_TimeCode >= pStream->m_iTimeCodeReportFrequency )
			{
				pStream->ReportTimecode();
				iCounter_TimeCode = 1;
			}
		}
		
		// We need to wait 500ms after the stream starts before doing the seek!
		if ( pStream->m_iSpecialOneTimeSeek )
		{
		 	jCounter++;
			if ( jCounter > 5 ) 
			{
				pStream->Seek(pStream->m_iSpecialOneTimeSeek,10000); // As long as we're within 10 seconds that's fine
				pStream->m_iSpecialOneTimeSeek = 0;
				pStream->ReportTimecode();
				pStream->changePlaybackSpeed( PLAYBACK_NORMAL );
			}
		}
		else
		{
		  	jCounter = 0;
		}
		
		//updating time and speed when @trickplay mode
		if (pStream->m_bTrickModeActive)
		{
			//only every 0.2s
			if (iCounter%2==0)
			{
				pStream->DisplaySpeedAndTimeCode();
			}
		}
		
		//updating time and speed when paused
		if ( pStream->m_iPlaybackSpeed==0)
		{
			//only every 0.5s
			if (iCounter%5==0)
			{
				pStream->DisplaySpeedAndTimeCode();
			}
		}		
		
		if ( pStream->m_iSpecialSeekSpeed )
			pStream->HandleSpecialSeekSpeed();

		usleep( 100000 );
	}

	return NULL;
}

int Xine_Stream::XServerEventProcessor(XEvent &event )
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "XServerEventProcessor called on non-initialized stream - aborting command");
		return false;
	}	
	
	Atom XA_DELETE_WINDOW;

	switch ( event.type )
	{
		case ClientMessage:
		{
			XA_DELETE_WINDOW = XInternAtom( m_pFactory->m_pXDisplay, "WM_DELETE_WINDOW", False );

			if ( ( unsigned ) event.xclient.data.l[ 0 ] == XA_DELETE_WINDOW )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::XServerEventProcessor XA_DELETE_WINDOW" );
				m_bIsRendering = false;
			}
			break;
		}

		case MotionNotify:
		{
			XMotionEvent *mevent = ( XMotionEvent * ) & event;
			int x, y;

			if ( translate_point( mevent->x, mevent->y, &x, &y ) )
			{
				xine_event_t xineEvent;
				xine_input_data_t xineInput;

				xineEvent.type = XINE_EVENT_INPUT_MOUSE_MOVE;
				xineEvent.stream = m_pXineStream;
				xineEvent.data = &xineInput;
				xineEvent.data_length = sizeof( xineInput );
				gettimeofday( &xineEvent.tv, NULL );

				xineInput.button = 0;
				xineInput.x = x;
				xineInput.y = y;
				xine_event_send( m_pXineStream, &xineEvent );
			}
			break;
		}

		case ButtonPress:
		{
			XButtonEvent *bevent = ( XButtonEvent * ) & event;
			xine_input_data_t xineInputData;
			xine_event_t xineEvent;

			x11_rectangle_t rect;

			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: mouse button event: mx=%d my=%d", bevent->x, bevent->y );

			rect.x = bevent->x;
			rect.y = bevent->y;
			rect.w = 0;
			rect.h = 0;

			xine_port_send_gui_data ( m_pXineVideoOutput, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, ( void* ) & rect );
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: mouse button event after translation: mx=%d my=%d", rect.x, rect.y );

			xineEvent.stream = m_pXineStream;
			xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
			xineEvent.data = &xineInputData;
			xineEvent.data_length = sizeof( xineInputData );

			xineInputData.button = bevent->button;
			xineInputData.x = rect.x;
			xineInputData.y = rect.y;

			gettimeofday( &xineEvent.tv, NULL );
			XLockDisplay( bevent->display );
			xine_event_send( m_pXineStream, &xineEvent );
			XUnlockDisplay( bevent->display );

			break;
		}

		case KeyPress:
		{
			XKeyEvent kevent;
			KeySym ksym;
			char kbuf[ 256 ];
			int len;

			kevent = event.xkey;

			XLockDisplay( kevent.display );
			len = XLookupString( &kevent, kbuf, sizeof( kbuf ), &ksym, NULL );
			XUnlockDisplay( kevent.display );
                // LoggerWrapper::GetInstance()->Write(LV_STATUS, "Key (%d), \"%s\" - %d", len, kbuf, ksym);

			xine_event_t xineEvent;
                // xine_input_data_t  xineInput;

			switch ( ksym )
			{
				case XK_Up:
					xineEvent.type = XINE_EVENT_INPUT_UP;
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: key press event: XK_Up");
					break;
				case XK_Down:
					xineEvent.type = XINE_EVENT_INPUT_DOWN;
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: key press event: XK_Down");
					break;
				case XK_Left:
					xineEvent.type = XINE_EVENT_INPUT_LEFT;
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: key press event: XK_Left");
					break;
				case XK_Right:
					xineEvent.type = XINE_EVENT_INPUT_RIGHT;
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: key press event: XK_Right");
					break;
				case XK_Return:
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: key press event: XK_Return");
					xineEvent.type = XINE_EVENT_INPUT_SELECT;
					break;
				default:
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine player: key press event: not in (Up, Down, Left, Right, Return)");
					xineEvent.type = 0;
			}
			xineEvent.stream = m_pXineStream;
			xineEvent.data = NULL;
			xineEvent.data_length = 0;
			gettimeofday( &xineEvent.tv, NULL );
			xine_event_send( m_pXineStream, &xineEvent );
			break;
		}

		case Expose:
		{
			XExposeEvent *exposeEvent = ( XExposeEvent * ) & event;
                	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Expose with count %d", exposeEvent->count);

			if ( exposeEvent->count != 0 )
				break;

			if ( m_pXineVideoOutput )
				xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_EXPOSE_EVENT, exposeEvent );

			break;
		}

		case ConfigureNotify:
		{
			XConfigureEvent *cev = ( XConfigureEvent * ) & event;
			Window tmp_win;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "ConfigureNotify: %ix%i", cev->width, cev->height);
			m_pFactory->m_iImgWidth = cev->width;
			m_pFactory->m_iImgHeight = cev->height;

			if ( ( cev->x == 0 ) && ( cev->y == 0 ) )
			{
				XLockDisplay( cev->display );
				XTranslateCoordinates( cev->display, cev->window, DefaultRootWindow( cev->display ), 0, 0, &m_pFactory->m_iImgXPos, &m_pFactory->m_iImgYPos, &tmp_win );
				XUnlockDisplay( cev->display );
			}
			else
			{
				m_pFactory->m_iImgXPos = cev->x;
				m_pFactory->m_iImgYPos = cev->y;
			}

		}
		break;
	}
	return true;
}

void Xine_Stream::Seek(int pos,int tolerance_ms)
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Seek called on non-initialized stream - aborting command");
		return;
	}

	// we are very tolerant or this is a broadcasting stream
	if( tolerance_ms==0 )
	{
		timespec ts1,ts2,tsElapsed;
		gettimeofday( &ts1, NULL );

		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);

		// we should use ordinary play instead of seek if we have audio-only
		if (m_bHasVideo)
		{
			if (g_bXINE_HAS_TRICKPLAY_SUPPORT)
				custom_xine_seek( m_pXineStream, 0, pos );
			else
				xine_play( m_pXineStream, 0, pos );
		}
		else
			xine_play( m_pXineStream, 0, pos );

		gettimeofday( &ts2, NULL );
		tsElapsed = ts2-ts1;
		int positionTime, totalTime;
		bool getPositionResult;
		getStreamPlaybackPosition( positionTime, totalTime, 1, &getPositionResult );
		if (!getPositionResult)
			positionTime = pos;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Seek took %d ms.  Tried for pos %d landed at %d <%s>, off by %d",
													tsElapsed.tv_sec * 1000 + tsElapsed.tv_nsec / 1000000,
													pos,positionTime, getPositionResult?"exact":"estimate",
													positionTime-pos);
                
                UpdateTitleChapterInfo();
                
		// reset to -1, which means "init"
		if (!m_bTimecodeIsPrecise)
			m_iStartVPTS_MS = -1;

		return ;
	}

	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::Seek seek to %d tolerance %d", pos, tolerance_ms );

	for ( int i = 0;i < 10;++i )
	{
		if (!m_bIsRendering)
			break;

		int positionTime, totalTime;
		bool result = getRealStreamPosition( positionTime, totalTime );
		
		if (!result)
		{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::Seek demuxer is not ready yet, sleeping 25 ms");
				Sleep( 25 );
		}
		else
		{
			if ( abs( positionTime - pos ) < tolerance_ms )
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::Seek Close enough %d %d total %d", positionTime, pos, totalTime );
				break;
			}
			else
			{
				PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::Seek get closer currently at: %d target pos: %d ctr %d", positionTime, pos, i );
				if (m_bHasVideo)
				{
					if (g_bXINE_HAS_TRICKPLAY_SUPPORT)
						custom_xine_seek( m_pXineStream, 0, pos );
					else
						xine_play( m_pXineStream, 0, pos );
				}
				else
					xine_play( m_pXineStream, 0, pos );
			}
		}
	}
        
	// reset to -1, which means "init"
	if (!m_bTimecodeIsPrecise)
		m_iStartVPTS_MS = -1;
	UpdateTitleChapterInfo();
}

void Xine_Stream::HandleSpecialSeekSpeed()
{
	if ( m_tsLastSpecialSeek.tv_sec==0 )  // Should not happen
		return ;

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "HandleSpecialSeek called on non-initialized stream - aborting command");
		return;
	}
	
	DisplaySpeedAndTimeCode();

	timespec ts;
	gettimeofday( &ts, NULL );

	timespec tsElapsed = ts-m_tsLastSpecialSeek;
	int msElapsed = tsElapsed.tv_sec * 1000 + tsElapsed.tv_nsec / 1000000;
	int seekTime = m_posLastSpecialSeek + (msElapsed * m_iSpecialSeekSpeed / 1000);  // Take the time that did elapse, factor the speed difference, and add it to the last seek
	int positionTime, totalTime;
	bool getPositionResult;
	getStreamPlaybackPosition( positionTime, totalTime, 1, &getPositionResult );

	// logic: limiting speed is 4x
	// so for ranges ( abs ) 0x-4x we are using jump 2000ms and wait sometime,
	
	if ( abs(m_iSpecialSeekSpeed) < 4*PLAYBACK_NORMAL )
	{
		// where do we seek?
		seekTime = m_posLastSpecialSeek + 2000 * ( (abs(m_iSpecialSeekSpeed)==m_iSpecialSeekSpeed)?1:-1 );
		
		if (m_iSpecialSeekSpeed==0)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"HandleSpecialSeekSpeed: speed is zero, skipping");
			return;
		}
		
		// desired values:
		// 0.25x => 750 ms
		// 0.50x => 500 ms
		// 1.00x => 250 ms
		int deltaTime;
		if (abs(m_iSpecialSeekSpeed)<=PLAYBACK_NORMAL/4)
			deltaTime = 750;
		else
			if (abs(m_iSpecialSeekSpeed)<=PLAYBACK_NORMAL/2)
				deltaTime = 500;
			else
			if (abs(m_iSpecialSeekSpeed)<=PLAYBACK_NORMAL)
				deltaTime = 250;
			else
				deltaTime = 250*m_iSpecialSeekSpeed/(4*PLAYBACK_NORMAL);
		
		if (msElapsed<deltaTime)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"HandleSpecialSeekSpeed: rewind mode, skipping seek now");
			return;
		}
	}
	else	// usual 'check interval to jump' logic
	{
		if ( (getPositionResult && (abs(positionTime-seekTime)<2000) )|| (!getPositionResult && (abs(m_posLastSpecialSeek-seekTime)<2000) ) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"HandleSpecialSeekSpeed: too small interval for seek (%i), skipping this time",
				getPositionResult?abs(positionTime-seekTime):abs(m_posLastSpecialSeek-seekTime) );
			return;
		}	
	}


	LoggerWrapper::GetInstance()->Write(LV_STATUS,"HandleSpecialSeekSpeed %d elapsed: %d ms last: %d this: %d pos %d",
												m_iSpecialSeekSpeed, msElapsed,
												m_posLastSpecialSeek,seekTime,positionTime);

	// stop if we are at stream begin
	if ( seekTime < 0 )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "aborting seek, we are at the beginning" );
		changePlaybackSpeed( PLAYBACK_FF_1 );
		ReportTimecode();
		return;
	}
	
	// stop if we are at stream end
	if ( seekTime > m_iCachedStreamLength  && m_iCachedStreamLength!=0 )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "aborting seek, we are at the end" );
		changePlaybackSpeed( PLAYBACK_STOP );
		ReportTimecode();
		playbackCompleted( false );
		{			
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bIsRendering = false;
		}
		return;
	}
	
	m_tsLastSpecialSeek=ts;
	m_posLastSpecialSeek=seekTime;
	Seek(seekTime,0);
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	m_iCachedStreamPosition = seekTime;
	
	if (!m_bTimecodeIsPrecise) 
	{
		// updating VPTS stuff and association
		m_iStartVPTS_MS = seekTime;
		m_iStartVPTS = m_iCurrentVPTS;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "HandleSpecialSeekSpeed: VPTS=>MS mapping got reset to: %i => %i", (int) m_iStartVPTS, (int) m_iStartVPTS_MS);
	}
}


bool Xine_Stream::getRealStreamPosition(int &positionTime, int &totalTime)
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getXineStreamPosition called on non-initialized stream - aborting command");
		return false;
	}

	if (!m_bTimecodeIsPrecise)
	{
		m_iCurrentVPTS = xine_get_current_vpts(m_pXineStream);
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "getXineStreamPosition: current VPTS: %i", m_iCurrentVPTS );
	}

	if ( xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_SEEKABLE ) == 0 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stream is not seekable" );
		positionTime = totalTime = 0;
		
		if (!m_bTimecodeIsPrecise && m_iStartVPTS_MS==-1)
		{
			m_iStartVPTS_MS = 0;
			m_iStartVPTS = m_iCurrentVPTS;
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "getXineStreamPosition: VPTS=>MS mapping got reset to: %i => %i", (int) m_iStartVPTS, (int) m_iStartVPTS_MS);
		}
			
		return false;
	}
	else
	{
		int iPosStream = 0;
		int iPosTime = 0;
		int iLengthTime = 0;
		
		if ( xine_get_pos_length( m_pXineStream, &iPosStream, &iPosTime, &iLengthTime ) )
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "getXineStreamPosition: %i/%i", iPosTime, iLengthTime );
			
			positionTime = iPosTime;
			totalTime = iLengthTime;
			
			if (!m_bTimecodeIsPrecise && m_iStartVPTS_MS==-1)
			{
				m_iStartVPTS_MS = iPosTime;
				m_iStartVPTS = m_iCurrentVPTS;
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "getXineStreamPosition: VPTS=>MS mapping got reset to: %i => %i", (int) m_iStartVPTS, (int) m_iStartVPTS_MS);
			}
			
			return true;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Error reading stream position: %d", xine_get_error( m_pXineStream ) );
			return false;
		}
	}
}


int Xine_Stream::getStreamPlaybackPosition( int &positionTime, int &totalTime, int attemptsCount, 
					    bool *getResult, bool alwaysFromCache, bool bEnforcePrecision )
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getStreamPlaybackPosition called on non-initialized stream - aborting command");
		if (getResult)
			*getResult = false;
		return false;
	}
	
	// care about dynamic pointer visibility
	if ( m_pDynamic_Pointer )
		m_pDynamic_Pointer->pointer_check_time();

	// if stream is in 'seek mode', then we should not call get position too often
	// the same is true if stream is paused
	if ( ( m_iSpecialSeekSpeed == 0 ) && (m_iPlaybackSpeed!=0) && !alwaysFromCache)
	{
		int iPosTime = 0;
		int iLengthTime = 0;	
		
		if ( getRealStreamPosition(iPosTime, iLengthTime) )
		{
			// updating cache
			m_iCachedStreamPosition = iPosTime;
			m_iCachedStreamLength = iLengthTime;
		}
	}
	
	// default fallback:
	// fetching from cache stream position
	if (bEnforcePrecision && !m_bTimecodeIsPrecise)
	{
		if (m_iStartVPTS_MS != -1)
		{
			positionTime = m_iStartVPTS_MS + (int64_t)(m_iCurrentVPTS-m_iStartVPTS) / 90;

			//LoggerWrapper::GetInstance()->Write( LV_STATUS, "getStreamPlaybackPosition curr %i, start %i, pos %i, ms %i",
			//		   (int)m_iCurrentVPTS, (int)m_iStartVPTS, (int)positionTime, (int)m_iStartVPTS_MS);
			
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "getStreamPlaybackPosition: position drift: %i", ((int) positionTime - m_iCachedStreamPosition));
		}
		else
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "getStreamPlaybackPosition VPTS=>MS mapping is not available, fallback to not precise position value");
			positionTime = m_iCachedStreamPosition;
		}
	}
	else
		positionTime = m_iCachedStreamPosition;
	
	totalTime = m_iCachedStreamLength;
	if (getResult)
		*getResult = true;
	return positionTime;
}

void Xine_Stream::DisplaySpeedAndTimeCode()
{

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "DisplaySpeedAndTimeCode called on non-initialized stream - aborting command");
		return;
	}

	int iDirection = m_iSpecialSeekSpeed<0?-1:1;
		
	int Whole = iDirection*m_iSpecialSeekSpeed / 1000;
	int Fraction = iDirection*m_iSpecialSeekSpeed % 1000;
	
	if (m_bTrickModeActive)
	{
		iDirection = 1;
		Whole = m_iTrickPlaySpeed / 1000;
		Fraction = m_iTrickPlaySpeed % 1000;
	}
	
	string sSpeed;

	if ( iDirection < 0 )
	{
		sSpeed += "-";
	}
	
	if ( Whole )
		sSpeed += StringUtils::itos( Whole );
	else
		sSpeed += "0";

	if ( Fraction )
		sSpeed += "." + StringUtils::itos( Fraction );

	sSpeed += "x     ";

	int seconds, totalTime;
	getStreamPlaybackPosition( seconds, totalTime );
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "seconds %d", seconds );
	seconds /= 1000;
	int seconds_only = seconds;
	int hours = seconds / 3600;
	seconds -= hours * 3600;
	int minutes = seconds / 60;
	seconds -= minutes * 60;
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "h %d m %d s %d", hours, minutes, seconds );
	if ( hours )
	{
		sSpeed += StringUtils::itos( hours ) + ":";
		if ( minutes < 10 )
			sSpeed += "0" + StringUtils::itos( minutes ) + ":";
		else
			sSpeed += StringUtils::itos( minutes ) + ":";
	}
	else
		sSpeed += StringUtils::itos( minutes ) + ":";

	if ( seconds < 10 )
		sSpeed += "0" + StringUtils::itos( seconds );
	else
		sSpeed += StringUtils::itos( seconds );


	if ( (( m_iSpecialSeekSpeed == 0 )&&!m_bTrickModeActive&&(m_iPlaybackSpeed!=0)) || ( seconds_only == 1 ) )
		DisplayOSDText("");
	else
		DisplayOSDText( sSpeed );
}


void Xine_Stream::DisplayOSDText( string sText )
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "DisplayOSDText called on non-initialized stream - aborting command");
		return;
	}

	// there is no OSD
	if (!m_xine_osd_t)
		return;
	
	if ( sText.size() == 0 )
	{
		xine_osd_clear( m_xine_osd_t );
		xine_osd_hide(m_xine_osd_t, 0);
		return;
	}
	
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "DisplayOSDText() : Attempting to display %s", sText.c_str() );
	
	xine_osd_clear( m_xine_osd_t );
	xine_osd_draw_rect( m_xine_osd_t, 0, 0, 999, 99, XINE_OSD_TEXT1, 1 );
	xine_osd_draw_text( m_xine_osd_t, 20, 20, sText.c_str(), XINE_OSD_TEXT1 );
	xine_osd_show( m_xine_osd_t, 0 );
}

void Xine_Stream::StartSpecialSeek( int Speed )
{	
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "StartSpecialSeek called on non-initialized stream - aborting command");
		return;
	}

	if (m_bHasVideo)
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		m_iSeekMuteStatus = xine_get_param(m_pXineStream, XINE_PARAM_AUDIO_MUTE);
		if (m_iSeekMuteStatus==0)
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Muting sound");
			xine_set_param(m_pXineStream, XINE_PARAM_AUDIO_MUTE, 1);
		}
	
		// this feature definitely works for MPEG2 only (may be some other audio codecs, but not generic)
		// e.g. MOV files cannot restore after it
		if (StringUtils::StartsWith(m_sCurrentFile,"dvd:", true) ||StringUtils::EndsWith(m_sCurrentFile,".mpg", true) || StringUtils::EndsWith(m_sCurrentFile,".mpeg", true))
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Disabling audio decoding for faster seek: %s", m_sCurrentFile.c_str());
			xine_set_param(m_pXineStream, XINE_PARAM_IGNORE_AUDIO, 1);
		}
	}
	
	int totalTime;
	gettimeofday( &m_tsLastSpecialSeek, NULL );
	getStreamPlaybackPosition( m_posLastSpecialSeek, totalTime );

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Starting special seek %d", Speed );
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		m_iPrebuffer = xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER );
		xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, 9000 );
	}
	
	
	m_iSpecialSeekSpeed = Speed;

	//	m_iPlaybackSpeed = PLAYBACK_NORMAL;
	DisplaySpeedAndTimeCode();
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "done Starting special seek %d", Speed );
	/*	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "V0: %d",  m_iSpecialSeekSpeed);
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "V1: %d",  xine_get_param( m_pXineStream, XINE_PARAM_SPEED));
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "V2: %d",  xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER));
	*/
}

void Xine_Stream::StopSpecialSeek()
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "StopSpecialSeek called on non-initialized stream - aborting command");
		return;
	}

	
	if (!m_iSpecialSeekSpeed)
		return;
	
	if (m_bHasVideo)
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if (xine_get_param(m_pXineStream, XINE_PARAM_IGNORE_AUDIO)==1)
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Enabling audio decoding for stopping seek");
			xine_set_param(m_pXineStream, XINE_PARAM_IGNORE_AUDIO, 0);
		}
		
		if (m_iSeekMuteStatus==0)
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Unmuting sound");
			xine_set_param(m_pXineStream, XINE_PARAM_AUDIO_MUTE, 0);
		}
	}
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stopping special seek" );
	{
		m_iSpecialSeekSpeed = 0;
	}
	//m_iPlaybackSpeed = PLAYBACK_NORMAL;
	DisplayOSDText("");
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, m_iPrebuffer );
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "done Stopping special seek" );
}


bool Xine_Stream::setSubtitle( int Value )
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "SetSubtitle called on non-initialized stream - aborting command");
		return false;
	}

	int numsubtitle = xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_MAX_SPU_CHANNEL );
	if ( Value > numsubtitle )
		Value = -1;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "SPU was %d now %d", getSubtitle(), Value );
	xine_set_param( m_pXineStream, XINE_PARAM_SPU_CHANNEL, Value );

	// TODO: Display current audio lang on all bound orbiter displays
	char lang[XINE_LANG_MAX];
	string sID = StringUtils::itos(Value);
	string sName = "";
	string sFormat = "";
	string sLanguage = "";
	if ( xine_get_spu_lang( m_pXineStream, Value, lang) ) {
		sLanguage = lang;
	}
	m_pFactory->m_pPlayer->EVENT_Subtitle_Track_Changed( sID, m_iStreamID, sName, sFormat, sLanguage );

	return true;
}

bool Xine_Stream::setAudio( int Value )
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "SetAudio called on non-initialized stream - aborting command");
		return false;
	}

	int numaudio = xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL );
	if ( Value > (numaudio - 1) )
		Value = 0;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "AUDIO was %d now %d", xine_get_param ( m_pXineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL ), Value );
	xine_set_param( m_pXineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, Value );

	// TODO: Display current audio lang on all bound orbiter displays
	int channel = -1; // the current channel
	char lang[XINE_LANG_MAX];
	string sID = StringUtils::itos(Value);
	string sName = "";
	string sFormat = "";
	string sLanguage = "";
	if ( xine_get_audio_lang( m_pXineStream, channel, lang) ) {
		sLanguage = lang;
	}
	m_pFactory->m_pPlayer->EVENT_Audio_Track_Changed( sID, m_iStreamID, sName, sFormat, sLanguage );

	return true;
}

int Xine_Stream::getSubtitle()
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getSubtitle called on non-initialized stream - aborting command");
		return false;
	}

	return xine_get_param ( m_pXineStream, XINE_PARAM_SPU_CHANNEL );
}

int Xine_Stream::getAudio()
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getAudio called on non-initialized stream - aborting command");
		return false;
	}
	
	return xine_get_param ( m_pXineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL );
}

const char *Xine_Stream::TranslateLanguage( const char *abbreviation )
{
	const char * pAbbreviation;
	if ( abbreviation[ 0 ] == ' ' )
		pAbbreviation = &abbreviation[ 1 ];
	else
		pAbbreviation = abbreviation;

	if ( strcmp( pAbbreviation, "en" ) == 0 )
		return "English";
	else if ( strcmp( pAbbreviation, "de" ) == 0 )
		return "Deutsch";
	else if ( strcmp( pAbbreviation, "fr" ) == 0 )
		return "Francais";
	else if ( strcmp( pAbbreviation, "ru" ) == 0 )
		return "Russian";
	else if ( strcmp( pAbbreviation, "ro" ) == 0 )
		return "Romanian";

	return pAbbreviation;
}

void Xine_Stream::XineStreamEventListener( void *streamObject, const xine_event_t *event )
{
	Xine_Stream * pXineStream = ( Xine_Stream * ) streamObject;
	
	if (!pXineStream->m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "XineStreamEventListener called on non-initialized stream - aborting command");
		return;
	}


	switch ( event->type )
	{
		case XINE_EVENT_UI_PLAYBACK_FINISHED:
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got XINE_EVENT_UI_PLAYBACK_FINISHED" );
			// Check if we have got any extended MRLs
			if (!pXineStream->PlayNextMRL())
			{
				//pXineStream->StopSpecialSeek();
				pXineStream->changePlaybackSpeed( PLAYBACK_STOP );
				pXineStream->ReportTimecode();
				pXineStream->playbackCompleted( false );
				{			
					PLUTO_SAFETY_LOCK(streamLock, pXineStream->m_streamMutex);
					pXineStream->m_bIsRendering = false;
				}
			}
			break;
		case XINE_EVENT_QUIT:
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stream was disposed" );
	                // the playback completed is sent from another place. (see the stopMedia)
			break;

		case XINE_EVENT_PROGRESS:
		{
			xine_progress_data_t *pProgressEvent = ( xine_progress_data_t * ) event->data;
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Playback (%s) is at %d%.", pProgressEvent->description, pProgressEvent->percent );
		}
		break;

		case XINE_EVENT_UI_NUM_BUTTONS:
		{
			//pXineStream->StopSpecialSeek();
			pXineStream->changePlaybackSpeed( PLAYBACK_NORMAL );
			pXineStream->ReportTimecode();
			int iButtons = ( ( xine_ui_data_t * ) event->data ) ->num_buttons;

			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Menu with %d buttons", iButtons );
			
                        pXineStream->UpdateTitleChapterInfo();
                        
			pXineStream->FireMenuOnScreen(iButtons);

			
		}
		break;

		case XINE_EVENT_UI_SET_TITLE:
			if ( pXineStream->m_iSpecialSeekSpeed )
				break; // Ignore this while we're doing all those seeks
			{
				PLUTO_SAFETY_LOCK(streamLock, pXineStream->m_streamMutex);

				xine_ui_data_t *data = ( xine_ui_data_t * ) event->data;
				
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "UI set title: %s %s", data->str, pXineStream->GetPosition().c_str() );

				//updating title/chapter numbers
				pXineStream->UpdateTitleChapterInfo();


                                // if this is Internet Radio, then we can read the album and title
                                if(StringUtils::StartsWith(pXineStream->m_sCurrentFile, "http://", true))
                                {
                                    bool bRead=false;
                                    string sAlbum = "";
                                    string sTitle = "";
                                    {
                                      PLUTO_SAFETY_LOCK(streamLock, pXineStream->m_streamMutex);
                                      if (pXineStream->m_bInitialized)
                                        {
                                          const char *pAlbum = xine_get_meta_info(pXineStream->m_pXineStream, XINE_META_INFO_ALBUM);
                                          const char *pTitle = xine_get_meta_info(pXineStream->m_pXineStream, XINE_META_INFO_TITLE);
                                          if (pAlbum) sAlbum = pAlbum;
                                          if (pTitle) sTitle = pTitle;
                                          bRead=true;
                                        }
                                    }

                                    // only if we actually read data, process it - otherwise skip
                                    if (bRead)
                                    {
                                      if (sAlbum!="")
                                        StringUtils::Replace(&sTitle, sAlbum+" - ", "");

                                      LoggerWrapper::GetInstance()->Write( LV_STATUS, "Title of new song: %s", sTitle.c_str());
				      if (sTitle!="")
				      {
					  pXineStream->SendMediaDescription("INTERNET_RADIO:"+sTitle);
				      }
                                    }
				    
				    // enabling VPTS feature to make clock tick
				    pXineStream->m_bTimecodeIsPrecise = false;
				    pXineStream->m_iStartVPTS_MS = -1;
                                }
			}
			break;
			
		case XINE_EVENT_FRAME_FORMAT_CHANGE:
		{
			pXineStream->ReadAVInfo();
			pXineStream->SendAVInfo();
		}
		break;

		case XINE_EVENT_INPUT_MOUSE_MOVE:
			if ( pXineStream->m_pDynamic_Pointer )
				pXineStream->m_pDynamic_Pointer->pointer_show();
			break;
		case XINE_EVENT_SPU_BUTTON:
		case XINE_EVENT_INPUT_MOUSE_BUTTON:
		case XINE_EVENT_INPUT_MENU1:
		case XINE_EVENT_INPUT_MENU2:
		case XINE_EVENT_INPUT_MENU3:
		case XINE_EVENT_INPUT_MENU4:
		case XINE_EVENT_INPUT_MENU5:
		case XINE_EVENT_INPUT_MENU6:
		case XINE_EVENT_INPUT_MENU7:
		case XINE_EVENT_INPUT_UP:
		case XINE_EVENT_INPUT_DOWN:
		case XINE_EVENT_INPUT_LEFT:
		case XINE_EVENT_INPUT_RIGHT:
		case XINE_EVENT_INPUT_SELECT:
		case XINE_EVENT_INPUT_NEXT:
		case XINE_EVENT_INPUT_PREVIOUS:
		case XINE_EVENT_INPUT_ANGLE_NEXT:
		case XINE_EVENT_INPUT_ANGLE_PREVIOUS:
		case XINE_EVENT_INPUT_BUTTON_FORCE:
		case XINE_EVENT_INPUT_NUMBER_0:
		case XINE_EVENT_INPUT_NUMBER_1:
		case XINE_EVENT_INPUT_NUMBER_2:
		case XINE_EVENT_INPUT_NUMBER_3:
		case XINE_EVENT_INPUT_NUMBER_4:
		case XINE_EVENT_INPUT_NUMBER_5:
		case XINE_EVENT_INPUT_NUMBER_6:
		case XINE_EVENT_INPUT_NUMBER_7:
		case XINE_EVENT_INPUT_NUMBER_8:
		case XINE_EVENT_INPUT_NUMBER_9:
		case XINE_EVENT_INPUT_NUMBER_10_ADD:
            // ignored at the moment;
			break;

		case XINE_EVENT_UI_CHANNELS_CHANGED:
			if ( pXineStream->m_iSpecialSeekSpeed )
				break; // Ignore this while we're doing all those seeks
			{
				int numaudio = xine_get_stream_info( pXineStream->m_pXineStream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL );
				if ( numaudio > 1 )
				{
					string sAudioTracks = StringUtils::itos( pXineStream->getAudio() ) + "\n";
					int iResult = 1, i;
					for ( i = 0;i < 100;++i )
					{
						char lang[ XINE_LANG_MAX ];
						strcpy( lang, "**error**" );
						iResult = xine_get_audio_lang( pXineStream->m_pXineStream, i, lang );
						if ( iResult == 0 )
							break;
						sAudioTracks += string( pXineStream->TranslateLanguage( lang ) ) + "\n";
					}
					if ( i >= 100 )
						LoggerWrapper::GetInstance()->Write( LV_WARNING, "Something went wrong audio tracks>100" );
					
					pXineStream->m_pFactory->ReportAudioTracks( sAudioTracks );
				}
				else
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Ignoring XINE_EVENT_UI_CHANNELS_CHANGED since there aren't multiple audio tracks" );

				int numsubtitle = xine_get_stream_info( pXineStream->m_pXineStream, XINE_STREAM_INFO_MAX_SPU_CHANNEL );
				if ( numsubtitle > 1 )
				{
					string sSubtitles = StringUtils::itos( pXineStream->getSubtitle() ) + "\n";
					int iResult = 1, i;
					for ( i = 0;i < 100;++i )
					{
						char lang[ XINE_LANG_MAX ];
						strcpy( lang, "**error**" );
						iResult = xine_get_spu_lang( pXineStream->m_pXineStream, i, lang );
						if ( iResult == 0 )
							break;
						sSubtitles += string( pXineStream->TranslateLanguage( lang ) ) + "\n";
					}
					if ( i >= 100 )
						LoggerWrapper::GetInstance()->Write( LV_WARNING, "Something went wrong subitltes>100" );
					
					pXineStream->m_pFactory->ReportSubtitles( sSubtitles );
				}
				else
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Ignoring XINE_EVENT_UI_CHANNELS_CHANGED since there aren't multiple subtitles" );
					
				pXineStream->ReadAVInfo();
				pXineStream->SendAVInfo();
			}
			break;

		case XINE_EVENT_UI_MESSAGE:
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Received XINE_EVENT_UI_MESSAGE, decoding:" );
			
			string message;
			
			xine_ui_message_data_t *data = (xine_ui_message_data_t *) event->data;
			switch(data->type)
			{
				case XINE_MSG_NO_ERROR:
				{
					/* copy strings, and replace '\0' separators by '\n' */
					char* s = data->messages;
					char* d = new char[2000];

					while(s && (*s != '\0') && ((*s + 1) != '\0'))
					{
						switch(*s)
						{
							case '\0':
							{
								*d = '\n';
								break;
							}
							default:
							{
								*d = *s;
								break;
							}
						}
						s++;
						d++;
					}
					*++d = '\0';

					message = d;
					delete [] d;
					break;
				}
				case XINE_MSG_GENERAL_WARNING:
				{
					message = "XINE_MSG_GENERAL_WARNING: ";

					if(data->explanation)
						message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);
					else
						message = message + "no information";

					break;
				}
				case XINE_MSG_SECURITY:
				{
					message = "XINE_MSG_SECURITY: ";

					if(data->explanation)
						message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);

					break;
				}
				case XINE_MSG_UNKNOWN_HOST:
				{
					message = "XINE_MSG_UNKNOWN_HOST: ";
					if(data->explanation)
						message = message + ((char *) data + data->parameters) ;
					break;
				}
				case XINE_MSG_UNKNOWN_DEVICE:
				{
					message = "XINE_MSG_UNKNOWN_DEVICE: ";
					if(data->explanation)
						message = message  + ((char *) data + data->parameters) ;
					break;
				}
				case XINE_MSG_NETWORK_UNREACHABLE:
				{
					message = "XINE_MSG_NETWORK_UNREACHABLE: ";
					if(data->explanation)
						message = message + ((char *) data + data->parameters) ;
					break;
				}
				case XINE_MSG_AUDIO_OUT_UNAVAILABLE:
				{
					message = "XINE_MSG_AUDIO_OUT_UNAVAILABLE: ";
					if(data->explanation)
						message = message + ((char *) data + data->parameters) ;
                                        
                                        // notifying orbiter
					string sOrbiterMsg = "Audio output is busy or not available";
					pXineStream->SendMessageToOrbiter(sOrbiterMsg);
                                        
					break;
				}
				case XINE_MSG_CONNECTION_REFUSED:
				{
					message = "XINE_MSG_CONNECTION_REFUSED: ";
					if(data->explanation)
						message = message + ((char *) data + data->parameters) ;
					break;
				}
				case XINE_MSG_FILE_NOT_FOUND:
				{
					message = "XINE_MSG_FILE_NOT_FOUND: ";
					if(data->explanation)
						message = message  + ((char *) data + data->parameters) ;

					// notifying orbiter
					string sOrbiterMsg = "File not found: " + pXineStream->m_sCurrentFile;
					pXineStream->SendMessageToOrbiter(sOrbiterMsg);
                                        
					break;
				}
				case XINE_MSG_PERMISSION_ERROR:
				{
					message = "XINE_MSG_PERMISSION_ERROR: ";
					if(data->explanation)
						message = message  + ((char *) data + data->parameters) ;
					break;
				}
				case XINE_MSG_READ_ERROR:
				{
					message = "XINE_MSG_READ_ERROR: ";
					if(data->explanation)
						message = message  + ((char *) data + data->parameters) ;
                                        
                                        // notifying orbiter
                                        string sOrbiterMsg = "Cannot read ";
                                        if ( StringUtils::StartsWith(pXineStream->m_sCurrentFile, "dvd:/") )
                                        {
                                          sOrbiterMsg += "DVD, disk is not readable or DVDCSS library is not installed";
                                        }
                                        else
                                        {
                                          sOrbiterMsg += "file " + pXineStream->m_sCurrentFile;
                                        }
                                        pXineStream->SendMessageToOrbiter(sOrbiterMsg);
                                        
					break;
				}
				case XINE_MSG_LIBRARY_LOAD_ERROR:
				{
					message = "XINE_MSG_LIBRARY_LOAD_ERROR: ";
					if(data->explanation)
						message = message + ((char *) data + data->parameters);
					break;
				}
				case XINE_MSG_ENCRYPTED_SOURCE:
				{
					message = "XINE_MSG_ENCRYPTED_SOURCE: ";
					if(data->explanation)
						message = message  + ((char *) data + data->parameters) ;
                                        
                                        // notifying orbiter
                                        string sOrbiterMsg = "Cannot read ";
                                        if ( StringUtils::StartsWith(pXineStream->m_sCurrentFile, "dvd:/") )
                                        {
                                          sOrbiterMsg += "encrypted DVD, probably DVDCSS library is not installed";
                                        }
                                        else
                                        {
                                          sOrbiterMsg += "encrypted file " + pXineStream->m_sCurrentFile;
                                        }
                                        pXineStream->SendMessageToOrbiter(sOrbiterMsg);
                                        
					break;
				}
				default:
				{
					message ="Unknown error: ";
					if(data->explanation)
						message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);
					break;
				}
			}
			
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Message details: %s", message.c_str() );
			
		}
		break;

		case XINE_EVENT_MRL_REFERENCE_EXT:
		{
			xine_mrl_reference_data_ext_t *ref = (xine_mrl_reference_data_ext_t *) event->data;
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "XINE_EVENT_MRL_REFERENCE_EXT got mrl [%s] (alternative=%d)\n", ref->mrl, ref->alternative);
			pXineStream->AddMRL(ref);
		}
		break;

		default:
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got unprocessed Xine playback event: %d", event->type );
			break;
	}

	if ( pXineStream->m_pDynamic_Pointer )
		pXineStream->m_pDynamic_Pointer->pointer_check_time();
}

void Xine_Stream::selectNextButton()
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "SelectNextButton called on non-initialized stream - aborting command");
		return;
	}

	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", m_iStreamID );

	xine_event_t event;

	event.type = XINE_EVENT_INPUT_UP;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
}

void Xine_Stream::selectPrevButton()
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Select Prev Button called on non-initialized stream - aborting command");
		return;
	}

	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", m_iStreamID );

	xine_event_t event;

	event.type = XINE_EVENT_INPUT_DOWN;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
}

void Xine_Stream::pushCurrentButton()
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "PushCurrentButton called on non-initialized stream - aborting command");
		return;
	}

	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Selecting next hot spot on the m_pstream %d", m_iStreamID );

	xine_event_t event;

	event.type = XINE_EVENT_INPUT_SELECT;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
}

bool Xine_Stream::playStream( string mediaPosition)
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "playStream called on non-initialized stream - aborting command");
		return false;
	}

	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		changePlaybackSpeed( PLAYBACK_NORMAL );
	}
	
	setZoomLevel();
	
	// hiding OSD panel
	DisplayOSDText("");
	
	time_t startTime = time( NULL );

	int Subtitle = -2, Angle = -2, AudioTrack = -2;
	
	int pos = CalculatePosition( mediaPosition, NULL, &Subtitle, &Angle, &AudioTrack );

	if ( xine_play( m_pXineStream, 0, 0) )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Playing... The command took %d seconds to complete at pos %d", time( NULL ) - startTime, pos );
			
		if ( pos )
		{
            // This functionality in xine keeps bouncing back and forth between working and not working
            // With some Xine revisions you can pass the seek position in when you start playing, but
            // other times you must wait at least 500ms after starting the stream before trying to seek
            // or else you get
            // "Stream is not seekable".  In such cases we have a m_iSpecialOneTimeSeek that we will use
            // if we fail to do the seek correctly
			m_iSpecialOneTimeSeek = pos;
		}

		if ( Subtitle != -2 )
			setSubtitle( Subtitle );
		if ( AudioTrack != -2 )
			setAudio( AudioTrack );

/*
		if ( playbackStopped )
			xine_set_param( m_pXineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
		else
			xine_set_param( m_pXineStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL );
*/
		// reset to -1, which means "init"
		if (!m_bTimecodeIsPrecise)
			m_iStartVPTS_MS = -1;
		
		ReportTimecode();
				
		ReadAVInfo();

		
		return true;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Play failed with error %d", xine_get_error( m_pXineStream ) );
		return false;
	}
}

void Xine_Stream::changePlaybackSpeed( PlayBackSpeedType desiredSpeed )
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed speed %d",(int) desiredSpeed);
	
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "changePlayback speed called on non-initialized stream - aborting command");
		return;
	}

	bool trickModeSupported =  g_bXINE_HAS_TRICKPLAY_SUPPORT && (StringUtils::StartsWith(m_sCurrentFile,"dvd:", true) ||StringUtils::EndsWith(m_sCurrentFile,".mpg", true) || StringUtils::EndsWith(m_sCurrentFile,".mpeg", true));
	
	bool trickModeActive = m_bTrickModeActive;
	
	// there few possible modes between which we have to switch correctly:
	// normal play
	// trick play
	// special seek
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		m_iPlaybackSpeed = desiredSpeed;
	}
	
	// pause or normal playback requested - stopping any special mode and playing as usually	
	if ((desiredSpeed == PLAYBACK_FF_1)||(desiredSpeed == PLAYBACK_STOP) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed stopping any seeker");
		
		if (m_iSpecialSeekSpeed)
		{
			StopSpecialSeek();
		}
		else 
		{
			if (g_bXINE_HAS_TRICKPLAY_SUPPORT)
			{
				if ( trickModeActive )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed stopping trick play");
					
					PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
					custom_xine_stop_trick_play(m_pXineStream);
					xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, m_iPrebuffer );
					EnableDeinterlacing();
					m_iTrickPlaySpeed = 0;
					m_bTrickModeActive = false;
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Xine_Stream::changePlaybackSpeed no running seekers found");
			}
			else
			{
						LoggerWrapper::GetInstance()->Write(LV_WARNING,"Xine_Stream::changePlaybackSpeed no running seekers found");
			}
		}

		
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		xine_set_param( m_pXineStream, XINE_PARAM_SPEED, (desiredSpeed == PLAYBACK_FF_1)?XINE_SPEED_NORMAL:XINE_SPEED_PAUSE );
		
		DisplayOSDText( "" );
		
		return;
	}
		
	// reverse play requested or trick_play is not supported for this stream
	// or we need ultra-fast-forward (>16x)
	// or this stream doesn't have video
	const int UltraFastFWD = 16*1000;
	if ( (desiredSpeed < 0)||( (desiredSpeed > 0) && !trickModeSupported )||(desiredSpeed>UltraFastFWD) ||!m_bHasVideo)
	{	
		if (g_bXINE_HAS_TRICKPLAY_SUPPORT)
		{
			if ( trickModeActive )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed stopping trick play");
				PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
				custom_xine_stop_trick_play(m_pXineStream);
				xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, m_iPrebuffer );
				EnableDeinterlacing();
				m_iTrickPlaySpeed = 0;
				m_bTrickModeActive = false;
			}
		}
		if (m_iSpecialSeekSpeed==0)
		{
			StartSpecialSeek( desiredSpeed );
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed changing special seek speed");
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
			m_iSpecialSeekSpeed = desiredSpeed;
		}
		
		return;
	}
	

	if (g_bXINE_HAS_TRICKPLAY_SUPPORT)
	{
		// trick play is supported and  desired speed is ok
		if ((desiredSpeed > 0) && trickModeSupported)
		{
			if (m_iSpecialSeekSpeed)
			{
				StopSpecialSeek();
			} 
			
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Xine_Stream::changePlaybackSpeed starting trick play");
	
			// are we doing restart
			if (!m_bTrickModeActive)
			{
				PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
				m_iPrebuffer = xine_get_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER);
				xine_set_param( m_pXineStream, XINE_PARAM_METRONOM_PREBUFFER, 9000 );
				DisableDeinterlacing();
			}
			else
			{
				PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
				custom_xine_stop_trick_play(m_pXineStream);	
			}
			
			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
			custom_xine_start_trick_play(m_pXineStream, desiredSpeed*1000);
			m_iTrickPlaySpeed = desiredSpeed;
			m_bTrickModeActive = true;
			
			return;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Xine_Stream::changePlaybackSpeed IMPOSSIBLE_STATE");
}

Xine_Stream::PlayBackSpeedType Xine_Stream::getPlaybackSpeed()
{

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getPlaybackSpeed called on non-initialized stream - aborting command");
		return PLAYBACK_STOP;
	}

	
	int currentSpeed;
	switch ( ( currentSpeed = xine_get_param( m_pXineStream, XINE_PARAM_SPEED ) ) )
	{
		case XINE_SPEED_SLOW_4:
			return PLAYBACK_FF_1_4;

		case XINE_SPEED_SLOW_2:
			return PLAYBACK_FF_1_2;

		case XINE_SPEED_NORMAL:
			return PLAYBACK_FF_1;

		case XINE_SPEED_FAST_2:
			return PLAYBACK_FF_2;

		case XINE_SPEED_FAST_4:
			return PLAYBACK_FF_4;

		case XINE_SPEED_PAUSE:
			return PLAYBACK_STOP;

		default:
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Can't translate current Xine speed %d. Assuming normal playback speed!", currentSpeed );
			return PLAYBACK_FF_1;
		}
	}
}

bool Xine_Stream::DestroyWindows()
{
	if ( m_pFactory->m_pXDisplay != NULL )
	{
		delete m_pDynamic_Pointer;
		m_pDynamic_Pointer = NULL;
		
		XLockDisplay( m_pFactory->m_pXDisplay );

		XFreeCursor( m_pFactory->m_pXDisplay, cursors[ 0 ] );
		XFreeCursor( m_pFactory->m_pXDisplay, cursors[ 1 ] );

		XUnlockDisplay( m_pFactory->m_pXDisplay );
	}

	return true;
}

void Xine_Stream::grab_x_window( int &width, int &height, char*&pData, int &iDataSize )
{	
	iDataSize=0;
	
	int iRetCode = system("xwd -silent -nobdrs -name pluto-xine-playback-window | convert - /tmp/pluto_xine_snapshot.jpg");
	
	if (iRetCode == 0)
	{
		FILE * file;
		file = fopen( "/tmp/pluto_xine_snapshot.jpg", "rb" );
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		fseek(file, 0, SEEK_SET);
		pData = new char[size];
		iDataSize = size*fread(pData, size, 1, file);
		fclose( file );
		
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "grab_x_window() :: window grabbed, data size is %i", iDataSize);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "grab_x_window() :: window grabbing failed, return code: %i", iRetCode);
	}
}

void Xine_Stream::make_snapshot( string sFormat, int iWidth, int iHeight, bool bKeepAspect, char*&pData, int &iDataSize )
{
	
    //   uint8_t   *yuv, *y, *u, *v, *rgb;
    //   void      *blob;
    //   int        ratio, format;
    //   int        width, height;
    //   double     desired_ratio, image_ratio, f;
    //   static int initMagick = 1;
	//
	// /*   FILE *outfile; */
	//
    //   ExceptionInfo exception;
    //   Image *image;
    //   Image *resize_image;
    //   ImageInfo *image_info;

	int imageWidth = 0, imageHeight = 0, imageRatio = 0, imageFormat = 0;
	uint8_t *imageData = NULL;

	// if we cannot use xine for making snapshots => using window grabbing
	if ( (m_sUsedVideoDriverName=="xvmc") || (m_sUsedVideoDriverName=="xxmc") )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getScreenshot called for XvMC picture - grabbing window directly");
		
		grab_x_window( imageWidth, imageHeight, pData, iDataSize);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Getting frame info" );
		if ( ! xine_get_current_frame( m_pXineStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, NULL ) )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::make_snapshot(): Error getting frame info. Returning!" );
			return ;
		}
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat );
	
		imageData = ( uint8_t* ) malloc ( ( imageWidth + 8 ) * ( imageHeight + 1 ) * 2 );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Getting frame data" );
		{
			if ( ! xine_get_current_frame ( m_pXineStream, &imageWidth, &imageHeight, &imageRatio, &imageFormat, imageData ) )
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::make_snapshot(): Error getting frame data. Returning!" );
				return ;
			}
		}
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Got %d %d %d 0x%x", imageWidth, imageHeight, imageRatio, imageFormat );
	
	// we should have the image in YV12 format aici
	// if not then we try to convert it.
		if ( imageFormat == XINE_IMGFMT_YUY2 )
		{
			uint8_t * yuy2Data = imageData;
			imageData = ( uint8_t * ) malloc ( imageWidth * imageHeight * 2 );
	
			Colorspace_Utils::yuy2toyv12 (
					imageData,
			imageData + imageWidth * imageHeight,
			imageData + imageWidth * imageHeight * 5 / 4, yuy2Data, imageWidth, imageHeight );
	
			free ( yuy2Data );
		}
	
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Converted to YV12" );
	// convert to RGB
	// keep the yv12Data array around to be able to delete it
		uint8_t *yv12Data = imageData;
	
		/** @brief this function will allocate the output parameter */
		imageData = Colorspace_Utils::yv12torgb (
				imageData,
		imageData + imageWidth * imageHeight,
		imageData + imageWidth * imageHeight * 5 / 4,
		imageWidth, imageHeight );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Converted to RGB!" );
		free( yv12Data );
	
		double outputRatio;
	// double currentRatio;
	
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Temp data was freed here!" );
		switch ( imageRatio )
		{
			case XINE_VO_ASPECT_ANAMORPHIC:                   /* anamorphic     */
				case XINE_VO_ASPECT_PAN_SCAN:                     /* we display pan&scan as widescreen */
					outputRatio = 16.0 / 9.0;
					break;
	
					case XINE_VO_ASPECT_DVB:                          /* 2.11:1 */
						outputRatio = 2.11 / 1.0;
						break;
	
						case XINE_VO_ASPECT_SQUARE:                       /* square pels */
							case XINE_VO_ASPECT_DONT_TOUCH:                   /* probably non-mpeg m_m_pXineStream => don't touch aspect ratio */
								outputRatio = ( double ) imageWidth / ( double ) imageHeight;
								break;
	
			default:
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream::make_snapshot(): Unknown aspect ratio for image (%d) using 4:3", imageRatio );
	
				case XINE_VO_ASPECT_4_3:                          /* 4:3             */
					outputRatio = 4.0 / 3.0;
					break;
		}
	
		double f = outputRatio / ( ( double ) imageWidth / ( double ) imageHeight );
	
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): The ratio between the current ration and the output ratio is %f", f );
		double t_width, t_height;
	//     if( !bKeepAspect )
	//     {
		if ( f >= 1.0 )
		{
			t_width = imageWidth * f;
			t_height = imageHeight;
		}
		else
		{
			t_width = imageWidth;
			t_height = imageHeight / f;
		}
	
		t_width /= 2;
		t_height /= 2;
	
	
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): Making jpeg from RGB" );
		JpegEncoderDecoder jpegEncoder;
		
		jpegEncoder.encodeIntoBuffer( ( char * ) imageData, imageWidth, imageHeight, 3, pData, iDataSize );
	
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): final image size: %d", iDataSize );
		free( imageData );
	
		FILE * file;
		file = fopen( "/tmp/file.jpg", "wb" );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): temporary image filep %p", file );
	
		fwrite( pData, iDataSize, 1, file );
		fclose( file );
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::make_snapshot(): At the end. Returning" );
	return ;
}

/**
    \fn Xine_Stream::restartMediaStream(int iStreamID)
 */
void Xine_Stream::restartMediaStream()
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "restartMediaStream called on non-initialized stream - aborting command");
		return;
	}

	changePlaybackSpeed( PLAYBACK_NORMAL );
}

/**
    \fn Xine_Stream::pauseMediaStream(int iStreamID)
 */
void Xine_Stream::pauseMediaStream( )
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "pauseMediaStream called on non-initialized stream - aborting command");
		return;
	}
	
	int stoppedTime, completeTime;

	if ( getPlaybackSpeed() == PLAYBACK_STOP )
		changePlaybackSpeed( PLAYBACK_FF_1 );
	else
		changePlaybackSpeed( PLAYBACK_STOP );

	getStreamPlaybackPosition( stoppedTime, completeTime );
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stream paused at time: %d from %d", stoppedTime, completeTime );
}

KeySym Xine_Stream::translatePlutoKeySymToXKeySym( int plutoButton )
{
	switch ( plutoButton )
	{
		case BUTTON_Up_Arrow_CONST:
			return XK_Up;
		case BUTTON_Down_Arrow_CONST:
			return XK_Down;
		case BUTTON_Left_Arrow_CONST:
			return XK_Left;
		case BUTTON_Right_Arrow_CONST:
			return XK_Right;
		case BUTTON_Enter_CONST:
			return XK_Return;

		default:
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Translating of the %d pluto key is not handled yet.", plutoButton );
			return 0;
	}
}

void Xine_Stream::selectMenu( int iMenuType )
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "selectMenu called on non-initialized stream - aborting command");
		return;
	}

	
	xine_event_t xine_event;

	xine_event.type = iMenuType + XINE_EVENT_INPUT_MENU1;
	xine_event.data_length = 0;
	xine_event.data = NULL;
	xine_event.stream = m_pXineStream;
	gettimeofday( &xine_event.tv, NULL );

	xine_event_send( m_pXineStream, &xine_event );
}

void Xine_Stream::playbackCompleted( bool bWithErrors )
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "playbackCompleted called on non-initialized stream - aborting command");
		return;
	}
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Fire playback completed event: %s (slim: %i, do not report: %i)", ( ! m_isSlimClient && ! m_bDontReportCompletion )?"yes":"no", m_isSlimClient, m_bDontReportCompletion );
	//XineStream * xineStream = getStreamForId( iStreamID, "Can't get the position of a nonexistent stream!" );

	if ( ! m_isSlimClient && ! m_bDontReportCompletion)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Player::EVENT_Playback_Completed(streamID=%i)", m_iStreamID);
		m_pFactory->m_pPlayer->EVENT_Playback_Completed(m_sCurrentFile,m_iStreamID, bWithErrors );
		m_pFactory->m_pPlayer->UnmountRemoteDVD();
	}
}

bool Xine_Stream::hasChapters()
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	return xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_HAS_CHAPTERS);
}

int Xine_Stream::DisableBroadcast( )
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "DisableBroadcast called on non-initialized stream - aborting command");
		return false;
	}
	
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		
		// setting up a watchdog
		pthread_t watchdog_thread;
		bStreamWatchDogFlag = true;
		iStreamWatchDogCounter++;
		pthread_create(&watchdog_thread, NULL,StreamWatchDogRoutine, NULL);
		pthread_detach(watchdog_thread);
		
		xine_set_param( m_pXineStream, XINE_PARAM_BROADCASTER_PORT, 0 );
		
		// disabling watchdog
		bStreamWatchDogFlag = false;
		
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Disabled broadcast on port: %i", m_iBroadcastPort);
		m_iBroadcastPort = 0;
		
	}
        	
        return 0;
}

int Xine_Stream::EnableBroadcast( )
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "EnableBroadcast called on non-initialized stream - aborting command");
		return m_iBroadcastPort=0;
	}

	// base port for broadcast
 	int portNumber = 20000;
 	{
 		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);	
		for ( int i = 0; i < 1024; i++ )
		{
			xine_set_param( m_pXineStream, XINE_PARAM_BROADCASTER_PORT, ++portNumber );
			if ( portNumber == xine_get_param( m_pXineStream, XINE_PARAM_BROADCASTER_PORT ) )
			{
				m_iBroadcastPort=portNumber;
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Enabled broadcast on port: %i", m_iBroadcastPort);
				return m_iBroadcastPort;
			}
		}
	}
	
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Enabling broadcast failed");
	return m_iBroadcastPort=0;
}

void Xine_Stream::simulateMouseClick( int X, int Y )
{
//	XineStream * pStream;
	xine_input_data_t xineInputData;
	xine_event_t xineEvent;

//	if ( ( pStream = getStreamForId( 1, "Xine_Stream::simulateMouseClick() getting one stream" ) ) == NULL )
//		return ;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::simulateMouseClick(): simulating mouse click: mx=%d my=%d", X, Y );

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "simulateMouseClick called on non-initialized stream - aborting command");
		return;
	}

			
	xineEvent.stream = m_pXineStream;
	xineEvent.type = XINE_EVENT_INPUT_MOUSE_BUTTON;
	xineEvent.data = &xineInputData;
	xineEvent.data_length = sizeof( xineInputData );

	xineInputData.button = 1;
	xineInputData.x = X;
	xineInputData.y = Y;

	gettimeofday( &xineEvent.tv, NULL );
	XLockDisplay( m_pFactory->m_pXDisplay );
	xine_event_send( m_pXineStream, &xineEvent );
	XUnlockDisplay( m_pFactory->m_pXDisplay );
}

void Xine_Stream::simulateKeystroke( int plutoButton )
{
	if (m_bBroadcastOnly)
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::simulateKeystroke(): plutoButton=%d  --- skipping, as in broadcasting-only mode", plutoButton );
	}
	else
	{
		Window oldWindow;
		int oldRevertBehaviour;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::simulateKeystroke(): plutoButton=%d", plutoButton );
	
		XLockDisplay( m_pFactory->m_pXDisplay );
		XGetInputFocus( m_pFactory->m_pXDisplay, &oldWindow, &oldRevertBehaviour );
		XSetInputFocus( m_pFactory->m_pXDisplay, windows[ m_iCurrentWindow ], RevertToParent, CurrentTime );
		XTestFakeKeyEvent( m_pFactory->m_pXDisplay, XKeysymToKeycode( m_pFactory->m_pXDisplay, translatePlutoKeySymToXKeySym( plutoButton ) ), True, 0 );
		XTestFakeKeyEvent( m_pFactory->m_pXDisplay, XKeysymToKeycode( m_pFactory->m_pXDisplay, translatePlutoKeySymToXKeySym( plutoButton ) ), False, 0 );
	
		if ( oldWindow )
			XSetInputFocus( m_pFactory->m_pXDisplay, oldWindow, oldRevertBehaviour, CurrentTime );
	
		XFlush( m_pFactory->m_pXDisplay );
		XUnlockDisplay( m_pFactory->m_pXDisplay );
	}
}

void Xine_Stream::sendInputEvent( int eventType )
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "sendInputEvent on non-initialized stream - aborting command");
		return;
	}

	
	//m_iSpecialSeekSpeed = 0;

	time_t startTime = time( NULL );
	//XineStream *pStream;
	//if ( ( pStream = getStreamForId( 1, "void Xine_Stream::sendInputEvent(int eventType) getting one stream" ) ) == NULL )
	//	return ;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Sending %d event to the engine.", eventType );

	xine_event_t event;

	event.type = eventType;
	event.stream = m_pXineStream;
	event.data = NULL;
	event.data_length = 0;
	gettimeofday( &event.tv, NULL );

	xine_event_send( m_pXineStream, &event );
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream::sendInputEvent() Sending input event with ID: %d took %d seconds", eventType, time( NULL ) - startTime );
}

/**
 * xine Callbacks.
 */

void Xine_Stream::destinationSizeCallback( void *data, int video_width, int video_height, double video_pixel_aspect,
		int *dest_width, int *dest_height,
		double *dest_pixel_aspect )
{
	Xine_Stream * pStream = ( Xine_Stream* ) data;

    /**
	 * @test
	if( ! m_pStream->m_bIsRendering )
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Destination size callback called (not rendering)!");
		 */

	*dest_width = pStream->m_pFactory->m_iImgWidth;
	*dest_height = pStream->m_pFactory->m_iImgHeight;
	*dest_pixel_aspect = pStream->m_pFactory->m_dScreenPixelAspect;
}

void Xine_Stream::frameOutputCallback( void *data, int video_width, int video_height, double video_pixel_aspect,
																						int *dest_x, int *dest_y, int *dest_width, int *dest_height,
																						double *dest_pixel_aspect,
																						int *win_x, int *win_y 
//
																						)
{
	Xine_Stream * pStream = ( Xine_Stream* ) data;

    /**
	 * @test
	 *     if( ! pStream->m_bIsRendering)
	 *         LoggerWrapper::GetInstance()->Write(LV_STATUS, "Framer Output callback called (not rendering).");
		 */

	*dest_x = 0;
	*dest_y = 0;
	*win_x = pStream->m_pFactory->m_iImgXPos;
	*win_y = pStream->m_pFactory->m_iImgYPos;
	*dest_width = pStream->m_pFactory->m_iImgWidth;
	*dest_height = pStream->m_pFactory->m_iImgHeight;
	*dest_pixel_aspect = pStream->m_pFactory->m_dScreenPixelAspect;
}

bool Xine_Stream::setDebuggingLevel( bool newValue )
{
	//XineStream * xineStream = getStreamForId( m_itreamID, "Trying to set debugging level for and invalid stream: (%d)" );

	//if ( xineStream == NULL )
//		return false;

	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Open media called on non-initialized stream - aborting command");
		return false;
	}

	
	if ( newValue )
	{
		xine_set_param( m_pXineStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG );
	}
	else
	{
		xine_set_param( m_pXineStream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE );
	}

	return true;
}

int Xine_Stream::translate_point( int gui_x, int gui_y, int *video_x, int *video_y )
{
	x11_rectangle_t rect;
    //   int             xwin, ywin;
    //   unsigned int    wwin, hwin, bwin, dwin;
    //   float           xf,yf;
    //   float           scale, width_scale, height_scale,aspect;
    //   Window          rootwin;

	rect.x = gui_x;
	rect.y = gui_y;
	rect.w = 0;
	rect.h = 0;

	if ( m_pXineVideoOutput && xine_port_send_gui_data( m_pXineVideoOutput, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, ( void* ) & rect ) != -1 )
	{
        /**
		driver implements gui->video coordinate space translation, use it
				 */
		*video_x = rect.x;
		*video_y = rect.y;
		return 1;
	}

	return 1;
}


void Xine_Stream::Dynamic_Pointer::pointer_hide()
{
	Window window = m_pOwner->windows[m_pOwner->m_iCurrentWindow];
	if (window)
	{
		XDefineCursor( m_pOwner->m_pFactory->m_pXDisplay, window, *m_pCursor_hidden );
		m_start_time = 0;
	}
}

void Xine_Stream::Dynamic_Pointer::pointer_show()
{
	Window window = m_pOwner->windows[m_pOwner->m_iCurrentWindow];
	if (window)
	{
		XDefineCursor( m_pOwner->m_pFactory->m_pXDisplay, window, *m_pCursor_normal );
		m_start_time = time( NULL );
	}
}

void Xine_Stream::Dynamic_Pointer::pointer_check_time()
{
	if ( m_start_time == 0 )
		return ;
	time_t now_time = time( NULL );
	if ( difftime( now_time, m_start_time ) >= POINTER_HIDE_SECONDS )
		pointer_hide();
}

Xine_Stream::Dynamic_Pointer::Dynamic_Pointer(
		Xine_Stream *pOwner,
    Cursor *pCursor_normal,
		Cursor *pCursor_hidden
																									) :
		m_pOwner(pOwner),
        m_pCursor_normal(pCursor_normal),
				m_pCursor_hidden(pCursor_hidden),
				m_start_time(0)
{
}

Xine_Stream::Dynamic_Pointer::~Dynamic_Pointer()
{
}

void Xine_Stream::getScreenShot( int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result )
{

   	// only make screenshots if it's a video stream.
		
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getScreenshot called on non-initialized stream - aborting command");
		return;
	}

/*
	if ( (m_sUsedVideoDriverName=="xvmc") || (m_sUsedVideoDriverName=="xxmc") )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "getScreenshot called for XvMC picture - aborting command");
		return;
	}
*/	
	if ( !m_bHasVideo )
	{
		iDataSize = 0;
		return ;
	}

	sFormat = "JPG";
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Making snapshot" );
	make_snapshot( sFormat.c_str(), iWidth, iHeight, true, pData, iDataSize );
}


string Xine_Stream::getRenderingWindowName()
{
	return m_sWindowTitle;
}

int Xine_Stream::getDeviceId()
{
	return 0;
	//!return m_pAggregatorObject->m_dwPK_Device;
}

bool Xine_Stream::isSlimClient()
{
	return m_isSlimClient;
}

void Xine_Stream::setSlimClient( bool isSlimClient )
{
	m_isSlimClient = isSlimClient;
}

string Xine_Stream::GetPosition()
{
	string sPosition;
	
	if( m_iChapter!=-1 )
		sPosition += " CHAPTER:" + StringUtils::itos(m_iChapter);
	
	int currentTime, totalTime;
	getStreamPlaybackPosition(currentTime, totalTime);
	sPosition += " POS:" + StringUtils::itos(currentTime);

	if( m_iTitle!=-1 )
		sPosition += " TITLE:" + StringUtils::itos(m_iTitle);

	sPosition += " SUBTITLE:" + StringUtils::itos(getSubtitle());
	sPosition += " AUDIO:" + StringUtils::itos(getAudio());
	sPosition += " TOTAL:" + StringUtils::itos(totalTime);

	return sPosition;
}

void Xine_Stream::ReportTimecode()
{
	if (!m_bInitialized)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "ReportTimecode called on non-initialized stream - aborting command");
		return;
	}

	if (!m_iTimeCodeReportFrequency||!m_bIsRendering )
		return;
	
	if( m_bIsVDR )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ignoring vdr timecode for now");
		return;
	}
	
	m_pFactory->ReportTimecodeViaIP( m_iStreamID, m_iPlaybackSpeed);
}

int Xine_Stream::CalculatePosition(string &sMediaPosition,string *sMRL,int *Subtitle,int *Angle,int *AudioTrack)
{
	string::size_type pos=0;
	int iTitle=-1,iChapter=-1,iPos=0;

	if( sMRL )
	{
		pos = sMediaPosition.find(" TITLE:");
		if( pos!=string::npos )
			iTitle = atoi( sMediaPosition.substr(pos+7).c_str() );

		pos = sMediaPosition.find(" CHAPTER:");
		if( pos!=string::npos )
			iChapter = atoi( sMediaPosition.substr(pos+9).c_str() );

		if( iTitle!=-1 && iChapter!=-1 )
			(*sMRL)+="/" + StringUtils::itos(iTitle) + "." + StringUtils::itos(iChapter);
	}

	if( Subtitle )
	{
		pos = sMediaPosition.find(" SUBTITLE:");
		if( pos!=string::npos )
			*Subtitle = atoi( sMediaPosition.substr(pos+10).c_str() );
	}

	if( AudioTrack )
	{
		pos = sMediaPosition.find(" AUDIO:");
		if( pos!=string::npos )
			*AudioTrack = atoi( sMediaPosition.substr(pos+7).c_str() );
	}

	pos = sMediaPosition.find(" POS:");
	if( pos!=string::npos )
		iPos = atoi( sMediaPosition.substr(pos+5).c_str() );

	return iPos;
}

void Xine_Stream::FireMenuOnScreen(int iButtons)
{
	m_iMenuButtons = iButtons;
	m_pFactory->m_pPlayer->FireMenuOnScreen( m_iRequestingObject, m_iStreamID, iButtons != 0 );
}

string Xine_Stream::readMediaInfo()
{
	// pre-reading media information
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Reading stream media info: \n");
	
	string sMediaInfo = "";
	
	// is this DVD ? the MRL should be dvd:// + /dev/drive OR /path/file
	if(StringUtils::StartsWith(m_sCurrentFile, "dvd://", true))
	{
		string dvdName = m_sCurrentFile;
		dvdName = StringUtils::Replace(dvdName, "dvd://", "");
		//HACK we often receive the MRL like dvd://dev/cdrom - this is WRONG, doing temporary fix
		if (dvdName[0]!='/')
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "DVD name with bad syntax supplied(%s)! Please fix the CMD sender!", m_sCurrentFile.c_str());
			dvdName = "/" + dvdName;
		}
		dvdnav_t *dvdHandle=NULL;
		dvdnav_status_t res = dvdnav_open(&dvdHandle, dvdName.c_str());
		if (res == DVDNAV_STATUS_ERR)
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "libdvdnav: Error opening DVD");
		}
		else
		{
			int titlesCount;
			res = dvdnav_get_number_of_titles(dvdHandle, &titlesCount);
			if (res == DVDNAV_STATUS_OK)
			{
				for (int i=1; i<=titlesCount; i++)
				{
					int chaptersCount;
					res = dvdnav_get_number_of_parts(dvdHandle, i, &chaptersCount);
					if (res == DVDNAV_STATUS_OK)
					{
						if (chaptersCount==0)
							sMediaInfo += "Title " + StringUtils::itos(i) + "\t\t" + StringUtils::itos(i) + "\n";
						else
						{
							for (int j=1; j<=chaptersCount; j++)
								sMediaInfo += "Title " + StringUtils::itos(i) + " Chapter "+ StringUtils::itos(j) +"\t"+ StringUtils::itos(j) +"\t" + StringUtils::itos(i) + "\n";
						}
					}
					else
					{
						LoggerWrapper::GetInstance()->Write( LV_WARNING, "libdvdnav: Error reading chapters count for title %i", i);
						sMediaInfo += "Title " + StringUtils::itos(i) + "\t\t" + StringUtils::itos(i) + "\n";
					}
				}
			}
			else
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "libdvdnav: Error reading titles count");

			dvdnav_close(dvdHandle);
			
		}
		
	}

	// if this is Audio CD disk, MRL is like cdda:///dev/cdrom/12 - for 12th track
	if(StringUtils::StartsWith(m_sCurrentFile, "cdda://", true))
	{
		string drvName = m_sCurrentFile;
		drvName = StringUtils::Replace(drvName, "cdda://", "");
		uint last_slash = drvName.find_last_of("/");
		if (last_slash!=string::npos)
		{
			drvName.erase(last_slash);
			
			cdrom_drive *pDrive = cdda_identify(drvName.c_str(), CDDA_MESSAGE_FORGETIT, NULL);
			if (!pDrive)
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Failed to identify cd drive: %s", drvName.c_str());
			}
			else
			{
				if (!cdda_open(pDrive))
				{
					int tracksCount = cdda_tracks(pDrive);
					cdda_close(pDrive);
					for (int i=1; i<=tracksCount; i++)
					{
						sMediaInfo += "Track " + StringUtils::itos(i) + "\t" + StringUtils::itos(i) + "\n";
					}
				}
				else
				{
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Failed to open cd drive: %s", drvName.c_str());
				}
			}				
		}
	}
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stream media info (length=%i): \n%s", sMediaInfo.length(), sMediaInfo.c_str());
	
	m_sMediaInfo = sMediaInfo;
	
	return sMediaInfo;
}

void Xine_Stream::ReadAVInfo()
{
	{
		PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		if (!m_bInitialized)
			return;
	}

	m_sAudioInfo = "";
	m_sVideoInfo = "";
	
	// temporary data
	string sAudioInfo = "";
	string sVideoInfo = "";
	
	if (m_bHasAudio)
	{
		const char *pInfo=NULL;
		 {
		 	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
		 	pInfo = xine_get_meta_info(m_pXineStream, XINE_META_INFO_AUDIOCODEC);
		 }

		if (pInfo)
		{
			sAudioInfo = pInfo;
			
			// is this dolby/dts?
			if (StringUtils::StartsWith(sAudioInfo, "A/52"))
			{
				m_sAudioInfo = "dolby digital";
				
				// getting number of channels if present
				string s = sAudioInfo;
				s = StringUtils::Replace(s, "A/52", "");
				s = StringUtils::TrimSpaces(s);
				uint pos = s.find_first_of(".");
				if ( (pos!=string::npos) && (pos==1) )
				{
					m_sAudioInfo += " " + s.substr(0, 1);
				}
			}
			else 
				if (StringUtils::StartsWith(sAudioInfo, "DTS"))
				{
					m_sAudioInfo = "dts";
				
					// getting number of channels if present
					string s = sAudioInfo;
					s = StringUtils::Replace(s, "DTS", "");
					s = StringUtils::TrimSpaces(s);
					uint pos = s.find_first_of(".");
					if ( (pos!=string::npos) && (pos==1) )
					{
						m_sAudioInfo += " " + s.substr(0, 1);
					}
				}
				else
					m_sAudioInfo = "pcm";
		}
	}
	
	if (m_bHasVideo)
	{
		const char *pInfo = NULL;
		 {
		 	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			pInfo = xine_get_meta_info(m_pXineStream, XINE_META_INFO_VIDEOCODEC);
		}
		
		if (pInfo)
		{
			int iVideoWidth = xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_VIDEO_WIDTH );
			int iVideoHeight = xine_get_stream_info( m_pXineStream, XINE_STREAM_INFO_VIDEO_HEIGHT );
			//detecting frame ratio (actual)
			if ( (iVideoHeight!=0) && (iVideoWidth!=0) )
			{
				// calculating X:Y - minimal ratio
				
				int iW = iVideoWidth;
				int iH = iVideoHeight;
				
				int div = 2;
				
				while ( iH >= 2*div )
				{
					if ( (iH%div==0) && (iW%div==0) )
					{
						iH /= div;
						iW /= div;
					}
					else
						div++;
				}
				
				m_sVideoInfo = StringUtils::itos( iW ) + ":" + StringUtils::itos( iH );
			}
			
			sVideoInfo = pInfo;
			sVideoInfo += ", " + StringUtils::itos( iVideoWidth ) + "x" + StringUtils::itos( iVideoHeight );
		}
	}	

	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Read media A/V information: [%s]/[%s] => [%s]/[%s]", sAudioInfo.c_str(), sVideoInfo.c_str(), m_sAudioInfo.c_str(), m_sVideoInfo.c_str() );
}

void Xine_Stream::SendAVInfo()
{
	m_pFactory->ReportAVInfo( m_sCurrentFile, m_iStreamID, m_sMediaInfo, m_sAudioInfo, m_sVideoInfo);
}

void Xine_Stream::SendMediaDescription(string sMediaDescription)
{
    m_pFactory->ReportMediaDescription(sMediaDescription);
}

bool Xine_Stream::setAspectRatio(string sAR)
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	int iAspectRatio = -1;
	
	if (sAR == "auto")
		iAspectRatio = XINE_VO_ASPECT_AUTO;
	else if (sAR == "1:1")
		iAspectRatio = XINE_VO_ASPECT_SQUARE;
	else if (sAR == "4:3")
		iAspectRatio = XINE_VO_ASPECT_4_3;
	else if (sAR == "16:9")
		iAspectRatio = XINE_VO_ASPECT_ANAMORPHIC;
	else if (sAR == "2.11:1")
		iAspectRatio = XINE_VO_ASPECT_DVB;
	
	if (iAspectRatio == -1)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Unknown aspect ratio: %s", sAR.c_str());
		return false;
	}
	else
	{
		xine_set_param( m_pXineStream, XINE_PARAM_VO_ASPECT_RATIO, iAspectRatio );
		int iSetValue = xine_get_param( m_pXineStream, XINE_PARAM_VO_ASPECT_RATIO);
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Current aspect ratio: %i", iSetValue);
		return true;
	}
}

bool Xine_Stream::setZoomLevel(string sZL)
{
/*	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	
	int iNewLevel = m_iZoomLevel;
	if ( sZL.length()>0 )
	{
		if ( sZL[0]=='-' || sZL[0]=='+' )
			iNewLevel += atoi(sZL.c_str());
		else
			iNewLevel = atoi(sZL.c_str());
	}
	
	if (iNewLevel<=0 || iNewLevel>XINE_VO_ZOOM_MAX)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Bad zoom level change request %s : leads to wrong zoom level %i", sZL.c_str(), iNewLevel);
		return false;
	}
	else
	{
		xine_set_param( m_pXineStream, XINE_PARAM_VO_ZOOM_X, iNewLevel );
		xine_set_param( m_pXineStream, XINE_PARAM_VO_ZOOM_Y, iNewLevel );
		
		int iValX = xine_get_param( m_pXineStream, XINE_PARAM_VO_ZOOM_X );
		int iValY = xine_get_param( m_pXineStream, XINE_PARAM_VO_ZOOM_Y );
		
		if ( iValX!=iNewLevel && iValY!=iNewLevel )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine engine completely refused to set new zoom level", sZL.c_str());
			return false;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine engine set new zoom level to X:%i, Y:%i", iValX, iValY); 
			m_iZoomLevel = iNewLevel;
			return true;
		}
	}
*/
}

void Xine_Stream::SendMessageToOrbiter(string sMessage)
{
  if (m_pFactory)
    m_pFactory->SendMessageToOrbiter(sMessage);
}

void Xine_Stream::UpdateTitleChapterInfo()
{
//<-mkr_B_via_b->
#ifndef VIA
	// VeXP has older version xine-lib, without this feature
	// updating title/chapter info when menu is on screen
    {
        PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
        m_iTitle = xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_DVD_TITLE_NUMBER);
        m_iChapter = xine_get_stream_info(m_pXineStream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER);
    }
#endif
//<-mkr_B_via_e->
}

void Xine_Stream::AddMRL(xine_mrl_reference_data_ext_t *ref)
{
        PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Stream::AddMRL(): Adding MRL to m_vetMRLs: '%s' ", ref->mrl );
	m_vectMRLs.push_back(ref->mrl);
}

bool Xine_Stream::PlayNextMRL()
{
	PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
	bool bMoreMRLs = !m_vectMRLs.empty() && m_iMRLPos < m_vectMRLs.size();

	if (bMoreMRLs)
	{
		string tmp = m_vectMRLs[m_iMRLPos];
		m_iMRLPos++;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Stream::PlayNextMRL(): Trying to open next MRL in list: '%s' ", tmp.c_str() );

		changePlaybackSpeed( PLAYBACK_STOP );
		bool mediaOpened = xine_open( m_pXineStream, tmp.c_str() );
		if (!mediaOpened)
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Stream::PlayNextMRL(): Opening media FAILED");
		else 
		{
			if (playStream(""))
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Stream::PlayNextMRL(): New MRL is playing.");
				return true;
			}
		}

	}
	return false;
}

} // DCE namespace end
