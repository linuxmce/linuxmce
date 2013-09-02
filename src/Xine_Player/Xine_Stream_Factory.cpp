/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"

#include <dlfcn.h>	
#include <cmath>

#include "Xine_Stream_Factory.h"
#include "Xine_Stream.h"
#include "Xine_Player.h"

#include <xine.h>
#include <xine/xineutils.h>
#include <xine/video_out.h>
#include <xine/audio_out.h>

#include "utilities/linux/window_manager/WMController/WMController.h"

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

namespace DCE
{ // DCE namespace begin

	int  (*custom_xine_seek) (xine_stream_t *stream, int start_pos, int start_time) = NULL;
	int  (*custom_xine_start_trick_play)(xine_stream_t *stream, int trick_speed) = NULL;
	int  (*custom_xine_stop_trick_play)(xine_stream_t *stream) = NULL;
	
	bool g_bXINE_HAS_TRICKPLAY_SUPPORT=false;

Xine_Stream_Factory::Xine_Stream_Factory(Xine_Player *pOwner):
		m_factoryMutex("xine-stream-factory-access-mutex"),
		m_pPlayer(pOwner)
{
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init( &mutexAttr );
	pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_factoryMutex.Init( &mutexAttr );
		
	m_bInitialized = false;
	m_pXineLibrary = NULL;
	
	m_sWindowTitle = "pluto-xine-playback-window";
	m_pXDisplay = NULL;
	m_iCurrentScreen = 0;
	m_iCurrentWindow = 0;
	
	m_iLastRenderingStream  = 0;

	// default drivers names
	m_sXineAudioDriverName = "alsa";
	m_sXineVideoDriverName = "xv";
	
	// default pixel aspect
	m_dScreenPixelAspect = 1.0;
	
	// set window size
	m_iImgWidth = 0;
	m_iImgHeight = 0;
	m_iImgXPos = 0;
	m_iImgYPos = 0;	

	DCEConfig * pConfig = new DCEConfig();
	m_sConfigFile = pConfig->ReadString( "XinePlayerConfigurationFile", "/etc/pluto/xine.conf" );
	delete pConfig;
}

Xine_Stream_Factory::~Xine_Stream_Factory()
{
	if (m_bInitialized)
		ShutdownFactory();
}

bool Xine_Stream_Factory::StartupFactory()
{
	PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
	
	// avoid double-initialization
	if (m_bInitialized) 
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Double initialization attempted - bad code?");
		return false;
	}
	
	// initing X threads
	if ( !XInitThreads() )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "XInitThreads() failed!" );
		return false;
	}

	// connect to libxine
	m_pXineLibrary = xine_new();
	 if (!m_pXineLibrary)
	 {
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Cannot connect to Xine Library");
		return false;
	 }
	
	IdentifyXineStuff();
	
	// loading config
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Loading config from %s", m_sConfigFile.c_str() );		
	xine_config_load( m_pXineLibrary, m_sConfigFile.c_str() );
	// be verbose
	xine_engine_set_param(m_pXineLibrary, XINE_ENGINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG);
	xine_init( m_pXineLibrary );	

	/*
# path to the title key cache
# string, default: /root/.dvdcss/
input.css_cache_path:/home/.dvdcss/
*/
	xine_config_register_string(m_pXineLibrary, "input.css_cache_path", "/home/.dvdcss/", "path to the title key cache", "", 0, NULL, NULL);
	// detecting output drivers
	DetectOutputDrivers();
	
	// setting audio settings 
	//setAudioSettings();
	
	// setting output driver
	setVideoDriver( m_pPlayer->DATA_Get_Hardware_acceleration() );
	
	m_sDeinterlacingConfig = m_pPlayer->DATA_Get_Deinterlacing_Mode();
	m_bUseDeinterlacing = (m_sDeinterlacingConfig!="");
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Deinterlacing Mode is '%s' => deinterlacing is %s", m_sDeinterlacingConfig.c_str(), m_bUseDeinterlacing?"on":"off" );
	
	 if (!CreateWindows())
	 {
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Cannot create output windows");
		return false;
	 }
	
	m_bInitialized = true;
	return true;
}

