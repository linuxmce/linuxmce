/**
 * 
 * AirPlay Protocol - AirPlay
 *
 */

#include "AirPlay_Protocol_AirTunes.h"

namespace DCE
{

  AirPlay_Protocol_AirTunes::AirPlay_Protocol_AirTunes(AirPlay_Streamer *pAirPlay_Streamer)
    : AirPlay_Protocol(pAirPlay_Streamer)
  {
    Port_set(49152);
    ServiceType_set("_raop._tcp");
    TXTRecord_Add("am=LinuxMCE,1");
    TXTRecord_Add("tp=UDP");
    TXTRecord_Add("sm=false");
    TXTRecord_Add("da=true");
    TXTRecord_Add("vn=65537");
    TXTRecord_Add("sv=false");
    TXTRecord_Add("ek=1");
    TXTRecord_Add("et=0,4");
    TXTRecord_Add("cn=0,1");
    TXTRecord_Add("ch=2");
    TXTRecord_Add("ss=16");
    TXTRecord_Add("sr=44100");
    TXTRecord_Add("pw=false");
    TXTRecord_Add("vn=3");
    TXTRecord_Add("txtvers=1");
  }

  AirPlay_Protocol_AirTunes::~AirPlay_Protocol_AirTunes()
  {
    // All relevant dtor called in base class.
  }

  bool AirPlay_Protocol_AirTunes::init()
  {
    return true;
  }

  bool AirPlay_Protocol_AirTunes::start()
  {
    return true;
  }

  bool AirPlay_Protocol_AirTunes::stop()
  {
    return true;
  }

}
