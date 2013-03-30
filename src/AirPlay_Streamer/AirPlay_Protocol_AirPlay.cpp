/**
 * 
 * AirPlay Protocol - AirPlay
 *
 */

#include "AirPlay_Protocol_AirPlay.h"
#include "PlutoUtils/CommonIncludes.h"

void * StartAirPlayThread(void * Arg)
{
  DCE::AirPlay_Protocol_AirPlay *pAirPlay_Protocol_AirPlay = (DCE::AirPlay_Protocol_AirPlay *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pAirPlay_Protocol_AirPlay->ServerThread();
  return NULL;
}

namespace DCE
{

  AirPlay_Protocol_AirPlay::AirPlay_Protocol_AirPlay(AirPlay_Streamer *pAirPlay_Streamer)
    : AirPlay_Protocol(pAirPlay_Streamer)
  {
    Port_set(7000);
    ServiceType_set("_airplay._tcp");
    char tmp[100];
    sprintf(tmp, "deviceid=%s",pAirPlay_Streamer->m_sCurrentMacAddress.c_str());
    string tmpStr = tmp;
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ADDR IS %s",tmpStr.c_str());
    TXTRecord_Add(tmpStr);
    TXTRecord_Add("features=0x77");
    TXTRecord_Add("model=LinuxMCE,1");
    TXTRecord_Add("srcvers=101.10");
    
  }

  AirPlay_Protocol_AirPlay::~AirPlay_Protocol_AirPlay()
  {
    // All relevant dtor called in base class.
    if (m_pAirPlaySocketListener)
      {
	delete m_pAirPlaySocketListener;
	m_pAirPlaySocketListener=NULL;
      }
  }

  bool AirPlay_Protocol_AirPlay::init()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Protocol_AirPlay::init() - Spawning server thread.");
    
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

  void AirPlay_Protocol_AirPlay::ServerThread()
  {
    while (!m_pAirPlay_Streamer->m_bQuit_get())
      {
	// Implement me.
	Sleep(1000);
      }
  }
  
}

