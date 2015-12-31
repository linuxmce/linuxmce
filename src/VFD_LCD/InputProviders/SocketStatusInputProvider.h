#ifndef __SOCKET_STATUS_INPUT_PROVIDER_H__
#define __SOCKET_STATUS_INPUT_PROVIDER_H__
//--------------------------------------------------------------------------------------------------------
#include "DCE/SocketListener.h"
//--------------------------------------------------------------------------------------------------------
#include "../LCDLogic/IInputProvider.h"
//--------------------------------------------------------------------------------------------------------
class SocketStatusInputProvider : public IInputProvider, public DCE::SocketListener
{
public:

	SocketStatusInputProvider(IInputProcessor *pInputProcessor, int nPort);
	~SocketStatusInputProvider();

	bool ReceivedString(DCE::Socket *pSocket, string sLine, int nTimeout = -1);
	void ReceivedMessage(DCE::Socket *pSocket, DCE::Message* pMessage);
};
//--------------------------------------------------------------------------------------------------------
#endif //__SOCKET_STATUS_INPUT_PROVIDER_H__
