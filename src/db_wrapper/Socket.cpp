// Implementation of the Socket class.


#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>

namespace DB_Wrapper
{

Socket::Socket() : m_sock(-1), m_sockAsFile(NULL)
{
	m_socklen = sizeof(m_sockaddr);
}

Socket::~Socket()
{
	if (m_sock!=-1)
		close(m_sock);
}

bool Socket::create()
{
	m_sock = socket ( PF_LOCAL, SOCK_STREAM,  0 );
	if (m_sock != -1)
		m_sockAsFile = fdopen(m_sock, "r+");
	return m_sock != -1;
}



bool Socket::bind ( const std::string unix_socket )
{
	unlink(unix_socket.c_str());
	
	m_sockaddr.sun_family = AF_LOCAL;
	strcpy(m_sockaddr.sun_path, unix_socket.c_str() );
	
	return ::bind(m_sock, (sockaddr*)&m_sockaddr, m_socklen) != -1;
}


bool Socket::listen() const
{
	return ::listen(m_sock, 5) != -1;
}


bool Socket::accept ( Socket& new_socket ) const
{
	new_socket.m_sock = ::accept(m_sock, (sockaddr*) &new_socket.m_sockaddr, &new_socket.m_socklen);
	if (new_socket.m_sock!=-1)
		new_socket.m_sockAsFile = fdopen(new_socket.m_sock, "r+");
	return new_socket.m_sock !=-1;
}

bool Socket::send ( const TLV &tlv ) const
{
	uint length = sizeof(tlv.type) + sizeof(tlv.length) + tlv.length;

	char *buf = new char[length];
	if (!buf)
		return false;
	
	memcpy(buf, (char*)&tlv.type, sizeof(tlv.type));
	memcpy(buf+sizeof(tlv.type), (char*)&tlv.length, sizeof(tlv.length));
	memcpy(buf+sizeof(tlv.type)+sizeof(tlv.length),tlv.value, tlv.length);
	
	write(m_sock,  buf, length);

	flush();

	delete[] buf;

	return true;
}

int Socket::recv ( TLV &tlv ) const
{
	int cnt = read ( m_sock, &tlv.type, sizeof(tlv.type) );
	
	if (cnt == 0)
	{
		tlv.type = TLV::typeNone;
		return false;
	}
	
	cnt = read ( m_sock, &tlv.length, sizeof(tlv.length) );
	if (cnt == 0)
	{
		tlv.type = TLV::typeNone;
		tlv.length = 0;
		return false;
	}

	if (tlv.length)
	{	
		tlv.value = new char[tlv.length];
		
		cnt = read ( m_sock, tlv.value, tlv.length );
		if (cnt == 0)
		{
			tlv.type = TLV::typeNone;
			tlv.length = 0;
			delete[] tlv.value;
			return false;
		}
	}
	
	return true;
}

bool Socket::connect (const std::string unix_socket )
{
	m_sock = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (m_sock != -1)
		m_sockAsFile = fdopen(m_sock, "r+");
	m_sockaddr.sun_family = AF_LOCAL;
	strcpy(m_sockaddr.sun_path, unix_socket.c_str() );
	int result = ::connect(m_sock, (sockaddr*)&m_sockaddr, m_socklen);
	if (result == -1)
		return false;
	else
	{
		int on = 1;
		setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE,  &on, sizeof(on));
		return true;
	}
}

void Socket::flush() const
{
	if (m_sockAsFile)
	{
		int iRes = fflush(m_sockAsFile);
		if (iRes)
		{
//			cerr << "failed to flush socket!" << endl;
		}
	}
	else
	{
//		cerr << "m_sockAsFile == NULL" << endl;
	}
}

}