bool Xine_Stream_Factory::ShutdownFactory()
{
	// avoid double-deinitialization
	if (!m_bInitialized) 
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Double deinitialization attempted - wrong code?");
		return false;
	}
	
	// destroying all streams
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Destroying all active streams");
	map<int, Xine_Stream*>::iterator stream;
	while(true)
	{
		int streamID;
		{
			PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
			stream = streamsMap.begin();
			if (stream == streamsMap.end())
				break;
			else
				streamID = (*stream).first;
		}
		DestroyStream( streamID);
	}
	
	DestroyWindows();
	
	PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
	
	// disconnect from libxine
	xine_exit(m_pXineLibrary);
	m_pXineLibrary = NULL;
	m_bInitialized = false;
	
	return true;
}


// TODO refactor using new ALSA configurator

// detecting currently used drivers
void Xine_Stream_Factory::DetectOutputDrivers()
{
	//registering video key param

	xine_cfg_entry_t xineConfigEntry;
	char **driver_ids;
	int i;
	const char *const *driver_names;
	
	driver_names = xine_list_video_output_plugins( m_pXineLibrary );
	
	i = 0;
	while ( driver_names[ i++ ] )
		;
	driver_ids = ( char ** ) xine_xmalloc( sizeof( char * ) * ( i + 1 ) );
	i = 0;
	driver_ids[ i ] = strdup( "auto" );
	
	while ( driver_names[ i ] )
	{
		driver_ids[ i + 1 ] = strdup( driver_names[ i ] );
		i++;
	}
	
	driver_ids[ i + 1 ] = NULL;
	xine_config_register_enum( m_pXineLibrary, "video.driver", 0, driver_ids, "video driver to use", "Choose video driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL );	
	
	// setting video driver
	// TODO: Shouldn't we use Xine_Stream_Factory::setVideoDriver in here as well?
	if ( xine_config_lookup_entry ( m_pXineLibrary, "video.driver", &xineConfigEntry ) )
	{
		driver_names = xine_list_video_output_plugins( m_pXineLibrary );
		m_sXineVideoDriverName = driver_ids[ xineConfigEntry.num_value ];
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Using video driver: %s", m_sXineVideoDriverName.c_str() );
	}
	else
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Video driver key was not defined in the config file, using hardcoded default: %s", m_sXineVideoDriverName.c_str() );
	
	i = 0;
	while (driver_ids[i])
	{
		free( driver_ids[i] );
		i++;
	}
	free( driver_ids );


	// registering audio keyparam

	driver_names = xine_list_audio_output_plugins( m_pXineLibrary );
	i = 0;
	while ( driver_names[ i++ ] )
		;
	driver_ids = ( char ** ) xine_xmalloc( sizeof( char * ) * ( i + 1 ) );
	i = 0;
	driver_ids[ i ] = strdup( "auto" );
	while ( driver_names[ i ] )
	{
		driver_ids[ i + 1 ] = strdup( driver_names[ i ] );
		i++;
	}
	
	driver_ids[ i + 1 ] = NULL;
	xine_config_register_enum( m_pXineLibrary, "audio.driver", 0, driver_ids, "audio driver to use", "Choose audio driver. NOTE: you may restart xine to use the new driver", 0, NULL, NULL );

	// audio driver
	if ( xine_config_lookup_entry ( m_pXineLibrary, "audio.driver", &xineConfigEntry ) )
	{
		driver_names = xine_list_audio_output_plugins( m_pXineLibrary );
		m_sXineAudioDriverName = driver_ids[ xineConfigEntry.num_value ];
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Using audio driver: %s", m_sXineAudioDriverName.c_str() );
	}
	else
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Audio driver key was not defined in the config file, using hardcoded default: %s", m_sXineAudioDriverName.c_str() );
	
	free( driver_ids );
}

static const char *audio_out_types_strs[] =
{
	"Mono 1.0",
	"Stereo 2.0",
	"Headphones 2.0",
	"Stereo 2.1",
	"Surround 3.0",
	"Surround 4.0",
	"Surround 4.1",
	"Surround 5.0",
	"Surround 5.1",
	"Surround 6.0",
	"Surround 6.1",
	"Surround 7.1",
	"Pass Through",
	NULL
};

// if pEnumValues == NULL, considering variable as string
// otherwise, treat it as an enum and set the variable in integer mode
void Xine_Stream_Factory::SetALSAConfigurationEntry(string sEntry, string sValue, const char **pEnumValues)
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::SetALSAConfigurationEntry: => setting '%s' to '%s'",
					   sEntry.c_str(), sValue.c_str());
	
	xine_cfg_entry_t Entry;
	if (xine_config_lookup_entry(m_pXineLibrary, sEntry.c_str(), &Entry) == 0)
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream_Factory::SetALSAConfigurationEntry: cannot find entry '%s', skipping",
						   sEntry.c_str());
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::SetALSAConfigurationEntry: variable type is %s", pEnumValues?"enum":"string");
		
		if (pEnumValues)
		{
			string sOldValue;
			int iNewValue=-1;
			for (int i=0; const char *pValue=pEnumValues[i]; i++)
			{
				if (Entry.num_value==i)
					sOldValue = pValue;
				if (strcmp(sValue.c_str(), pValue)==0)
					iNewValue=i;
			}
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::SetALSAConfigurationEntry: previous value for '%s' is '%s'", sEntry.c_str(), sOldValue.c_str());
			if (iNewValue!=-1)
				Entry.num_value=iNewValue;
			else
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Xine_Stream_Factory::SetALSAConfigurationEntry: failed to find the current value in possible enum values, not setting this variable");
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::SetALSAConfigurationEntry: previous value for '%s' is '%s'", sEntry.c_str(), Entry.str_value );		
			Entry.str_value = strdup(sValue.c_str());
		}
		
		xine_config_update_entry( m_pXineLibrary, &Entry );
		if (xine_config_lookup_entry(m_pXineLibrary, sEntry.c_str(), &Entry) == 0)
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::SetALSAConfigurationEntry: could not lookup the '%s' after update",
							   sEntry.c_str());
		}
		else
		{
			if (pEnumValues)
			{
				string sCurrentValue;
				for (int i=0; const char *pValue=pEnumValues[i]; i++)
				{
					if (Entry.num_value==i)
						sCurrentValue = pValue;
				}
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::SetALSAConfigurationEntry: current value for '%s' is '%s'", sEntry.c_str(), sCurrentValue.c_str());
			}
			else
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::SetALSAConfigurationEntry: current value for '%s' is '%s'",
					sEntry.c_str(), Entry.str_value );
			}
		}
	}
}

