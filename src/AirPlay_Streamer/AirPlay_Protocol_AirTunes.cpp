/**
 * 
 * AirPlay Protocol - AirPlay
 *
 */

#include "AirPlay_Protocol_AirTunes.h"
#include <sstream>

void * StartAirTunesThread(void * Arg)
{
  DCE::AirPlay_Protocol_AirTunes *pAirPlay_Protocol_AirTunes = 
    (DCE::AirPlay_Protocol_AirTunes *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pAirPlay_Protocol_AirTunes->ServerThread();
  return NULL;
}

int shairport_log(const char* msg, size_t msgSize)
{
  // This is a hack to remove the spurious line feed.
  string sMsg = msg;
  DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SHAIRPORT: %s",sMsg.substr(0,sMsg.size()-1).c_str());
  return 1;
}

namespace DCE
{

  AirPlay_Protocol_AirTunes::AirPlay_Protocol_AirTunes(AirPlay_Streamer *pAirPlay_Streamer)
    : AirPlay_Protocol(pAirPlay_Streamer)
  {
    Port_set(7322);
    ServiceType_set("_raop._tcp");
    TXTRecord_Add("am=LinuxMCE,1");
    TXTRecord_Add("tp=UDP");
    TXTRecord_Add("sm=false");
    TXTRecord_Add("da=true");
    TXTRecord_Add("vn=65537");
    TXTRecord_Add("sv=false");
    TXTRecord_Add("ek=1");
    TXTRecord_Add("et=0,1");
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
    stop();
  }

  bool AirPlay_Protocol_AirTunes::init()
  {
    m_ao.ao_initialize = AudioOutputFunctions::ao_initialize;
    m_ao.ao_play = AudioOutputFunctions::ao_play;
    m_ao.ao_default_driver_id = AudioOutputFunctions::ao_default_driver_id;
    m_ao.ao_open_live = AudioOutputFunctions::ao_open_live;
    m_ao.ao_close = AudioOutputFunctions::ao_close;
    m_ao.ao_append_option = AudioOutputFunctions::ao_append_option;
    m_ao.ao_free_options = AudioOutputFunctions::ao_free_options;
    m_ao.ao_get_option = AudioOutputFunctions::ao_get_option;

    if (!start())
      {
	return false;
      }
    else
      {
	return true;
      }

  }

  bool AirPlay_Protocol_AirTunes::start()
  {
    int numArgs = 3;

    char cName[100];
    char cMacAddress[100];
    char cPort[100];

    sprintf(cName,"--apname=LinuxMCE");
    sprintf(cPort,"--server_port=%d",Port_get());
    sprintf(cMacAddress,"--mac=%s",m_pAirPlay_Streamer->DeviceNameFromMacAddress(m_pAirPlay_Streamer->m_sCurrentMacAddress).c_str());

    char* argv[] = {cName, cPort, cMacAddress, NULL };
    struct printfPtr funcPtr;
    funcPtr.extprintf = shairport_log;
    shairport_set_printf(&funcPtr);
    shairport_set_ao(&m_ao);
    shairport_main(numArgs, argv);
    
    if (pthread_create(&m_serverThread, NULL, StartAirTunesThread, (void *) this))
      {
        // failed, bail.
	LoggerWrapper::GetInstance()
          ->Write(LV_CRITICAL,"AirPlay_Protocol_AirTunes::start() - failed to create server thread.");
        return false;
      }
    
    return true;
  }

  bool AirPlay_Protocol_AirTunes::stop()
  {
    return true;
    shairport_exit();
  }

  void AirPlay_Protocol_AirTunes::ServerThread()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ServerThread triggered.");
    while (!m_pAirPlay_Streamer->m_bQuit_get() && shairport_is_running())
      {
	shairport_loop();
      }
  }
  
  // beg ao functions

  void AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_initialize(void)
  {
    // TODO
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_initialize!");
  }

  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_play(ao_device *device, char *output_samples, uint32_t num_bytes)
  {
    // TODO
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_play!");
    return 0;
  }
  
  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_default_driver_id(void)
  {
    // TODO
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_default_driver_id!");
    return 0;
  }

  ao_device* AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_open_live( int driver_id, 
								 ao_sample_format *format,
								 ao_option *option)
  {
    // TODO
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_open_live!");
    return NULL;
  }
  
  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_close(ao_device *device)
  {
    // TODO
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_close!");
    return 0;
  }

  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_append_option(ao_option **options, const char *key, const char *value)
  {
    // TODO
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_append_option!");
    return 0;
  }

  void AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_free_options(ao_option *options) 
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_free_options!");
    // TODO
  }
  
  char* AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_get_option(ao_option *options, const char* key)
  {
    // TODO
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_get_option!");
    return NULL;
  }

  void AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_set_metadata(const char *buffer, unsigned int size)
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_set_metadata!");
    // TODO
  }

  void AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_set_metadata_coverart(const char *buffer, unsigned int size)
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_set_metadata_coverart!");
    // TODO

  }

  // end ao functions

}
