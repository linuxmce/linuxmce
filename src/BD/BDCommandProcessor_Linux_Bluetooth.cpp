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
//#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "Logger.h"
#include "PhoneDevice.h"
#include "BDCommandProcessor.h"
#include "BDCommandProcessor_Linux_Bluetooth.h"

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <sys/socket.h>

#include "sdpbrowse.h"

using namespace DCE;

//void DummySignalHandler(int) {
//LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Need to cancel bd comm");
//}

BDCommandProcessor_Linux_Bluetooth::BDCommandProcessor_Linux_Bluetooth(string sMacAddressPhone,string sMacAddressDongle,class PhoneDevice *pDevice)
: BDCommandProcessor(sMacAddressPhone)
	
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"start of const %s",sMacAddressPhone.c_str());

	m_bQuit = false;
	m_bRunning = false;

	m_pDevice=pDevice;
	if( !m_pDevice )
		cerr << "temporary hack -- m_pDevice is NULL. " << sMacAddressPhone << endl;
	// Assert: ON_OFF is 1 and the map has no connection.  Try to connect
	
	struct sockaddr_rc laddr, raddr;
	struct rfcomm_dev_req req;
	socklen_t alen;

	laddr.rc_family = AF_BLUETOOTH;
	bacpy(&laddr.rc_bdaddr, &m_pDevice->m_bdaddrDongle);
	laddr.rc_channel = 0;

	//if( NULL != m_pDevice->m_bdaddrDongle ) 
	//printf("m_pDevice->m_bdaddrDongle is null !!\n");

	printf("@mac address: %s\n", pDevice->m_sMacAddress.c_str());
	
	raddr.rc_family = AF_BLUETOOTH;
	str2ba(pDevice->m_sMacAddress.c_str(), &raddr.rc_bdaddr);
	raddr.rc_channel = sdp_lookup_pluto_rfcomm_channel(raddr.rc_bdaddr);
	// raddr.rc_channel = 3;
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "received channel: %i\n",raddr.rc_channel);
	
	if ((m_CommHandle = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Can't create RFCOMM socket");
		m_bDead=true;
		return;
	}

	if (bind(m_CommHandle, (struct sockaddr *)&laddr, sizeof(laddr)) < 0) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Can't bind RFCOMM socket %s",sMacAddressPhone.c_str());
		close(m_CommHandle);
		m_bDead=true;
		return;
	}
	
	int err_code = 0;
	printf("# try to connect\n");
	if((err_code = connect(m_CommHandle, (struct sockaddr *)&raddr, sizeof(raddr))) < 0) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Can't connect RFCOMM socket %s, channel %d",sMacAddressPhone.c_str(), raddr.rc_channel);
		printf("# error code: %d\n", err_code);

        	close(m_CommHandle);
        	m_bDead=true;
        	return;
	}

	alen = sizeof(laddr);
	if (getsockname(m_CommHandle, (struct sockaddr *)&laddr, &alen) < 0) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Can't get RFCOMM socket name %s",sMacAddressPhone.c_str());
		close(m_CommHandle);
		m_bDead=true;
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Successfully connected to the symbian application %s",sMacAddressPhone.c_str());
}

BDCommandProcessor_Linux_Bluetooth::~BDCommandProcessor_Linux_Bluetooth()
{
	if (m_CommHandle)
	{
		m_bQuit = true;
		//signal(SIGUSR1, DummySignalHandler);
		//raise(SIGUSR1);
		
		time_t start = time(NULL);
        	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Waiting any operation with the socket to finish...");
		while(m_bRunning)
		{
			if(time(NULL) - start > 5)
				break;
			
			Sleep(50);
		}
		
		close(m_CommHandle);
		m_CommHandle = 0;
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "BDCommandProcessor_Linux_Bluetooth is destroyed.");
}