void Xine_Stream_Factory::RegisterALSAConfigurationString(string sEntryName, string sDefaultValue, string sDescription)
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::RegisterALSAConfigurationString: registered entry '%s' with default value '%s' and description '%s'",
				   sEntryName.c_str(), sDefaultValue.c_str(), sDescription.c_str());

	xine_config_register_string(m_pXineLibrary, sEntryName.c_str(), sDefaultValue.c_str(), sDescription.c_str(), NULL, 0, NULL, NULL);
}

void Xine_Stream_Factory::RegisterALSAConfigurationEnum(string sEntryName, int iDefaultValue, string sDescription, const char **pEnumValues)
{
	// collecting all values
	string sValues;
	string sDefaultValue;
	for (int i=0; const char *pValue=pEnumValues[i]; i++)
	{
		if (sValues=="")
			sValues += string("'") + pValue + "'";
		else
			sValues += string(", '") + pValue + "'";
		
		if (iDefaultValue==i)
			sDefaultValue = pValue;
	}
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Xine_Stream_Factory::RegisterALSAConfigurationEnum: registered entry '%s' with default value '%s', description '%s' and possible values {%s}", sEntryName.c_str(), sDefaultValue.c_str(), sDescription.c_str(), sValues.c_str());
	
	xine_config_register_enum(m_pXineLibrary, sEntryName.c_str(), iDefaultValue, (char **) pEnumValues, sDescription.c_str(), NULL, 0, NULL, NULL);
}

