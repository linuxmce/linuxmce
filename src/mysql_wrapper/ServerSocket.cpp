// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "SocketException.h"

#include <iostream>

ServerSocket::ServerSocket (std::string unix_socket )
{
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create server socket." );
    }

  if ( ! Socket::bind ( unix_socket ) )
    {
      throw SocketException ( "Could not bind to socket." );
    }

  if ( ! Socket::listen() )
    {
      throw SocketException ( "Could not listen to socket." );
    }

}

ServerSocket::~ServerSocket()
{
}

const ServerSocket& ServerSocket::operator << ( const TLV& tlv ) const
{
  if ( ! Socket::send ( tlv ) )
    {
      throw SocketException ( "Could not write to socket." );
    }
  return *this;

}


const ServerSocket& ServerSocket::operator >> ( TLV& tlv ) const
{
  if ( ! Socket::recv ( tlv ) )
    {
      throw SocketException ( "Could not read from socket." );
    }
  return *this;
}

void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
      throw SocketException ( "Could not accept socket." );
    }
}
