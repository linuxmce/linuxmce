#include "../PlutoUtils/StringUtils.h"

#include "OMX_Player.h"
#include "OMXPlayerStream.h"

#include "utilities/linux/window_manager/WMController/WMController.h"
#include "WindowUtils/WindowUtils.h"

namespace DCE
{

OMXPlayerStream::OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pPlayer, int iTimeCodeReportFrequency) :
	OMXPlayerInterface(sAudioDevice, bPassthrough, sGpuDeInt),
	m_pPlayer(pPlayer)
{
	threadEventLoop = 0;
	m_iTimeCodeReportFrequency = iTimeCodeReportFrequency;

	m_sWindowTitle = "pluto-omx-playback-window";
}

OMXPlayerStream::~OMXPlayerStream()
{
}

bool OMXPlayerStream::Init()
{
	XInitThreads();
	if ((m_pDisplay = XOpenDisplay(getenv("DISPLAY"))) == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OMXPlayerStream::Init() - Could not open X display from %s",getenv("DISPLAY"));
		return false;
	}

	if (!CreateWindow())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OMXPlayerStream::Init() could not create target window.");
		return false;
	}

	if (!Minimize())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "OMXPlayerStream::Init() could not minimize VLC window.");
		// return false;
	}
	return true;
}



bool OMXPlayerStream::CreateWindow()
{
	int xpos, ypos, width, height, border;
	XSizeHints sizeHints;
	XClassHint classHint;

	xpos=10;
	ypos=20;
	width=100;
	height=100;
	border=0;

	XLockDisplay(m_pDisplay);

	m_Window = XCreateSimpleWindow(m_pDisplay, XDefaultRootWindow(m_pDisplay), xpos, ypos, width, height, border, 0, 0);

	classHint.res_name = ( char* ) m_sWindowTitle.c_str();
	classHint.res_class = ( char* ) m_sWindowTitle.c_str();
	XSetClassHint ( m_pDisplay, m_Window, &classHint );

	classHint.res_class = ( char* ) m_sWindowTitle.c_str();
	XSetClassHint ( m_pDisplay, m_Window, &classHint );

	XSetStandardProperties( m_pDisplay, m_Window, m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0 );

	// changing wm hints
	sizeHints.win_gravity = StaticGravity;
	sizeHints.flags = PPosition | PSize | PWinGravity;

	XSetWMNormalHints( m_pDisplay, m_Window, &sizeHints );
	Atom XA_DELETE_WINDOW = XInternAtom( m_pDisplay, "WM_DELETE_WINDOW", False );
	XSetWMProtocols( m_pDisplay, m_Window, &XA_DELETE_WINDOW, 1 );

	int xcode = XMapWindow( m_pDisplay, m_Window );
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "XMapWindow returned: %i", xcode);

	XSync(m_pDisplay, false);
	XUnlockDisplay(m_pDisplay);

	return true;
  }

bool OMXPlayerStream::Minimize()
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OMXPlayerStream::Minimize() - attempting to minimize OMX window.");

	if (!m_pDisplay)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OMXPlayerStream::Minimize() - no display set.");
		return false;
	}

	WMControllerImpl *pWMController = new WMControllerImpl();
	bool bResult = pWMController->SetVisible("pluto-omx-playback-window.pluto-omx-playback-window", false);

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "OMXPlayerStream::Minimize()- WMController SetVisible returned %d", bResult);

	if(!bResult)
	{
		//failed to move the window, try again in half of second
		Sleep(500);

		bResult = pWMController->SetVisible("pluto-omx-playback-window.pluto-omx-playback-window", false);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "OMXPlayerStream::Minimize() - returned %d", bResult);
		return true;
	}

	delete pWMController;
	pWMController = NULL;

	return true;
}

/*****************************************************/

