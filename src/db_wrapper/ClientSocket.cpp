// Implementation of the ClientSocket class

#include "ClientSocket.h"
#include "SocketException.h"

namespace DB_Wrapper
{

ClientSocket::ClientSocket ( std::string unix_socket )
{
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create client socket." );
    }

  if ( ! Socket::connect ( unix_socket ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

}


bool ClientSocket::send( const TLV& tlv) const
{
  if ( ! Socket::send ( tlv ) )
    {
      return false;
    }

  return true;
}


bool ClientSocket::recv( TLV& tlv) const
{
  if ( ! Socket::recv ( tlv ) )
    {
      return false;
    }

  return true;
}

}

