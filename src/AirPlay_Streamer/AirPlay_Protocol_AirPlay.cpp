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
    TXTRecord_Add("features=0x39f7");
    TXTRecord_Add("model=LinuxMCE,1");
    TXTRecord_Add("srcvers=130.14");
    
    m_pAirPlaySocketListener = new AirPlaySocketListener(string("m_pAirPlaySocketListener"));

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
    
    m_pAirPlaySocketListener->StartListening(Port_get());
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

  bool AirPlay_Protocol_AirPlay::AirPlaySocketListener::ReceivedString(Socket *pSocket, string sLine, int nTimeout)
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Received String %s",sLine.c_str());
    return true;
  }
  
}