// inherited from OMXPlayerInterface
void OMXPlayerStream::Stop(int iStreamID) {
//	Log("OMXPlayerStream::Stop - iStreamID: " + to_string(iStreamID));

	// stop the event thread first
	if ( threadEventLoop != 0)
	{
		Log("OMXPlayerStream::Stop - Stopping event processor." );

		{
//			PLUTO_SAFETY_LOCK(streamLock, m_streamMutex);
			m_bExitThread = true;
		}

		pthread_join( threadEventLoop, NULL );
		Log("OMXPlayerStream::Stop - Stopped event processor." );
		threadEventLoop = 0;
}


	STATE state = Get_PlayerState();
	if ( iStreamID == m_iStreamID || iStreamID == 0)
		if ( state != STATE::STOPPING && state != STATE::STOPPED ) {
			Log("OMXPlayerStream::Stop - Stopping -- iStreamID: " + to_string(iStreamID));
			OMXPlayerInterface::Stop();
		}
}

void OMXPlayerStream::SetDeviceData_SubtitleTracks() {
	// Get/Set Subtitles DD
	std::string sSubtitles("");
	sSubtitles = std::to_string(OMXPlayerInterface::getCurrentSubtitle()) + "\n";
	std::vector< std::string > vsSubtitles;
	vsSubtitles = OMXPlayerInterface::Get_ListSubtitles();
	for(std::vector<std::string>::iterator it = vsSubtitles.begin(); it < vsSubtitles.end(); it++)
	{
		string::size_type tokenPos=0;
		// <index>:<language>:<name>:<codec>:<active>
		string index = StringUtils::Tokenize(*it, ":", tokenPos);
		string language = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string name = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string codec = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string active = StringUtils::Tokenize(*it, string(":"), tokenPos);

		sSubtitles += index + " " + language + " " + name + " " + codec + "\n";
		//Log("OMXPlayerStream::Play - Adding Subtitle : " + index + " " + language + " " + name + " " + codec);
	}
	Log("OMXPlayerStream::Play - DATA_Set_Subtitles : " + sSubtitles );
	m_pPlayer->DATA_Set_Subtitles(sSubtitles);
}

void OMXPlayerStream::SetDeviceData_AudioTracks() {
	// Get/Set Audio DD
	std::string sAudio("");
	sAudio = std::to_string(OMXPlayerInterface::getCurrentAudio()) + "\n";
	std::vector< std::string > vsAudio;
	vsAudio = OMXPlayerInterface::Get_ListAudio();
	for(std::vector<std::string>::iterator it = vsAudio.begin(); it < vsAudio.end(); it++)
	{
		string::size_type tokenPos=0;
		// <index>:<language>:<name>:<codec>:<active>
		string index = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string language = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string name = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string codec = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string active = StringUtils::Tokenize(*it, string(":"), tokenPos);

		sAudio += index + " " + language + " " + name + " " + codec + "\n";
		//Log("OMXPlayerStream::Play - Adding Audio Track : " + index + " " + language + " " + name + " " + codec);
	}
	Log("OMXPlayerStream::Play - DATA_Set_Audio_Tracks : " + sAudio );
	m_pPlayer->DATA_Set_Audio_Tracks(sAudio);
}

void OMXPlayerStream::SetDeviceData_VideoTracks() {
	// Get/Set Video DD
	std::string sVideo("");
	sVideo = std::to_string(OMXPlayerInterface::getCurrentVideo()) + "\n";
	std::vector< std::string > vsVideo;
	vsVideo = OMXPlayerInterface::Get_ListVideo();
	for(std::vector<std::string>::iterator it = vsVideo.begin(); it < vsVideo.end(); it++)
	{
		string::size_type tokenPos=0;
		// <index>:<language>:<name>:<codec>:<active>
		string index = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string language = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string name = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string codec = StringUtils::Tokenize(*it, string(":"), tokenPos);
		string active = StringUtils::Tokenize(*it, string(":"), tokenPos);

		sVideo += index + " " + language + " " + name + " " + codec + "\n";
		//Log("OMXPlayerStream::Play - Adding Audio Track : " + index + " " + language + " " + name + " " + codec);
	}
	Log("OMXPlayerStream::Play - DATA_Set_Angles : " + sVideo );
	m_pPlayer->DATA_Set_Angles(sVideo);

}

