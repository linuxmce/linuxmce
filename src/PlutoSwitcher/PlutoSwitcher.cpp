// PlutoSwitcher.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PlutoSwitcher.h"

#include <string>
using namespace std;

#include "Winsock2.h"
#include "time.h"
#include "Iptypes.h"
#include "Iphlpapi.h"

#include "DCE/Socket.h"
#include "DCE/Message.h"
#include "DCE/ClientSocket.h"
#include "DCE/Logger.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/uuencode.h"
#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"

#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include "PlutoUtils/FileUtils.h"
using namespace DCE;

//-------------------------------------------------------------------------------------------------------
string DetectCoreIpAddress();
void EnablePrivileges();
string GetMACaddress();
//-------------------------------------------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//initialize sockets
	WORD    wVersion;
	WSADATA wsaData;
	wVersion = MAKEWORD( 1, 1 );
	if (WSAStartup(wVersion, &wsaData)!=0)
	{
		//int ec = WSAGetLastError();
		//printf("WSAStartup err %d", ec);
		return -1;
	}

	//find the core
	string sCoreIP = DetectCoreIpAddress();

	if(sCoreIP.empty())
	{
		::MessageBox(NULL, TEXT("Can't find your core. Please check if this computer is in the same network with the core."), 
			TEXT("Pluto Switcher"), MB_OK);

		WSACleanup();
		return -2;
	}

	string sMessage = "Found the CORE on ip " + sCoreIP + ". Do you want to reboot as MD?";

	if(
		IDYES == ::MessageBox(NULL, sMessage.c_str(), TEXT("Pluto Switcher"), MB_YESNO)
	)
	{
		int iPK_Device = 0;
		string sForce = "N"; //net boot
		string sMac_address = GetMACaddress();

		//send message to core
		CMD_Halt_Device_DT cmd_Halt_Device_DT(0, DEVICETEMPLATE_General_Info_Plugin_CONST, BL_SameHouse, 
			iPK_Device, sForce, sMac_address);
        		
		Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, sCoreIP, true, -1, 1);

		enum RouterResponse
		{
			rrOk,
			rrRetry,
			rrFailed
		};

		RouterResponse response;

		while(true)
		{
			pEvent->m_pClientSocket->SendString("READY");

			string sResponse;
			if(!pEvent->m_pClientSocket->ReceiveString(sResponse, 5))
			{
				response = rrFailed;
				break;
			}
			if( sResponse=="YES" )
			{
				response = rrOk;
				break;
			}
			else if( sResponse=="NO" )
			{
				response = rrRetry;
			}
			else
			{
				response = rrFailed;
				break;
			}
		}

		switch(response)
		{
			case rrFailed:
			{
				::MessageBox(NULL, TEXT("Found your core, but I can't communicate with the router. Make sure that the router is running."), 
					TEXT("Pluto Switcher"), MB_OK);	
			}
			break;

			case rrRetry:
			{
				::MessageBox(NULL, TEXT("Found your core, but the router is busy. Please try again in few seconds."), 
					TEXT("Pluto Switcher"), MB_OK);	
			}
			break;

			case rrOk:
			{
				if(!pEvent->SendMessage(cmd_Halt_Device_DT.m_pMessage))
				{
					::MessageBox(NULL, TEXT("Got an error while communicating with the router. Please try again later."), 
						TEXT("Pluto Switcher"), MB_OK);	

					response = rrFailed;
				}
			}
			break;
		}
		
		delete pEvent;

		if(response == rrOk)
		{
			//reboot		
			EnablePrivileges();
			::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
		}
	}

	WSACleanup();

	return 0;
}
//-------------------------------------------------------------------------------------------------------
string DetectCoreIpAddress() 
{
	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
	// for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to

	if(NULL != pAdapterInfo)
	{
		char *pIP = pAdapterInfo->IpAddressList.IpAddress.String;

		if(NULL != pIP)
		{
			string sMyIP(pIP);

			size_t pos = sMyIP.rfind('.');
			if(pos != string::npos)
			{
				string sCoreIP = sMyIP.substr(0, pos + 1) + "1";
				return sCoreIP;
			}
		}
	}
	
	//create datagram socket
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//make it nonblocking
	int nonblocking = 1;
	ioctlsocket(s, FIONBIO, (unsigned long*) &nonblocking);

	//set opt broadcast
	int on = 1;
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof(on));

	//setup sockaddr
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	memset((char *) &si_other, sizeof(si_other), 0);
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(33333);
	si_other.sin_addr.s_addr = inet_addr("255.255.255.255");

	//send query
	char send_buf[] = "Search DCERouter";
	sendto(s, send_buf, sizeof(send_buf), 0, (const sockaddr *)&si_other, slen);

	//wait for response
	char recv_buf[128];
	memset(recv_buf, 0, sizeof(recv_buf));
	time_t start_time = time(0);
	while ((time(0) - start_time) < 2) 
	{
		int a = recvfrom(s, recv_buf, sizeof(recv_buf), 0, (sockaddr *)&si_other, &slen);
		if(a > 0)
			break;
	}

	//close socket
	closesocket(s);

	//get ip address
	char *ret = inet_ntoa(si_other.sin_addr);

	string sIP;
	if(NULL != ret)
		sIP = ret;

	return sIP;
} 
//-------------------------------------------------------------------------------------------------------
void EnablePrivileges()
{
	HANDLE hToken; // handle to process token
	TOKEN_PRIVILEGES tkp; // pointer to token structure
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES |
		TOKEN_QUERY, &hToken); // Get the LUID for shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// Get shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
}
//-------------------------------------------------------------------------------------------------------
// Fetches the MAC address and prints it
string GetMACaddress()
{
	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
	// for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to

	char buf[20];
	sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
		pAdapterInfo->Address[0],
		pAdapterInfo->Address[1],
		pAdapterInfo->Address[2],
		pAdapterInfo->Address[3],
		pAdapterInfo->Address[4],
		pAdapterInfo->Address[5]
	);

	return buf;
}
//-------------------------------------------------------------------------------------------------------