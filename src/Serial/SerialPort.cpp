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
#include "SerialPort.h"
#include <cstdlib>
#include <cstring>

#ifdef WIN32

#include "PlutoUtils/MultiThreadIncludes.h"

#ifdef DEBUG_USING_FILES
FILE *filer,*filew;
#endif

CSerialPort::CSerialPort(string Port, unsigned BPS, eParityBitStop ParityBitStop, bool FlowControl)
{
#ifdef DEBUG_USING_FILES
	filer=fopen("/serialread.txt","wb");
	filew=fopen("/serialwrite.txt","wb");
#endif

    m_Serio.port=NULL;
	if (serio_open(&m_Serio, BPS, Port.c_str(), NOPARITY, 8, ONESTOPBIT)!=comm_STATUS_OK)
	{
		throw string("Couldn't open serial port "+Port+".");
	}    
}

CSerialPort::~CSerialPort()
{
#ifdef DEBUG_USING_FILES
	fclose(filer);
	fclose(filew);
#endif

	serio_close(&m_Serio);
}

void CSerialPort::Write(char *Buf, size_t Len)
{
#ifdef DEBUG_USING_FILES
	fwrite(Buf,Len,1,filew);
#endif

	serio_write(&m_Serio, Buf, Len);
}

size_t CSerialPort::Read(char *Buf, size_t MaxLen, int Timeout)
{
	timespec tv_now, tv_timeout;
	gettimeofday(&tv_now, NULL);
	tv_timeout = tv_now;
	tv_timeout += Timeout;

	size_t ReadSoFar=0;
	size_t ReturnValue = 0;
	while( tv_now<tv_timeout )
	{
		serio_read(&m_Serio, &Buf[ReadSoFar], MaxLen, &ReturnValue);


#ifdef DEBUG_USING_FILES
		for(int i=0;i<ReturnValue;++i )
		{
			unsigned char c=Buf[ReadSoFar+i];
			fprintf(filer,"-%02x- ",(int) c );
		}
		if( ReturnValue==0 )
			fprintf(filer,".");
#endif

		ReadSoFar+=ReturnValue;
		if( ReadSoFar>=MaxLen )
			return ReadSoFar;
		Sleep(10); // We really need something that blocks -- nasty HACK
		gettimeofday(&tv_now, NULL);
	}
	return ReadSoFar;
}

bool CSerialPort::IsReadEmpty()
{
    Sleep(0);    
    return FALSE;
}

void CSerialPort::Flush() 
{
}

bool CSerialPort::SendBreak(int time)
{
	return true;
}

bool CSerialPort::IsBusy()
{
	return false;
}

