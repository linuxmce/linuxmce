/** 
 *
 * AirPlay Protocol - AirPlay
 *
 */

#ifndef AIRPLAYPROTOCOLAIRPLAY_H
#define AIRPLAYPROTOCOLAIRPLAY_H

#include "AirPlay_Protocol.h"

#define SOCKET_STATE_INITIAL 0;

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

    };

}

#endif
