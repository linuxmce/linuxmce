#include "OMXPlayerStream.h"
#include "OMXPlayerInterface.h"

OMXPlayerStream::OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, const void *pInstance) :
	OMXPlayerInterface(sAudioDevice, bPassthrough, sGpuDeInt)
{
//  m_opOMX_Player = (DCE::OMX_Player)pInstance;
}

OMXPlayerStream::~OMXPlayerStream()
{
}

// inherited from OMXPlayerInterface
void OMXPlayerStream::PlayerStateNotifier(OMXPlayerInterface::State PlayerState)
{
  // do something with PlayerState
}