bool CSerialPort::IsRngSet()
{
	return true;
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <pthread.h>

//#include <iostream>
#include <deque>

#include "PlutoUtils/LinuxSerialUSB.h"

class CSerialPort::Private
{
	public:
	
		Private(CSerialPort * parent);
		
		~Private();
	
		static void* receiveFunction(void *);
		
		bool receiveThread(bool);
		
		pthread_mutex_t mutex_serial;
		pthread_mutex_t mutex_buffer;
		pthread_t write_thread;
		
		bool running;
		
		std::deque<char> buffer;
	
	private:
	
		CSerialPort * parent_;
};

CSerialPort::Private::Private(CSerialPort * parent)
	: running(false),
	  parent_(parent)
{
}

CSerialPort::Private::~Private()
{
}

bool CSerialPort::Private::receiveThread(bool start)
{
	if( running != start )
	{
		if( running )
		{
			// stop
			running = false;
			pthread_join(write_thread, NULL);
			pthread_mutex_destroy(&mutex_serial);
			pthread_mutex_destroy(&mutex_buffer);
			
			buffer.clear();
		}
		else
		{
			// start
			running = true;
			pthread_mutex_init(&mutex_serial, NULL);
			pthread_mutex_init(&mutex_buffer, NULL);
			int stat = pthread_create(&write_thread, NULL, receiveFunction, (void*)this);
			if( stat != 0)
			{
				running = false;
				return false;
			}
		}
	}
	
	return true;
}

void *CSerialPort::Private::receiveFunction(void * serialPrivate)
{
	CSerialPort::Private * priv = (CSerialPort::Private *) serialPrivate;
	if( priv == NULL )
	{
		return NULL;
	}
	
	CSerialPort * parent = priv->parent_;
	if( parent == NULL )
	{
		return NULL;
	}
	
	char mybuf[1024];
	size_t len = sizeof(mybuf);
	
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000;
	
	ssize_t retval = 0;
	int ret = 0;
	
	while( priv->running )
	{
		pthread_mutex_lock( &priv->mutex_serial );
		fd_set rfds;
		
		FD_ZERO(&rfds);
		FD_SET(parent->m_fdSerial, &rfds);
		
		ret = select(parent->m_fdSerial+1, &rfds, NULL, NULL, &tv);
		if (ret <= 0 || !FD_ISSET(parent->m_fdSerial, &rfds))
		{
			retval = -1;
		}
		else
		{
			retval = read(parent->m_fdSerial, mybuf, len);
		}
		pthread_mutex_unlock( &priv->mutex_serial );
		
		if(retval > 0 && (size_t)retval <= sizeof(mybuf))
		{
			pthread_mutex_lock( &priv->mutex_buffer );
			for(size_t i=0; i<(size_t)retval; i++)
			{
				priv->buffer.push_back(mybuf[i]);
			}
			pthread_mutex_unlock( &priv->mutex_buffer );
		}
		
		usleep(100);
		
	}
	
	pthread_exit(NULL);
	
	return NULL;
}

CSerialPort::CSerialPort(string Port, unsigned BPS, eParityBitStop ParityBitStop, bool FlowControl)
{
	d.reset(new Private(this));
	if( d.get() == NULL )
	{
		throw string("Not enough memory!");
	}
	
	struct termios t;
	
	Port = TranslateSerialUSB(Port);
	if ((m_fdSerial = open(string(Port).c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
	{
		throw string("Cannot open ")+Port+string("\r\n");
    }
    memset(&t, 0, sizeof(struct termios));
    t.c_iflag = IGNBRK | INPCK;
    t.c_oflag = 0;
    // TODO: Implement paritybitstop
	switch(ParityBitStop) {
	case epbsN81:
	    t.c_cflag = CS8 | CREAD | CLOCAL;
		break;
	case epbsE81:
	    t.c_cflag = CS8 | CREAD | CLOCAL | PARENB;
		break;
	case epbsO81:
	    t.c_cflag = CS8 | CREAD | CLOCAL | PARENB | PARODD;
		break;
	default:
	    t.c_cflag = CS8 | CREAD | CLOCAL/* | CSTOPB*/;
		break;
	}
	if (FlowControl)
		t.c_cflag |= CRTSCTS;
    t.c_lflag = 0;
    switch(BPS)
    {
    case 300:
        cfsetispeed(&t,B300);
        cfsetospeed(&t,B300);
        break;
    case 1200:
        cfsetispeed(&t,B1200);
        cfsetospeed(&t,B1200);
        break;
    case 2400:
        cfsetispeed(&t,B2400);
        cfsetospeed(&t,B2400);
        break;
    case 4800:
        cfsetispeed(&t,B4800);
        cfsetospeed(&t,B4800);
        break;
    case 9600:
        cfsetispeed(&t,B9600);
        cfsetospeed(&t,B9600);
        break;
    case 19200:
        cfsetispeed(&t,B19200);
        cfsetospeed(&t,B19200);
        break;
    case 38400:
        cfsetispeed(&t,B38400);
        cfsetospeed(&t,B38400);
	break;
    case 57600:
        cfsetispeed(&t,B57600);
        cfsetospeed(&t,B57600);
	break;
    case 115200:
        cfsetispeed(&t,B115200);
        cfsetospeed(&t,B115200);
    break;
		default:
        throw string("Baud rate not supported");
    }
    ::tcsetattr(m_fdSerial,TCSANOW,&t);
	
	if( !d->receiveThread(true) )
	{
		throw string("The receiving thread doesn't start.");
	}
}

CSerialPort::~CSerialPort()
{
	d->receiveThread(false);
	
	::tcdrain(m_fdSerial);
	close(m_fdSerial);
}

void CSerialPort::Write(char *Buf, size_t Len)
{
   time_t Timeout = 3000; /** 3 sec */
   time_t timeoutSec = Timeout/1000;
   size_t BytesWritten = 0;
   ssize_t iWrite = 0;
   time_t end = 0;
   time_t start = time(NULL);

	pthread_mutex_lock( &d->mutex_serial );
	do
	{
		fd_set wrfds;
		FD_ZERO(&wrfds);
		FD_SET(m_fdSerial, &wrfds);
		
		int ret = 0;
		struct timeval tv;
		tv.tv_sec = timeoutSec;
		tv.tv_usec = 0;
		
		do
		{
			ret = select(m_fdSerial+1, NULL, &wrfds, NULL, &tv);
			end = time(NULL);
		} while( !ret && (end-start) < timeoutSec );
		
		if (ret <= 0 || !FD_ISSET(m_fdSerial, &wrfds) )
		{
			continue;
		}
		
		iWrite = write(m_fdSerial, Buf, Len);
		if( iWrite >= 0 )
		{
			BytesWritten += iWrite;
		}
		
		end = time(NULL);
	} while(BytesWritten < Len && (end-start) < timeoutSec);
	pthread_mutex_unlock( &d->mutex_serial );
}

size_t CSerialPort::Read(char *Buf, size_t MaxLen, int Timeout)
{
	struct timeval now;
	struct timeval last;
	size_t bytes = 0;
	
	gettimeofday(&now, NULL);
	
	last.tv_sec = now.tv_sec+Timeout/1000;
	last.tv_usec = now.tv_usec+(Timeout % 1000) * 1000;
	
	int iTimeout = 0;
	while( iTimeout >= 0 )
	{
		usleep(1000);
		
		pthread_mutex_lock( &d->mutex_buffer );
		bytes = d->buffer.size();
		pthread_mutex_unlock( &d->mutex_buffer );
		if( MaxLen <= bytes )
		{
			bytes = MaxLen;
			break;
		}
		
		gettimeofday(&now, NULL);
		iTimeout = (last.tv_sec - now.tv_sec) * 1000000 + (last.tv_usec - now.tv_usec);
	}
	
	pthread_mutex_lock( &d->mutex_buffer );
	for(size_t i=0; i<bytes; i++)
	{
		Buf[i] = d->buffer.front();
		d->buffer.pop_front();
	}
	pthread_mutex_unlock( &d->mutex_buffer );
	
	return bytes;
}

bool CSerialPort::IsReadEmpty()
{
	if( d->buffer.empty() )						//Is there anything in the buffer?
	{								//No, there isn't!
		struct timeval tv;					//	time
		fd_set rfds;						//	rfds = readfds
			
		FD_ZERO(&rfds);						//m_fdSerial is a file reference to the serial port opened with open() in the constructor.
		FD_SET(m_fdSerial, &rfds);
		int ret = 0;
		
		tv.tv_sec = 0 ;
		tv.tv_usec = 0;
									//	select(nfds(ignored),readfds,writefds,execptfds,timeval timeout)
		ret = select(m_fdSerial+1, &rfds, NULL, NULL, &tv);	//	select returns the total number of socket handles that are ready and contained in the fd_set functions, or 0 on timeout, or an error
		if ( ret <= 0 ) /*time limit expired(0) or error occurred(-1)*/
		{
			return true;
		}
		else /*got a positive value back*/
		{
			return false;
		}
	}
									//Yes, there is data already in in the buffer!
	return false;
}


void CSerialPort::Flush() {
    ::tcflush(m_fdSerial, TCIOFLUSH);
}

bool CSerialPort::SendBreak(int time)
{
	pthread_mutex_lock( &d->mutex_serial );
	int iSendBreak = ::tcsendbreak(m_fdSerial, time);
	pthread_mutex_unlock( &d->mutex_serial );

	return 0 == iSendBreak;
}

bool CSerialPort::IsBusy()
{
	int flags = 0;
	
	pthread_mutex_lock( &d->mutex_serial );
	int iCTS = ioctl( m_fdSerial, TIOCMGET, &flags ); 
	pthread_mutex_unlock( &d->mutex_serial );
	
	if( -1 == iCTS )
	{
		// bad serial device
		return false;
	}
	
/*		cout << "IsBusy --- 2 --- CAR = " << (int)(flags & TIOCM_CAR) 
			<< " RTS = " << (int)(flags & TIOCM_RTS) 
			<< " CTS = " << (int)(flags & TIOCM_CTS) << "\n";*/
	
	// if CTS is not set, then there isn't something connected to the serial port
	// or the hardware dataflow control lines are not used
	if( !(flags & TIOCM_CTS) )
	{
		return false;
	}
	
	return true;
}

bool CSerialPort::IsRngSet()
{
	int flags = 0;

	pthread_mutex_lock( &d->mutex_serial );
	int iRNG = ioctl( m_fdSerial, TIOCMGET, &flags ); 
	pthread_mutex_unlock( &d->mutex_serial );

	if( -1 == iRNG )
	{
		// bad serial device
		return false;
	}

	if( !(flags & TIOCM_RNG) )
	{
		return false;
	}

	return true;
}

#endif