// inherited from OMXPlayerInterface
bool OMXPlayerStream::Play(int iStreamID, string sMediaURL, string sMediaPosition, int iMediaType) {
	if ( Get_PlayerState() != STATE::STOPPED ) {
		Log("OMXPlayerStream::Play - ERROR - Can't Play() unless STOPPED, sending Stop()");
		bEvent = false;
		OMXPlayerInterface::Stop();
		while ( Get_PlayerState() != STATE::STOPPED) usleep(100);
	}

	bEvent = true;
	m_iStreamID = iStreamID;
	m_sMediaURL = sMediaURL;
	m_iMediaType = iMediaType;

	if ( !OMXPlayerInterface::Play(sMediaURL, sMediaPosition) )
		return false;

	m_xDuration = OMXPlayerInterface::Get_Duration();

	if (threadEventLoop == 0)
	{
		Log("OMXPlayerStream::Play - Creating event processor" );
		pthread_create( &threadEventLoop, NULL, EventProcessingLoop, this );
		Log("OMXPlayerStream::Play - Event processor started" );
	}
	else
		Log("OMXPlayerStream::Play - Event processor not false?!?!?" );

	SetDeviceData_SubtitleTracks();

	SetDeviceData_AudioTracks();

	SetDeviceData_VideoTracks();

	return true;
}


void OMXPlayerStream::Log(string txt) {
	m_mtxLog.lock();
	if (m_pPlayer) {
		m_pPlayer->Log(txt);
	}
	else
		cout << "*** " << txt << endl;
	m_mtxLog.unlock();
}

// inherited from OMXPlayerInterface
void OMXPlayerStream::PlayerStateNotifier(OMXPlayerInterface::STATE playerState)
{
	int iStreamID;
	string sMediaURL;

	iStreamID = m_iStreamID;
	sMediaURL = m_sMediaURL;

	// do something with PlayerState
	Log("OMXPlayerStream::PlayerStateNotifier - " + to_string((int)playerState));
	if (m_pPlayer)
	{
		m_pPlayer->StateChanged(playerState, sMediaURL, iStreamID, bEvent);
	}
	else
		Log("OMXPlayerStream::PlayerStateNotifier - no m_pPlayer");
}

string OMXPlayerStream::GetPosition()
{
	string sPosition;

	if( m_iChapter!=-1 )
		sPosition += " CHAPTER:" + StringUtils::itos(m_iChapter);

	int currentTime = Get_Position() / 1000;
	int totalTime = m_xDuration / 1000;
	sPosition += " POS:" + StringUtils::itos(currentTime);

	if( m_iTitle!=-1 )
		sPosition += " TITLE:" + StringUtils::itos(m_iTitle);

	sPosition += " SUBTITLE:" + StringUtils::itos(Get_Subtitle());
	sPosition += " AUDIO:" + StringUtils::itos(Get_Audio());
	sPosition += " TOTAL:" + StringUtils::itos(totalTime);

	return sPosition;
}

void OMXPlayerStream::ReportTimecode()
{
//	Log("OMXPlayerStream::ReportTimecode() - called.");

	STATE state = Get_PlayerState();
	if ( state != STATE::PLAYING )
	{
		Log("OMXPlayerStream::ReportTimecode - skipping report as we're not PLAYING");
		return;
	}
	// FIXME: CHECK STATE
//	if (!m_bInitialized)
//	{
//		LoggerWrapper::GetInstance()->Write( LV_WARNING, "ReportTimecode called on non-initialized stream - aborting command");
//		return;
//	}

//	if (!m_iTimeCodeReportFrequency||!m_bIsRendering )
	if (!m_iTimeCodeReportFrequency )
	       return;

//	if( m_bIsVDR )
//	{
//		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ignoring vdr timecode for now");
//		return;
//	}
	int iSpeed = Get_Speed() / 1000;

	m_pPlayer->ReportTimecodeViaIP( m_iStreamID, iSpeed);
//	Log("OMXPlayerStream::ReportTimecode() - Timecode reported.");
}

