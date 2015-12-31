#include "DCE/Logger.h"
#include "DCE/PlainClientSocket.h"

using namespace DCE;

int main( int argc, char* argv[] )
{
	if(argc < 5)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Usage: StatusOnLCD <host> <port> <progress> <status> \n");
		return 4;
	}

	string sHost = argv[1];
	string sPort = argv[2];
	string sProgress = argv[3];
	string sStatus = argv[4];

#ifdef WIN32
	WORD    wVersion;
    WSADATA wsaData;
    wVersion = MAKEWORD( 1, 1 );
	if (WSAStartup(wVersion, &wsaData)!=0)
	{
		int ec = WSAGetLastError();
		char s[91];
		sprintf(s, "WSAStartup err %d", ec);
		cerr << s << endl;
		return 3;
	}
#endif

	PlainClientSocket *pClientSocket = new PlainClientSocket(sHost + ":" + sPort);
	if(!pClientSocket->Connect())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to connect to LCD app - host %s, port %s",
			sHost.c_str(), sPort.c_str());
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connected");
		pClientSocket->SendReceiveString(sProgress + "%|" + sStatus);
	}

	delete pClientSocket;

#ifdef WIN32
    WSACleanup();
#endif

	return 0;
}
