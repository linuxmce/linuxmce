/**
 * 
 * AirPlay Protocol Implementation
 *
 */

#include "AirPlay_Protocol.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>

namespace DCE 
{

  AirPlay_Protocol::AirPlay_Protocol(AirPlay_Streamer *pAirPlay_Streamer)
  {
    m_pAirPlay_Streamer = pAirPlay_Streamer;
    m_iPort=0;
    m_cServiceType=NULL;
    m_aslTXTRecords = avahi_string_list_new(NULL);
       // char tmp[100];
//    sprintf(tmp, "device=%s",pAirPlay_Streamer->m_sCurrentMacAddress.c_str());
//    string tmpStr = tmp;
//    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ADDR IS %s",tmpStr.c_str());
//    TXTRecord_Add(tmpStr);
  }

  AirPlay_Protocol::~AirPlay_Protocol()
  {
    delete m_pAirPlay_Streamer;
    m_pAirPlay_Streamer = NULL;
    if (m_cServiceType)
      {
	avahi_free(m_cServiceType);
      }
    if (m_aslTXTRecords)
      {
	avahi_string_list_free(m_aslTXTRecords);
      }
  }

  void AirPlay_Protocol::Port_set(int iPort)
  {
    m_iPort = iPort;
  }

  int AirPlay_Protocol::Port_get()
  {
    return m_iPort;
  }

  void AirPlay_Protocol::ServiceType_set(string sServiceType)
  {
    if (m_cServiceType)
      {
	avahi_free(m_cServiceType);
      }
    m_cServiceType = avahi_strdup(sServiceType.c_str());
  }

  char* AirPlay_Protocol::ServiceType_get()
  {
    return m_cServiceType;
  }

  void AirPlay_Protocol::TXTRecord_Add(string sVal)
  {
    m_aslTXTRecords = avahi_string_list_add(m_aslTXTRecords, sVal.c_str());
  }

  AvahiStringList* AirPlay_Protocol::TXTRecords_get()
  {
    return m_aslTXTRecords;
  }

  bool AirPlay_Protocol::init()
  {
    return true;
  }
  
  bool AirPlay_Protocol::start()
  {
    return true;
  }

  bool AirPlay_Protocol::stop()
  {
    return true;
  }

}