void Xine_Stream_Factory::setAudioSettings()
{
	string sAudioSettings = m_pPlayer->Get_MD_AudioSettings();

	if (sAudioSettings=="")
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "M/D Audio Settings are empty, assuming 'M'");
		sAudioSettings = "M";
	}
	else
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "M/D Audio Settings: %s", sAudioSettings.c_str());

	// temporary workaround for #3995
	if (sAudioSettings=="S3")
	{
	    LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "M/D Audio Settings are broken (S3), assuming 'S'");
	    sAudioSettings = "S";
	}
	
	bool updateConfig = true;
	string sAlsaDevice = "plug:dmix";
	string sSpeakersArrangement = "Stereo 2.0";
	string sPassthroughDevice = "iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2";
	bool bUsePassThrough = false;
	
	for (uint i=0; i<sAudioSettings.length(); i++)
	{
		switch (sAudioSettings[i])
		{
		case 'C':
		case 'O':
			sAlsaDevice = "asym_spdif";
			sPassthroughDevice = "iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2";
			break;

		case 'H':
			sAlsaDevice = "asym_hdmi";
			sPassthroughDevice = "hdmi:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2";
			break;
		
		case 'S':		
		case 'L':
			sAlsaDevice = "plug:dmix";
			break;
		
		case '3':
			sSpeakersArrangement = "Pass Through";
			bUsePassThrough = true;
			break;
		
		case 'M':
			updateConfig = false;
			break;
		
		default:
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Unknown audio settings flag: '%c'", sAudioSettings[i]);
		}
	}

	if (!updateConfig)
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Flag 'M' found, we won't override the defaults from /etc/pluto/xine.conf");
		return;
	}
	
	RegisterALSAConfigurationString("audio.device.alsa_front_device", "default", "device used for stereo output");
	SetALSAConfigurationEntry("audio.device.alsa_front_device", sAlsaDevice);
	
	RegisterALSAConfigurationString("audio.device.alsa_default_device", "default", "device used for mono output");
	SetALSAConfigurationEntry("audio.device.alsa_front_device", sAlsaDevice);
	
	if (bUsePassThrough)
	{
		RegisterALSAConfigurationString("audio.device.alsa_passthrough_device", "default", "device used for 5.1-channel passthrough output");
		SetALSAConfigurationEntry("audio.device.alsa_passthrough_device", sPassthroughDevice);
	}
	
	RegisterALSAConfigurationEnum("audio.output.speaker_arrangement", 1, "Speakers arrangement", audio_out_types_strs);
	SetALSAConfigurationEntry("audio.output.speaker_arrangement", sSpeakersArrangement, audio_out_types_strs);
}


// returns pointer to stream if exists/was created or NULL otherwise
Xine_Stream *Xine_Stream_Factory::GetStream(int streamID, bool createIfNotExist, int requestingObject, bool bBroadcasting, int iZoomLevel)
{
	{
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		
		if (!m_bInitialized)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Xine_Stream_Factory::GetStream factory is not yet initialized, not creating stream");
			return NULL;
		}
			
		
		if( streamID==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Xine_Stream_Factory::GetStream streamID is 0");
			streamID = m_iLastRenderingStream;
		}

		map<int, Xine_Stream*>::iterator stream = streamsMap.find(streamID);
		if (stream != streamsMap.end())
		{
			Xine_Stream *pStream = (*stream).second;
			pStream->StartupStream();
			return pStream;
		}
	}
		
	if (createIfNotExist && streamID!=0)
	{
		Xine_Stream *new_stream = new Xine_Stream(this, m_pXineLibrary, streamID, m_pPlayer->DATA_Get_Time_Code_Report_Frequency(), requestingObject, bBroadcasting);
		new_stream->m_bUseDeinterlacing = m_bUseDeinterlacing;
		new_stream->m_sDeinterlacingConfig = m_sDeinterlacingConfig;
		new_stream->m_iZoomLevel = iZoomLevel;
		if ((new_stream!=NULL) && new_stream->StartupStream())
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Created new stream with ID=%i deint %d %s %d %d", streamID, (int) new_stream->m_bUseDeinterlacing, new_stream->m_sDeinterlacingConfig.c_str(), (int) new_stream->m_iZoomLevel, bBroadcasting);
			
			PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
			streamsMap[streamID] = new_stream;
			return new_stream;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Cannot create new stream with ID=%i", streamID);
			if (new_stream!=NULL)
				delete new_stream;
			return NULL;
		}
	}
	else
		return NULL;
}


