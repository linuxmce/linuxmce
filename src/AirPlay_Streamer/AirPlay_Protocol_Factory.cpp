/**
 *
 * Airplay Protocol Factory Implementation
 *
 */

#include "AirPlay_Protocol_Factory.h"
#include "AirPlay_Protocol.h"
#include "AirPlay_Protocol_AirPlay.h"
#include "AirPlay_Protocol_AirTunes.h"

namespace DCE 
{

  /* ctor */ 
  AirPlay_Protocol_Factory::AirPlay_Protocol_Factory(AirPlay_Streamer *pAirPlay_Streamer)
  {
    m_pAirPlay_Streamer = pAirPlay_Streamer;
  }

  /* dtor */
  AirPlay_Protocol_Factory::~AirPlay_Protocol_Factory()
  {
    for (map<int, AirPlay_Protocol *>::iterator it = m_mapAirPlayProtocolForMediaType.begin();
	 it != m_mapAirPlayProtocolForMediaType.end();
	 ++it)
      {
	delete (*it).second;
      }

    m_mapAirPlayProtocolForMediaType.clear();

  }

  void AirPlay_Protocol_Factory::addProtocolForMediaType(int iPK_MediaType, AirPlay_Protocol *pAirPlay_Protocol)
  {
    if (!pAirPlay_Protocol)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_Factory::addProtocolForMediaType(%d) pAirPlayProtocol was NULL, not adding.",iPK_MediaType);
      }
    else
      {
	m_mapAirPlayProtocolForMediaType[iPK_MediaType] = pAirPlay_Protocol;
	pAirPlay_Protocol->init();
      }
  }

  AirPlay_Protocol* AirPlay_Protocol_Factory::getAirPlayProtocolForMediaType(int iPK_MediaType)
  {
    map<int, AirPlay_Protocol *>::iterator it=m_mapAirPlayProtocolForMediaType.find(iPK_MediaType);
    return it == m_mapAirPlayProtocolForMediaType.end() ? NULL : it->second;
  }

  bool AirPlay_Protocol_Factory::init()
  {
    addProtocolForMediaType(61, new AirPlay_Protocol_AirPlay(m_pAirPlay_Streamer));
    addProtocolForMediaType(60, new AirPlay_Protocol_AirTunes(m_pAirPlay_Streamer));
    // Come back in here and register the protocols.
    return true;
  }

}
