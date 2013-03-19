/**
 * 
 * AirPlay Protocol Factory
 *  
 */

#ifndef AIRPLAYPROTOCOLFACTORY_H
#define AIRPLAYPROTOCOLFACTORY_H

#include "AirPlay_Protocol.h"
#include "AirPlay_Streamer.h"

namespace DCE 
{
  class AirPlay_Streamer;
  class AirPlay_Protocol;

  class AirPlay_Protocol_Factory 
  {
  public:
    AirPlay_Streamer *m_pAirPlay_Streamer;
    AirPlay_Protocol_Factory(AirPlay_Streamer *pAirPlay_Streamer);
    ~AirPlay_Protocol_Factory();
    bool init();
    void addProtocolForMediaType(int iPK_MediaType, AirPlay_Protocol *pAirPlay_Protocol);
    AirPlay_Protocol* getAirPlayProtocolForMediaType(int iPK_MediaType);
    vector<AirPlay_Protocol *> allAirPlayProtocols()
      {
	vector<AirPlay_Protocol *> vectRet;
	for (map<int, AirPlay_Protocol*>::iterator it = m_mapAirPlayProtocolForMediaType.begin(); 
	     it != m_mapAirPlayProtocolForMediaType.end(); ++it)
	  {
	    vectRet.push_back(it->second);
	  }
	return vectRet;
      }
  private:
    map<int, AirPlay_Protocol*> m_mapAirPlayProtocolForMediaType;

  };

}

#endif