int OMXPlayerStream::CalculatePosition(string &sMediaPosition,string *sMRL,int *Subtitle,int *Angle,int *AudioTrack)
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

int OMXPlayerStream::Get_iStreamID() {
	return m_iStreamID;
}

string OMXPlayerStream::Get_MediaType() {
	// this is the string version of the MediaID, not to be confused with the lmce MediaType.
	return m_sMediaType;
}

int OMXPlayerStream::Get_MediaID() {
	return m_iMediaID;
}

bool OMXPlayerStream::GetScreenShot(int iWidth, int iHeight, string& sCMD_Result)
{
	string cmd("");
	cmd += "raspi2png ";
	cmd += "--pngname /tmp/shot.png ";
	cmd += "--width " + to_string(iWidth) + " ";
	cmd += "--height " + to_string(iHeight) + " ";
	if (system(cmd.c_str()) != 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OMXPlayerStream::GetScreenShot(): Could not take snapshot.");
		sCMD_Result="ERROR";
		return false;
	}
	sCMD_Result="OK";
	return true;
}

bool OMXPlayerStream::setVolumeUp(int iRepeat_Command, string &sCMD_Result)
{
	OMXPlayerInterface::Do_UnMute();
	OMXPlayerInterface::Do_IncreaseVolume();
	for (int repeats = iRepeat_Command - 1; repeats > 1; --repeats)
	{
		// FIXME: get the repeat delay from the db?
		sleep(50);
		OMXPlayerInterface::Do_IncreaseVolume();
	}
	sCMD_Result="OK";
	return true;
}

bool OMXPlayerStream::setVolumeDown(int iRepeat_Command, string &sCMD_Result)
{
	OMXPlayerInterface::Do_DecreaseVolume();
	for (int repeats = iRepeat_Command - 1; repeats > 1; --repeats)
	{
		// FIXME: get the repeat delay from the db?
		sleep(50);
		OMXPlayerInterface::Do_DecreaseVolume();
	}
	sCMD_Result="OK";
	return true;
}

bool OMXPlayerStream::setMute(string &sCMD_Result)
{
	OMXPlayerInterface::Do_Mute();
	sCMD_Result="OK";
	return true;
}

bool OMXPlayerStream::setUnMute(string &sCMD_Result)
{
	OMXPlayerInterface::Do_UnMute();
	sCMD_Result="OK";
	return true;
}

/*
int OMXPlayerStream::setVideo(int track) {
	if ( track < ( OMXPlayerInterface::getMaxVideo() ) )
		if ( OMXPlayerInterface::setVideo(track) ) {
			SetDeviceData_VideoTracks();
			m_pPlayer->EVENT_Angle_Changed( sID, m_iStreamID, sName, sFormat, sLanguage );
			return track;
		}
	return -1;
}
*/

int OMXPlayerStream::setAudio(int track) {
	int iMaxAudio = OMXPlayerInterface::getMaxAudio();
	if ( track >= iMaxAudio || track < 0)
		track = 0;

	// TODO: Display current audio lang on all bound orbiter displays
	string sID = StringUtils::itos(track);
	string sName = "";
	string sFormat = "";
	string sLanguage = "";

	Log("OMXPlayerStream::setAudio - " + to_string(track) );
	if ( OMXPlayerInterface::setAudio(track) ) {
		SetDeviceData_AudioTracks();
		m_pPlayer->EVENT_Audio_Track_Changed( sID, m_iStreamID, sName, sFormat, sLanguage );
		return track;
	}
	m_pPlayer->EVENT_Audio_Track_Changed( sID, m_iStreamID, sName, sFormat, sLanguage );
	Log("OMXPlayerStream::setAudio - track not available");
	return -1;
}


