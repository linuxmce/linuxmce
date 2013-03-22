/** 
 *
 * AirPlay Protocol - AirPlay
 *
 */

#ifndef AIRPLAYPROTOCOLAIRPLAY_H
#define AIRPLAYPROTOCOLAIRPLAY_H

#include "AirPlay_Protocol.h"

#include "DCE/SocketListener.h"
#include "DCE/ServerSocket.h"

namespace DCE
{

  class AirPlay_Protocol_AirPlay : public AirPlay_Protocol
    {
    private:
      pthread_t m_serverThread;
    protected:
    public:
      AirPlay_Protocol_AirPlay(AirPlay_Streamer *pAirPlay_Streamer);
      ~AirPlay_Protocol_AirPlay();

      virtual bool init();
      virtual bool start();
      virtual bool stop();

      void ServerThread();
      
      class AirPlaySocketListener : public SocketListener
      {
      public:
      AirPlaySocketListener(string sName):SocketListener(sName){};
	virtual void ReceivedMessage(Socket *pSocket, Message* pMessage){};
	virtual bool ReceivedString(Socket *pSocket, string sLine, int nTimeout = - 1);
      };
      AirPlaySocketListener *m_pAirPlaySocketListener;
    };

}

#endif
