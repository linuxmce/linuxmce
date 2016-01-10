#include "DCE/Logger.h"
using namespace DCE;

#include "VDRCommon.h"

int main()
{
	string sResponse;

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
		exit(1);
	}
#endif

	if(SendVDRCommand("localhost", "LSTE", sResponse))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received response: \n\n%s", sResponse.c_str());
	}

#ifdef WIN32
    WSACleanup();
#endif

	return 0;
}

