/**
 * 
 * AirPlay Protocol - AirPlay
 *
 */

#include "AirPlay_Protocol_AirPlay.h"

namespace DCE
{

  AirPlay_Protocol_AirPlay::AirPlay_Protocol_AirPlay(AirPlay_Streamer *pAirPlay_Streamer)
    : AirPlay_Protocol(pAirPlay_Streamer)
  {
    Port_set(7000);
    ServiceType_set("_airplay._tcp");
    TXTRecord_Add("features=0x3b");
    TXTRecord_Add("model=AppleTV2,1");
    TXTRecord_Add("srcvers=130.14");
  }

  AirPlay_Protocol_AirPlay::~AirPlay_Protocol_AirPlay()
  {
    // All relevant dtor called in base class.
  }

  bool AirPlay_Protocol_AirPlay::init()
  {
    return true;
  }

  bool AirPlay_Protocol_AirPlay::start()
  {
    return true;
  }

  bool AirPlay_Protocol_AirPlay::stop()
  {
    return true;
  }

}
