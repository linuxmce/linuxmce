// Definition of the ClientSocket class

#ifndef ClientSocket_class
#define ClientSocket_class

#include "Socket.h"

namespace DB_Wrapper
{

class ClientSocket : private Socket
{
 public:

  ClientSocket ( std::string unix_socket );
  virtual ~ClientSocket(){};

  bool send( const TLV& ) const;
  bool recv( TLV& ) const;
};

};
#endif
