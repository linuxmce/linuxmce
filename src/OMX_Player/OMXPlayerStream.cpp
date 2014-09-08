#include "OMX_Player.h"
#include "OMXPlayerStream.h"


namespace DCE
{

OMXPlayerStream::OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pPlayer) :
	OMXPlayerInterface(sAudioDevice, bPassthrough, sGpuDeInt),
	m_pPlayer(pPlayer)
{
}

OMXPlayerStream::~OMXPlayerStream()
{
}


// inherited from OMXPlayerInterface
void OMXPlayerStream::Stop(int iStreamID) {
//	Log("OMXPlayerStream::Stop - iStreamID: " + to_string(iStreamID));
	STATE state = Get_PlayerState();
	if ( iStreamID == m_iStreamID || iStreamID == 0)
		if ( state != STATE::STOPPING && state != STATE::STOPPED ) {
			Log("OMXPlayerStream::Stop - Stopping -- iStreamID: " + to_string(iStreamID));
			OMXPlayerInterface::Stop();
		}
}

// inherited from OMXPlayerInterface
bool OMXPlayerStream::Play(int iStreamID, string sMediaURL, string sMediaPosition) {
	if ( Get_PlayerState() != STATE::STOPPED ) {
		Log("OMXPlayerStream::Play - ERROR - Can't Play() unless STOPPED, sending Stop()");
		bEvent = false;
		OMXPlayerInterface::Stop();
		while ( Get_PlayerState() != STATE::STOPPED) usleep(100);
	}

	bEvent = true;
	m_iStreamID = iStreamID;
	m_sMediaURL = sMediaURL;

	if ( !OMXPlayerInterface::Play(sMediaURL, sMediaPosition) )
		return false;

	m_xDuration = OMXPlayerInterface::Get_Duration();

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
	// FIXME: CHECK STATE
//	if (!m_bInitialized)
//	{
//		LoggerWrapper::GetInstance()->Write( LV_WARNING, "ReportTimecode called on non-initialized stream - aborting command");
//		return;
//	}

//	if (!m_iTimeCodeReportFrequency||!m_bIsRendering )
//	       return;

//	if( m_bIsVDR )
//	{
//		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ignoring vdr timecode for now");
//		return;
//	}
	int iSpeed = Get_Speed() / 1000;

	m_pPlayer->ReportTimecodeViaIP( m_iStreamID, iSpeed);
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
	string ret = "N";
	// FIXME: get the type and return it
	return ret;
}

int OMXPlayerStream::Get_MediaID() {
	int ret = 0;
	// FIXME: get this data somehow...
	return ret;
}


} // DCE namespace
