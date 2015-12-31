#include "SocketStatusInputProvider.h"

#include "PlutoUtils/StringUtils.h"
//--------------------------------------------------------------------------------------------------------
SocketStatusInputProvider::SocketStatusInputProvider(IInputProcessor *pInputProcessor, int nPort) :
	IInputProvider(pInputProcessor), DCE::SocketListener("Status Listener")
{
	StartListening(nPort);
}
//--------------------------------------------------------------------------------------------------------
SocketStatusInputProvider::~SocketStatusInputProvider()
{

}
//--------------------------------------------------------------------------------------------------------
bool SocketStatusInputProvider::ReceivedString(DCE::Socket *pSocket, string sLine, int /*nTimeout*/)
{
	pSocket->SendString("OK");
	pSocket->Close();

	Input input;
	input.type = itChangeStatus;

	string::size_type TokenPos = sLine.find("|");
	if(TokenPos != string::npos)
	{
		input.progress = sLine.substr(0, TokenPos);
	    input.status = sLine.substr(TokenPos + 1);
	}

	return HandleInput(input);
}
//--------------------------------------------------------------------------------------------------------
void SocketStatusInputProvider::ReceivedMessage(DCE::Socket * /*pSocket*/, DCE::Message* /*pMessage*/)
{
	//do nothing ?
}
//--------------------------------------------------------------------------------------------------------
