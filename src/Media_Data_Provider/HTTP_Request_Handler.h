/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#ifndef HTTP_Request_Handler_h
#define HTTP_Request_Handler_h

#include <map>

using namespace std;

#include "DCE/ServerSocket.h"
#include "DCE/SocketListener.h"

namespace DCE
{
	class Generic_Mysql_Provider;

	class HTTP_Request_Socket : public ServerSocket
	{
		Generic_Mysql_Provider *m_pGeneric_Mysql_Provider;

	public:
		string m_sGet;

		HTTP_Request_Socket( Generic_Mysql_Provider *pGeneric_Mysql_Provider, SocketListener *pListener, SOCKET Sock, string sName, string sIPAddress="", string sMacAddress="" )
			: ServerSocket(pListener, Sock, sName, sIPAddress, sMacAddress) 
		{
			m_pGeneric_Mysql_Provider=pGeneric_Mysql_Provider;
		}

		void HandleRequest(string sRequest);
		string ProcessParameters(string sRequest, string *sParameter);
		bool PreprocessRequest(string sID,string sParameters,string *sData_String);
		void PreprocessMediaDbRequest(string &sDB,string &sRequest,string &sParms,string &sQuery);

		bool ServeClient();
	};

	class HTTP_Request_Handler : public SocketListener
	{
		Generic_Mysql_Provider *m_pGeneric_Mysql_Provider;

	public:
		HTTP_Request_Handler(Generic_Mysql_Provider *pGeneric_Mysql_Provider) : SocketListener("HTTP_Request_Handler") { m_pGeneric_Mysql_Provider=pGeneric_Mysql_Provider; }
		virtual bool ReceivedString( Socket *pSocket, string sLine, int nTimeout = - 1 )
		{
			printf("Received: %s", sLine.c_str());
			int k=2;
			return true;
		}

		virtual Socket *CreateSocket( SOCKET newsock, string sName, string sIPAddress="", string sMacAddress="" )
		{
			PLUTO_SAFETY_LOCK( lm, m_ListenerMutex );
			ServerSocket *pSocket = new HTTP_Request_Socket( m_pGeneric_Mysql_Provider, this, newsock, sName, sIPAddress, sMacAddress );
			pSocket->m_bSendOnlySocket = m_bSendOnlySocket;
			m_vectorServerSocket.push_back(pSocket);
			pSocket->Run();
			return pSocket;
		}

		virtual void ReceivedMessage( Socket *pSocket, Message* pMessage ) { }
	};
}

/*
#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
#endif
	
	LoggerWrapper::SetType(LT_LOGGER_FILE,"/temp.log");

	HillcrestListener _HillcrestListener;
    _HillcrestListener.StartListening(9876);
	Sleep(2000);
	if( !_HillcrestListener.m_bRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not start listening");
		return 1;
	}
    while( _HillcrestListener.m_bRunning )
    {
        // TODO: Check m_mapServerSocket and see if we've lost any
        // command connections.
        Sleep(1000);
    }
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
}
*/

#endif
