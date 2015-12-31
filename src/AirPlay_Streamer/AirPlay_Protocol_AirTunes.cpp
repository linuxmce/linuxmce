/**
 * 
 * AirPlay Protocol - AirPlay
 *
 */

#include "AirPlay_Protocol_AirTunes.h"
#include "PlutoUtils/FileUtils.h"

DCE::AirPlay_Streamer *gAirPlay_Streamer;

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
    gAirPlay_Streamer = pAirPlay_Streamer;
    Port_set(7322);
    ServiceType_set("_raop._tcp");
    char tmp[100];
    sprintf(tmp, "device=%s",pAirPlay_Streamer->m_sCurrentMacAddress.c_str());
    string tmpStr = tmp;
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ADDR IS %s",tmpStr.c_str());
    TXTRecord_Add(tmpStr);
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
#ifdef LMCE_SHAIRPORT
    m_ao.ao_set_metadata = AudioOutputFunctions::ao_set_metadata;    
    m_ao.ao_set_metadata_coverart = AudioOutputFunctions::ao_set_metadata_coverart;        
#endif
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
#ifdef PIPE
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_initialize!");
#else
    ::ao_initialize();
#endif
  }

  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_play(ao_device *device, char *output_samples, uint32_t num_bytes)
  {
#ifdef PIPE
    if (!device)
      return 0;
    
    ao_device_linuxmce* device_linuxmce = (ao_device_linuxmce*) device;
    
#define NUM_OF_BYTES 64
    
    unsigned int sentBytes = 0;
    unsigned char buf[NUM_OF_BYTES];
    while (sentBytes < num_bytes)
      {
	int n = (num_bytes - sentBytes < NUM_OF_BYTES ? num_bytes - sentBytes : NUM_OF_BYTES);
	memcpy(buf, (char*) output_samples + sentBytes, n);

	if (device_linuxmce->pipe->Write(buf, n) == 0)
	  return 0;
	
	sentBytes += n;
      }
    
    return 1;
#else
    return ::ao_play(device, output_samples, num_bytes);
#endif
  }
  
  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_default_driver_id(void)
  {
#ifdef PIPE
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_default_driver_id!");
    return 0;
#else
    return ::ao_default_driver_id();
#endif
  }

  ao_device* AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_open_live( int driver_id, 
								 ao_sample_format *format,
								 ao_option *option)
  {
#ifdef PIPE
    // TODO - more

    ao_device_linuxmce* device = new ao_device_linuxmce();
    
    device->pipe = new AirTunesPipe("/tmp/airplay.fifo");
    device->pipe->Open();

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_open_live!");
    
    if (ao_get_option(option, "artist"))
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX Artist is %s",ao_get_option(option, "artist"));
    
    if (ao_get_option(option, "album"))
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX Album is %s",ao_get_option(option, "album"));
    
    if (ao_get_option(option, "name"))
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX Title is %s",ao_get_option(option, "name"));
    
    return (ao_device*) device;
#else
    if (ao_get_option(option, "artist"))
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX Artist is %s",ao_get_option(option, "artist"));
    
    if (ao_get_option(option, "album"))
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX Album is %s",ao_get_option(option, "album"));
    
    if (ao_get_option(option, "name"))
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX Title is %s",ao_get_option(option, "name"));

    gAirPlay_Streamer->StartAirTunesPlayback();
    return ::ao_open_live(driver_id, format, option);
#endif
  }
  
  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_close(ao_device *device)
  {
#ifdef PIPE
    // TODO
    ao_device_linuxmce* device_linuxmce = (ao_device_linuxmce*) device;
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_close!");
    device_linuxmce->pipe->Close();
    delete device_linuxmce->pipe;
    return 0;
#else
    if (!gAirPlay_Streamer->m_bIsPlaying)
      gAirPlay_Streamer->StopAirTunesPlayback();
    return ::ao_close(device);
#endif
  }

  int AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_append_option(ao_option **options, const char *key, const char *value)
  {
#ifdef PIPE
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_append_option!");
    ao_option *op, *list;
    
    op = (ao_option*) calloc(1,sizeof(ao_option));
    if (op == NULL) return 0;
    
    op->key = strdup(key);
    op->value = strdup(value?value:"");
    op->next = NULL;
    
    if ((list = *options) != NULL)
      {
	list = *options;
	while (list->next != NULL)
	  list = list->next;
	list->next = op;
      }
    else
      {
	*options = op;
      }
    
    return 1;
#else
    return ::ao_append_option(options, key, value);
#endif  
  }

  void AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_free_options(ao_option *options) 
  {
#ifdef PIPE
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_free_options!");
    ao_option *rest;
    
    while (options != NULL)
      {
	rest = options->next;
	free(options->key);
	free(options->value);
	free(options);
	options = rest;
      }
    
    // TODO
#else
    ::ao_free_options(options);
#endif
  }
  
  char* AirPlay_Protocol_AirTunes::AudioOutputFunctions::ao_get_option(ao_option *options, const char* key)
  {
#ifdef PIPE
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ao_get_option!");
    while (options != NULL)
      {
	if (strcmp(options->key, key) == 0)
	  return options->value;
	options = options->next;
      }
    
    return NULL;
#else
    return NULL;
#endif 
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

  // airtunes pipe functions

  AirPlay_Protocol_AirTunes::AirTunesPipe::AirTunesPipe(string sPath)
  {
    m_bIsOpen=false;
    m_fifoFd=0;
    m_sPath=sPath;
  }

  AirPlay_Protocol_AirTunes::AirTunesPipe::~AirTunesPipe()
  {
    m_bIsOpen=false;
    m_fifoFd=0;
  }

  bool AirPlay_Protocol_AirTunes::AirTunesPipe::Open()
  {
    unlink(m_sPath.c_str());
    mknod(m_sPath.c_str(), S_IFIFO | 0666, 0);
    m_fifoFd = open(m_sPath.c_str(), O_WRONLY);
    return true;
  }

  bool AirPlay_Protocol_AirTunes::AirTunesPipe::Close()
  {
    close(m_fifoFd);
    return true;
  }

  int AirPlay_Protocol_AirTunes::AirTunesPipe::Write(unsigned char* cBuffer, size_t iLen)
  {
    return write(m_fifoFd, cBuffer, iLen);
  }

  // end airtunes pipe funcs

}