void Xine_Stream_Factory::ReportAudioTracks(string sTracks)
{
	m_pPlayer->DATA_Set_Audio_Tracks(sTracks);
}

void Xine_Stream_Factory::ReportSubtitles( string sSubtitles)
{
	m_pPlayer->DATA_Set_Subtitles(sSubtitles);
}

void Xine_Stream_Factory::ReportAVInfo( string sFilename, int iStreamID, string sMediaInfo, string sAudioInfo, string sVideoInfo )
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Player::EVENT_Playback_Started(streamID=%i) <= AV info", iStreamID);
	m_pPlayer->EVENT_Playback_Started(sFilename, iStreamID, sMediaInfo, sAudioInfo, sVideoInfo);
}

void Xine_Stream_Factory::ReportMediaDescription ( string sMediaDescription )
{
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Player::EVENT_Media_Description_Changed: %s", sMediaDescription.c_str());
    m_pPlayer->EVENT_Media_Description_Changed(sMediaDescription);
}

void Xine_Stream_Factory::ReportTimecodeViaIP(int iStreamID, int Speed)
{
	m_pPlayer->ReportTimecodeViaIP( iStreamID, Speed);
}

void Xine_Stream_Factory::DestroyStream(int iStreamID)
{
	Xine_Stream *pStream = NULL;
	map<int, Xine_Stream*>::iterator stream;
	
	{	
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		stream = streamsMap.find(iStreamID);
		
		if (stream != streamsMap.end())
		{
			pStream = (*stream).second;
			streamsMap.erase(stream);
		}
	}
	
	if (pStream)
	{
		delete pStream;
	}
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Destroyed stream with ID=%i", iStreamID);
}