int OMXPlayerStream::getCurrentAudioTrack() {

	return OMXPlayerInterface::getCurrentAudio();
}

int OMXPlayerStream::getCurrentSubtitle() {
	if ( !OMXPlayerInterface::getSubtitlesShowing() ) return -1;

	return OMXPlayerInterface::getCurrentSubtitle();
}

int OMXPlayerStream::setSubtitle(int track) {
//	if ( !OMXPlayerInterface::getSubtitlesShowing() ) track = 0;

	// TODO: Display current subtitle lang on all bound orbiter displays
	string sID = StringUtils::itos(track);
	string sName = "";
	string sFormat = "";
	string sLanguage = "";

	int numsubs = OMXPlayerInterface::getMaxSubtitle();
	Log("OMXPlayerStream::setSubtitle - " + to_string(track) + " of " + to_string(numsubs) );
	if ( track < numsubs ) {
		if ( OMXPlayerInterface::setSubtitle(track) )
		{
			Log("OMXPlayerStream::setSubtitle - track available, subtitles showing");
			OMXPlayerInterface::ShowSubtitles();
			SetDeviceData_SubtitleTracks();
			m_pPlayer->EVENT_Subtitle_Track_Changed( sID, m_iStreamID, sName, sFormat, sLanguage );
			return track;
		}
	}

	track = -1;
	sID = StringUtils::itos(track);
	OMXPlayerInterface::HideSubtitles();
	Log("OMXPlayerStream::setSubtitle - track not available, subtitles hidden");
	m_pPlayer->EVENT_Subtitle_Track_Changed( sID, m_iStreamID, sName, sFormat, sLanguage );
	return track;
}


void *EventProcessingLoop( void *pInstance )
{
	OMXPlayerStream * pStream = ( OMXPlayerStream* ) pInstance;

//	bool checkResult;

	// counter for timecode report
	int iCounter_TimeCode = 0;

	// 1/10th second interval counter
	int iCounter = 0; //, jCounter = 0;

//	XEvent event;
	while ( ! pStream->m_bExitThread )
	{
/*
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
*/
		// updating every second - position
		if ( ++iCounter >= 10 )
		{
//			if (pStream->m_bInitialized)
//			{
//				LoggerWrapper::GetInstance()->Write( LV_WARNING, "[ID: %d] %s (seek %d) t.c. ctr %d freq %d,", pStream->m_iStreamID, pStream->GetPosition().c_str(), pStream->m_iSpecialSeekSpeed, iCounter_TimeCo$
//			}
			iCounter = 0;

			//if it is a time - reporting our timecode to player object
			if ( pStream->m_iTimeCodeReportFrequency && ++iCounter_TimeCode >= pStream->m_iTimeCodeReportFrequency )
			{
				pStream->ReportTimecode();
				iCounter_TimeCode = 1;
			}
		}

/*
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
*/
/*
		//updating time and speed when @trickplay mode
		if (pStream->m_bTrickModeActive)
		{
			//only every 0.2s
			if (iCounter%2==0)
			{
				pStream->DisplaySpeedAndTimeCode();
			}
		}
*/

// FIXME: Add this
/*
		//updating time and speed when paused
		if ( pStream->Get_Speed==0)
		{
			//only every 0.5s
			if (iCounter%5==0)
			{
				pStream->DisplaySpeedAndTimeCode();
			}
		}
*/
/*
		if ( pStream->m_iSpecialSeekSpeed )
			pStream->HandleSpecialSeekSpeed();
*/
		usleep( 100000 );

	}

	return NULL;
}

/*
void OMXPlayerStream::DisplaySpeedAndTimeCode()
{

//	if (!m_bInitialized)
//	{
//		LoggerWrapper::GetInstance()->Write( LV_WARNING, "DisplaySpeedAndTimeCode called on non-initialized stream - aborting command");
//		return;
//	}

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
*/

} // DCE namespace
