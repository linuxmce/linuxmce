#include "OMXPlayer.h"
#include "OMXPlayerInterface.h"

OMXPlayer::OMXPlayer(string sAudioDevice, bool bPassthrough, string sGpuDeInt, const void *pInstance) :
	OMXPlayerInterface(sAudioDevice, bPassthrough, sGpuDeInt)
{
//  m_opOMX_Player = (DCE::OMX_Player)pInstance;
}

/*
OMXPlayer::OMXPlayer(string sAudioDevice, bool bPassthrough, string sGpuDeInt) : 
	OMXPlayerInterface(sAudioDevice, bPassthrough, sGpuDeInt)
{
}

*/

OMXPlayer::~OMXPlayer()
{
}


void OMXPlayer::PlayerStateNotifier(OMXPlayerInterface::State PlayerState)
{
  // do something with PlayerState
}