bool BDCommandProcessor_Linux_Bluetooth::SendData(int size, const char *data)
{
//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to send %d bytes of data", size);
	
	m_bRunning = true;
	
	int br;
	int bytes_to_send = size;
	int bytes_sent = 0;
	
	fd_set wrfds;
	struct timeval tv_total, tv;
        struct timeval tv_select_1; 
        struct timeval tv_select_2; 
        struct timeval tv_select;	

	while(bytes_to_send > 0 && !m_bQuit)
	{
		int iRet;
		tv_total.tv_sec = 5;
		tv_total.tv_usec = 0;
		do
		{
			if(m_bQuit)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"SendData (BD connection): have to quit!");
				m_bRunning = false;
				return false;
			}

			FD_ZERO(&wrfds);
			FD_SET(m_CommHandle, &wrfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100000;

            		//before select
            		gettimeofday(&tv_select_1, NULL);

			iRet = select((int) (m_CommHandle+1), NULL, &wrfds, NULL, &tv);

            		//after select
            		gettimeofday(&tv_select_2, NULL);
            		//the select took 'tv_select' time
            		tv_select = tv_select_2 - tv_select_1;

			if (errno == EINTR)
			{
				errno = 0;
				iRet = 0;
			}

			tv_total -= tv_select;
		} 
		while(iRet != -1 && iRet != 1 && tv_total.tv_sec > 0);
	
		br = write(m_CommHandle, data + bytes_sent, bytes_to_send);

		if(br == -1) 
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to send %d bytes of data", size);
			m_bRunning = false;
			return false;
		}
		else 
		{
			bytes_sent += br;
			bytes_to_send -= br;

//			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sent %d bytes [total %d/%d]", br, bytes_sent, size);
		}

		if(bytes_to_send > 0)
		{
//			printf("@need to send more data... \n");
		}
	}

	m_bRunning = false;

	if(m_bQuit)
		return false;

	return true;
}

char *BDCommandProcessor_Linux_Bluetooth::ReceiveData(int size)
{
//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to receive %d bytes of data", size);
//
	m_bRunning = true;
		
	char *buffer = new char[size];
	int br = 0;
	int bytes_to_receive = size;
	int bytes_received = 0;

	fd_set wrfds;
	struct timeval tv_total, tv;
        struct timeval tv_select_1; 
        struct timeval tv_select_2; 
        struct timeval tv_select;	
	
	while(bytes_to_receive > 0 && !m_bQuit)
	{
		int iRet;
		tv_total.tv_sec = 5;
		tv_total.tv_usec = 0;
		do
		{
			if(m_bQuit)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"ReceiveData (BD connection): have to quit!");
				m_bRunning = false;
				return false;
			}

			FD_ZERO(&wrfds);
			FD_SET(m_CommHandle, &wrfds);

			tv.tv_sec = 0;
			tv.tv_usec = 100000;

            		//before select
            		gettimeofday(&tv_select_1, NULL);

			iRet = select((int) (m_CommHandle+1), NULL, &wrfds, NULL, &tv);

            		//after select
            		gettimeofday(&tv_select_2, NULL);
            		//the select took 'tv_select' time
            		tv_select = tv_select_2 - tv_select_1;

			if (errno == EINTR)
			{
				errno = 0;
				iRet = 0;
			}

			tv_total -= tv_select;
		} 
		while(iRet != -1 && iRet != 1 && tv_total.tv_sec > 0);	
	
		br = read(m_CommHandle, buffer + bytes_received, bytes_to_receive);

		if (br==-1) 
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to receive %d bytes of data", size);
			return NULL;
		}
		else 
		{
			bytes_received += br;
			bytes_to_receive -= br;
			
//			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received %d bytes [total %d/%d]", br, bytes_received, size);
		}

		if(bytes_to_receive > 0)
		{
//			printf("@need to receive more data.. \n");
		}
	}

	m_bRunning = false;

	if(m_bQuit)
		return NULL;
	
	return buffer;
}

