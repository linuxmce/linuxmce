/** 
 *
 * AirPlay Protocol - AirPlay
 *
 */

#ifndef AIRPLAYPROTOCOLAIRPLAY_H
#define AIRPLAYPROTOCOLAIRPLAY_H

#include "AirPlay_Protocol.h"

namespace DCE
{

  class AirPlay_Protocol_AirPlay : public AirPlay_Protocol
    {
    private:
    protected:
    public:
      AirPlay_Protocol_AirPlay(AirPlay_Streamer *pAirPlay_Streamer);
      ~AirPlay_Protocol_AirPlay();

      virtual bool init();
      virtual bool start();
      virtual bool stop();
      
    };

}

#endif
