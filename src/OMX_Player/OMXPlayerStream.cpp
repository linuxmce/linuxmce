#include "OMX_Player.h"
#include "OMXPlayerStream.h"
#include "OMXPlayerInterface.h"


namespace DCE
{

OMXPlayerStream::OMXPlayerStream(string sAudioDevice, bool bPassthrough, string sGpuDeInt, OMX_Player *pOwner) :
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
	cout << "*** [StateNotifier] - " << PlayerState << endl;
}

} // DCE namespace