void Xine_Stream_Factory::HideStreamWindows(int iStreamID)
{
	Xine_Stream *pStream = NULL;
	map<int, Xine_Stream*>::iterator stream;
	
	{	
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		stream = streamsMap.find(iStreamID);
		
		if (stream != streamsMap.end())
		{
			pStream = (*stream).second;
		}
	}
	
	if (pStream)
	{
		pStream->hideWindows();
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Hide windows for stream with ID=%i", iStreamID);
}

void Xine_Stream_Factory::CloseStreamAV(int iStreamID)
{
	Xine_Stream *pStream = NULL;
	map<int, Xine_Stream*>::iterator stream;
	
	{	
		PLUTO_SAFETY_LOCK( factoryLock, m_factoryMutex );
		stream = streamsMap.find(iStreamID);
		
		if (stream != streamsMap.end())
		{
			pStream = (*stream).second;
		}
	}
	
	if (pStream)
	{
		pStream->ShutdownStream();
	}
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Closed stream AV with ID=%i", iStreamID);
}

void Xine_Stream_Factory::setVideoDriver(string strVideoDriver)
{

        if (strVideoDriver!="Manual")
        {
                LoggerWrapper::GetInstance()->Write( LV_STATUS, "Overriding video.driver setting in xine.conf, using '%s' from web admin for video output", strVideoDriver.c_str());
                m_sXineVideoDriverName = strVideoDriver;
        }
}


// creates stream windows
bool Xine_Stream_Factory::CreateWindows()
{
	XSizeHints sizeHints;
	MWMHints wmHints;
	XClassHint classHint;

	long propertyValues[ 1 ];
	int completionEvent;
	int xpos, ypos, width, height;

	if ( ( m_pXDisplay = XOpenDisplay( getenv( "DISPLAY" ) ) ) == NULL )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Could not open X DISPLAY from: %s", getenv( "DISPLAY" ) );
		return false;
	}
	
	//XSynchronize( m_pXDisplay, true);

	XLockDisplay( m_pXDisplay );

	m_iCurrentScreen = XDefaultScreen( m_pXDisplay );
	// creating it on last (invisible) screen
	int iScreens = XScreenCount( m_pXDisplay );
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Count of screens at this display: %i", iScreens);
	//m_iCurrentScreen = iScreens-1;
	xpos = 10;
	ypos = 20;
	width = 100;
	height = 100;
	int border = 0;

	windows[ 0 ] = XCreateSimpleWindow( m_pXDisplay, XDefaultRootWindow( m_pXDisplay ), xpos, ypos, width, height, border, 0, 0 );
	windows[ 1 ] = XCreateSimpleWindow( m_pXDisplay, XDefaultRootWindow( m_pXDisplay ), xpos, ypos, width, height, border, 0, 0 );

/*
	windows[ 0 ] = XCreateSimpleWindow( m_pXDisplay, XRootWindow( m_pXDisplay, m_iCurrentScreen ), xpos, ypos, width, height, 
																			1, 0, 0 );
	windows[ 1 ] = XCreateSimpleWindow( m_pXDisplay, XRootWindow( m_pXDisplay, m_iCurrentScreen ),
																			0, 0, ( DisplayWidth( m_pXDisplay, m_iCurrentScreen ) ), DisplayHeight( m_pXDisplay, m_iCurrentScreen ),
																			0, 0, 0 );
*/
	classHint.res_name = ( char* ) m_sWindowTitle.c_str();
	classHint.res_class = ( char* ) m_sWindowTitle.c_str();
	XSetClassHint ( m_pXDisplay, windows[ 0 ], &classHint );

	classHint.res_class = ( char* ) m_sWindowTitle.c_str();
	XSetClassHint ( m_pXDisplay, windows[ 0 ], &classHint );

	XSelectInput( m_pXDisplay, windows[ 0 ], INPUT_MOTION );
	XSelectInput( m_pXDisplay, windows[ 1 ], INPUT_MOTION );

	XSetStandardProperties( m_pXDisplay, windows[ 0 ], m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0 );
	XSetStandardProperties( m_pXDisplay, windows[ 1 ], "pluto-stub", "pluto-stub", None, NULL, 0, 0 );

	int WidthMM = DisplayWidthMM(m_pXDisplay, m_iCurrentScreen);
	int HeightMM = DisplayHeightMM(m_pXDisplay, m_iCurrentScreen);

	// Projectors can return a physical size of 0x0 mm. This prevents a division by zero when 0x0 occurs.
	if (WidthMM == 0)
		WidthMM = 1;
	if (HeightMM == 0)
		HeightMM = 1;

	// calculating pixel aspect
	double res_h = DisplayWidth(m_pXDisplay, m_iCurrentScreen) * 1000 / WidthMM;
	double res_v = DisplayHeight(m_pXDisplay, m_iCurrentScreen) * 1000 / HeightMM;

	m_dScreenPixelAspect = res_v / res_h;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "XServer screen aspect %f", m_dScreenPixelAspect );

	if ( fabs( m_dScreenPixelAspect - 1.0 ) < 0.01 )
		m_dScreenPixelAspect = 1.0;
	
	// changing wm hints
	sizeHints.win_gravity = StaticGravity;
	sizeHints.flags = PPosition | PSize | PWinGravity;

	XSetWMNormalHints( m_pXDisplay, windows[ 0 ], &sizeHints );
	XSetWMNormalHints( m_pXDisplay, windows[ 1 ], &sizeHints );

	Atom XA_DELETE_WINDOW = XInternAtom( m_pXDisplay, "WM_DELETE_WINDOW", False );
	XSetWMProtocols( m_pXDisplay, windows[ 0 ], &XA_DELETE_WINDOW, 1 );
	XSetWMProtocols( m_pXDisplay, windows[ 1 ], &XA_DELETE_WINDOW, 1 );

	Atom XA_NO_BORDER = XInternAtom( m_pXDisplay, "_MOTIF_WM_HINTS", False );
	wmHints.flags = ( 1L << 1 ); // MWM_HINTS_DECORATIONS
	wmHints.decorations = 0;

	XChangeProperty( m_pXDisplay, windows[ 1 ], XA_NO_BORDER, XA_NO_BORDER, 32,
									 PropModeReplace, ( unsigned char * ) & wmHints,
									 5 /* PROP_MWM_HINTS_ELEMENTS */ );

	Atom XA_WIN_LAYER = XInternAtom( m_pXDisplay, "_NET_WM_STATE", False );
	propertyValues[ 0 ] = 10;
	XChangeProperty( m_pXDisplay, windows[ 1 ], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, ( unsigned char * ) propertyValues, 1 );


	XA_WIN_LAYER = XInternAtom( m_pXDisplay, "_NET_WM_STATE", False );
	propertyValues[ 0 ] = 12;
	XChangeProperty( m_pXDisplay, windows[ 1 ], XA_WIN_LAYER, XA_CARDINAL, 32, PropModeReplace, ( unsigned char * ) propertyValues, 1 );

	if ( XShmQueryExtension( m_pXDisplay ) == True )
		completionEvent = XShmGetEventBase( m_pXDisplay ) + ShmCompletion;
	else
		completionEvent = -1;

	int xcode = XMapWindow( m_pXDisplay, windows[ 0 ] );
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "XMapWindow returned: %i", xcode);

