#include "OMX_Player.h"
#include "OMXPlayerStream.h"
#include "OMXPlayerInterface.h"


namespace DCE
{

OMXPlayerStream::OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pOwner) :
	OMXPlayerInterface(sAudioDevice, bPassthrough, sGpuDeInt)
{
	m_pOwner = pOwner;
}

OMXPlayerStream::~OMXPlayerStream()
{
}

void OMXPlayerStream::Log(string txt) {
//	m_mtxLog.lock();
	m_pOwner->Log(txt);
	//cout << txt;
//	m_mtxLog.unlock();
}

// inherited from OMXPlayerInterface
void OMXPlayerStream::PlayerStateNotifier(OMXPlayerInterface::State PlayerState)
{
	// do something with PlayerState
	cout << "*** [StateNotifier] - " << PlayerState << endl;
	m_pOwner->StateChanged(PlayerState, m_pOwner->m_sMediaURL, m_pOwner->m_iStreamID, false);
	//EVENT_Playback_Completed(pThis->m_filename,pThis->m_iStreamID,false);
}

} // DCE namespace
