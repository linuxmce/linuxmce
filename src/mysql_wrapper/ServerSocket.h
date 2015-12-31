// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"
#include "TLV.h"

class ServerSocket : private Socket
{
 public:

  ServerSocket ( std::string unix_socket );
  ServerSocket (){};
  virtual ~ServerSocket();

/*
  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;
*/

  const ServerSocket& operator << ( const TLV& ) const;
  const ServerSocket& operator >> ( TLV& ) const;

  void accept ( ServerSocket& );

};


#endif
