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
	Log("OMXPlayerStream::Stop - iStreamID: " + to_string(iStreamID));
	STATE state = Get_PlayerState();
	if ( iStreamID == m_iStreamID || iStreamID == 0)
		if ( state != STATE::STOPPING && state != STATE::STOPPED ) {
			Log("OMXPlayerStream::Stop - Stopping -- iStreamID: " + to_string(iStreamID));
			OMXPlayerInterface::Stop();
		}
}

// inherited from OMXPlayerInterface
bool OMXPlayerStream::Play(int iStreamID, string sMediaURL) {
	if ( Get_PlayerState() != STATE::STOPPED ) {
		Log("OMXPlayerStream::Play - ERROR - Can't Play() unless STOPPED, sending Stop()");
		bEvent = false;
		OMXPlayerInterface::Stop();
		while ( Get_PlayerState() != STATE::STOPPED) usleep(100);
	}

	bEvent = true;
	m_iStreamID = iStreamID;
	m_sMediaURL = sMediaURL;

	if ( !OMXPlayerInterface::Play(sMediaURL) )
		return false;

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

} // DCE namespace
