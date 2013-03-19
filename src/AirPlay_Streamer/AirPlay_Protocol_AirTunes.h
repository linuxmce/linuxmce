/** 
 *
 * AirPlay Protocol - AirTunes
 *
 */

#ifndef AIRPLAYPROTOCOLAIRTUNES_H
#define AIRPLAYPROTOCOLAIRTUNES_H

#include "AirPlay_Protocol.h"

namespace DCE
{

  class AirPlay_Protocol_AirTunes : public AirPlay_Protocol
    {
    private:
    protected:
    public:
      AirPlay_Protocol_AirTunes(AirPlay_Streamer *pAirPlay_Streamer);
      ~AirPlay_Protocol_AirTunes();

      virtual bool init();
      virtual bool start();
      virtual bool stop();
      
    };

}

#endif