//	XIconifyWindow( m_pXDisplay, windows[ 0 ], 0 );

	XUnlockDisplay( m_pXDisplay );
        XSync(m_pXDisplay, false);

//<-mkr_B_via_b->
#ifndef VIA
        WMControllerImpl *pWMController = new WMControllerImpl();

	bool bResult = pWMController->SetVisible(m_sWindowTitle, false);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetVisible returned %d", bResult);

	if(!bResult)
	{
		//failed to move the window, try again in half of second
		Sleep(500);

		bResult = pWMController->SetVisible(m_sWindowTitle, false);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetVisible returned %d", bResult);
	}	

	delete pWMController;
	pWMController = NULL;
#endif
//<-mkr_B_via_e->

	return true;
}

bool Xine_Stream_Factory::DestroyWindows()
{
	if ( m_pXDisplay != NULL )
	{
//		if (!m_pDynamic_Pointer)
//			delete m_pDynamic_Pointer;
//		m_pDynamic_Pointer = NULL;
		
		XLockDisplay( m_pXDisplay );

		XUnmapWindow( m_pXDisplay, windows[ m_iCurrentWindow ] );

//		XFreeCursor( m_pXDisplay, cursors[ 0 ] );
//		XFreeCursor( m_pXDisplay, cursors[ 1 ] );

		XDestroyWindow( m_pXDisplay, windows[ 0 ] );
		XDestroyWindow( m_pXDisplay, windows[ 1 ] );

		XUnlockDisplay( m_pXDisplay );
		XCloseDisplay ( m_pXDisplay );

		m_pXDisplay = NULL;
	}

	return true;
}

void Xine_Stream_Factory::IdentifyXineStuff()
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Checking installed xine-lib capabilities" );
	// check if we have xine library with xine_seek support or not (reusing the loaded libxine)
	void *handle = dlopen("libxine.so.2", RTLD_LAZY | RTLD_NOLOAD);
	if (handle)
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "OK, libxine.so.2 already loaded" );
		*(void**)(&custom_xine_seek) = dlsym(handle, "xine_seek");
		*(void**)(&custom_xine_start_trick_play) = dlsym(handle, "xine_start_trick_play");
		*(void**)(&custom_xine_stop_trick_play) = dlsym(handle, "xine_stop_trick_play");
		 g_bXINE_HAS_TRICKPLAY_SUPPORT = ( custom_xine_seek && custom_xine_start_trick_play && custom_xine_stop_trick_play);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Warning: libxine.so.2 is not loaded yet (?)" );
	}
	
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Custom xine functions status:  xine_seek=%p, xine_start_trick_play=%p, xine_stop_trick_play=%p. Trickplay and seeking support: %s", 
		custom_xine_seek, custom_xine_start_trick_play, custom_xine_stop_trick_play, g_bXINE_HAS_TRICKPLAY_SUPPORT?"present":"absent");
//
}

void Xine_Stream_Factory::SendMessageToOrbiter(string sMessage)
{
  if (m_pPlayer)
    m_pPlayer->SendMessageToOrbiter(sMessage);
}

} // DCE namespace end
