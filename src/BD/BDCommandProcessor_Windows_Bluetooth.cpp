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
#ifndef SYMBIAN
	#include "PlutoUtils/CommonIncludes.h"
#endif

#include "PlutoUtils/MultiThreadIncludes.h"
#include "Logger.h"
#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD/BDCommandProcessor_Windows_Bluetooth.h"

#include "Socket.h"

using namespace DCE;

#define RECV_TIMEDOUT 15 //seconds

BDCommandProcessor_Windows_Bluetooth::BDCommandProcessor_Windows_Bluetooth(string sMacAddressPhone,string sMacAddressDongle)
	: BDCommandProcessor(sMacAddressPhone), m_ReceiveBufferMutex("receive buffer")
{
	m_IsConnected = false;
	m_pReceiveBuffer=NULL;
	m_iSizeReceiveBuffer=0;
	m_ReceiveBufferMutex.Init(NULL);

	m_ptr_rfcm = new CRfCommTransport( this );

	int RetryCount=0;
    while( !m_IsConnected && RetryCount++<5)
	{
		m_IsConnected = m_ptr_rfcm->TransportOpen( sMacAddressPhone.c_str() );
		if( !m_IsConnected )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WIDCOM failed to open RFCOM Port.  Try #%d",RetryCount);
			Sleep(1000);
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	if( !m_IsConnected )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to open bluetooth device");
		m_bExit=m_bDead=true;
	}
	else
		m_bDead = false;
}

BDCommandProcessor_Windows_Bluetooth::~BDCommandProcessor_Windows_Bluetooth()
{
	if ( m_IsConnected == true )
	{
		m_IsConnected = false;
		m_ptr_rfcm->TransportClose();
		delete m_ptr_rfcm;
		m_ptr_rfcm = NULL;
	}

	if ( m_ptr_rfcm != NULL )
	{
		delete m_ptr_rfcm;
		m_ptr_rfcm = NULL;
	}
}




void BDCommandProcessor_Windows_Bluetooth::OnTranspDataReceived( BYTE* data, ULONG len )
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Received data %d bytes long.  buffer was %d",(int) len,(int) m_iSizeReceiveBuffer);
	PLUTO_SAFETY_LOCK(rb,m_ReceiveBufferMutex);
	if( m_pReceiveBuffer)
		m_pReceiveBuffer=(char *)realloc(m_pReceiveBuffer,m_iSizeReceiveBuffer+len);
	else
	{
		m_pReceiveBuffer=(char *)malloc(len);
		::ZeroMemory(m_pReceiveBuffer, len);
	}

	memcpy( m_pReceiveBuffer+m_iSizeReceiveBuffer, data, len );
	m_iSizeReceiveBuffer+=len;
	pthread_cond_broadcast(&m_PollingCond);
}

bool BDCommandProcessor_Windows_Bluetooth::SendData(int size,const char *data)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending %d bytes of data", size);

	m_ptr_rfcm->TransportWrite( (BYTE *)data, size );

	if(NULL == data)
	{
		m_bDead = true;
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sent %d bytes of data", size);

	return true;
}

// The framework will call this function, which should just block the thread until that many characters
// are received.  
char *BDCommandProcessor_Windows_Bluetooth::ReceiveData(int size)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Request for data %d bytes long buffer has %d",(int) size,(int) m_iSizeReceiveBuffer);
	// Wait until we get that many characters in our buffer
	while( (int) m_iSizeReceiveBuffer<size )
	{
		PLUTO_SAFETY_LOCK(pm,m_PollingMutex);

		timespec abstime;
		abstime.tv_sec = static_cast<long>(time(NULL)) + RECV_TIMEDOUT;
		abstime.tv_nsec = 0;
		if(ETIMEDOUT == pm.TimedCondWait(abstime))
		{
			m_bDead = true;
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Receive data timed_out! The connection is down.");
			return NULL;
		}
	}

	PLUTO_SAFETY_LOCK(rb,m_ReceiveBufferMutex);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"returning %d of data",(int) size);
	// We're holding the mutex and have the data.  The caller will have to delete the buffer
	char *Buffer = (char *)malloc(size);
	memcpy(Buffer,m_pReceiveBuffer,size);
	if( (int) m_iSizeReceiveBuffer>size )
	{
		// There's still more data in the buffer
		char *NewBuffer = (char *)malloc(m_iSizeReceiveBuffer-size);
		memcpy(NewBuffer,m_pReceiveBuffer+size,m_iSizeReceiveBuffer-size);
		delete m_pReceiveBuffer;
		m_pReceiveBuffer = NewBuffer;
		m_iSizeReceiveBuffer-=size;
	}
	else
	{
		delete m_pReceiveBuffer;
		m_pReceiveBuffer = NULL; 
		m_iSizeReceiveBuffer=0;
	}


	return Buffer;
}

