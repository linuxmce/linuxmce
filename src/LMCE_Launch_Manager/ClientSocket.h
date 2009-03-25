// Definition of the ClientSocket class
// Note that is not used by LM directly, but is put here for prosperity.
// The ClientSocket will need included into any LM clients made!



#ifndef ClientSocket_class
#define ClientSocket_class

#include "Socket.h"


class ClientSocket : private Socket
{
 public:

  ClientSocket ( std::string host, int port );
  virtual ~ClientSocket(){};

  const ClientSocket& operator << ( const std::string& ) const;
  const ClientSocket& operator >> ( std::string& ) const;

};


#endif

