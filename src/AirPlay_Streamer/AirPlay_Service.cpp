/**
 *
 * AirPlay Service Implementation
 *
 */

#include "AirPlay_Service.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "DCE/Logger.h"

/** Static foobage for the avahi static calls */
static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata);
static void create_services(AvahiClient *c, void * userdata);
static void client_callback(AvahiClient *c, AvahiClientState state, void * userdata);
static bool startAvahi(DCE::AirPlay_Service* pAirPlay_Service);
static bool stopAvahi(DCE::AirPlay_Service* pAirPlay_Service);

DCE::AirPlay_Service::AirPlay_Service(AirPlay_Streamer *pAirPlay_Streamer)
{
  m_pAirPlay_Streamer = pAirPlay_Streamer;
  m_pAirPlay_Protocol_Factory = new AirPlay_Protocol_Factory(pAirPlay_Streamer);
  m_pAvahi_Client=NULL;
  m_pAvahi_Entry_Group=NULL;
  m_pAvahi_Threaded_Poll=NULL;
}

DCE::AirPlay_Service::~AirPlay_Service()
{
  stopAvahi(this);
  // delete m_pAirPlay_Protocol_Factory;
}

bool DCE::AirPlay_Service::init()
{
  if (!m_pAirPlay_Protocol_Factory->init())
    {
      return false;
    }
  return true;
}

bool DCE::AirPlay_Service::start()
{
  startAvahi(this);
  return true;
}

void DCE::AirPlay_Service::Name_set(string sName)
{
  m_cName = avahi_strdup(sName.c_str());
}

void DCE::AirPlay_Service::Name_AirPlay_set(string sName)
{
  m_cAirPlayName = avahi_strdup(sName.c_str());
}

void DCE::AirPlay_Service::MacAddress_set(string sMacAddress)
{
  m_sMacAddress = sMacAddress;
}

void DCE::AirPlay_Service::sanityCheck()
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sanity check. We're still here!");
}

static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata)
{
  /* Called whenever the entry group state changes */
  DCE::AirPlay_Service *pAirPlay_Service = (DCE::AirPlay_Service *)userdata;

  DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"entry_group_callback, state %d",state);

  switch (state) {
  case AVAHI_ENTRY_GROUP_ESTABLISHED :
    /* The entry group has been established successfully */
    DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS,"Service successfully established.");
    break;

  case AVAHI_ENTRY_GROUP_COLLISION : {
    char *n;

    /* A service name collision with a remote service
     * happened. Let's pick a new name */
    n = avahi_alternative_service_name(pAirPlay_Service->m_cName);
    avahi_free(pAirPlay_Service->m_cName);
    pAirPlay_Service->m_cName = n;

    /* And recreate the services */
    create_services(avahi_entry_group_get_client(g), pAirPlay_Service);
    break;
  }

  case AVAHI_ENTRY_GROUP_FAILURE :

    DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Entry group failure: %s", avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));

    /* Some kind of failure happened while we were registering our services */
    avahi_threaded_poll_stop(pAirPlay_Service->m_pAvahi_Threaded_Poll);
    avahi_entry_group_free(g);
    avahi_client_free(pAirPlay_Service->m_pAvahi_Client);
    avahi_threaded_poll_free(pAirPlay_Service->m_pAvahi_Threaded_Poll);
    pAirPlay_Service->m_pAirPlay_Streamer->m_bQuit_set(true);
    break;

  case AVAHI_ENTRY_GROUP_UNCOMMITED:
  case AVAHI_ENTRY_GROUP_REGISTERING:
    ;
  }
}


static void create_services(AvahiClient *c, void * userdata) {
  char *n;
  int ret;
  assert(c);

  DCE::AirPlay_Service *pAirPlay_Service = (DCE::AirPlay_Service *)userdata;

  /* If this is the first time we're called, let's create a new
   * entry group if necessary */

  if (!pAirPlay_Service->m_pAvahi_Entry_Group)
    if (!(pAirPlay_Service->m_pAvahi_Entry_Group = avahi_entry_group_new(c, entry_group_callback, NULL))) {
      DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"avahi_entry_group_new() failed: %s\n", avahi_strerror(avahi_client_errno(c)));
      goto fail;
    }

  /* If the group is empty (either because it was just created, or
   * because it was reset previously, add our entries.  */

  if (avahi_entry_group_is_empty(pAirPlay_Service->m_pAvahi_Entry_Group)) {
    DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Creating Services...");

    // COMEBACKHERE and Implement the shit.
    vector<DCE::AirPlay_Protocol *> vectAirPlayProtocols = pAirPlay_Service->m_pAirPlay_Protocol_Factory->allAirPlayProtocols();
    for (vector<DCE::AirPlay_Protocol *>::iterator it = vectAirPlayProtocols.begin();
	 it != vectAirPlayProtocols.end(); ++it)
      {
	DCE::AirPlay_Protocol *pAirPlay_Protocol = *it;
	DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"strlst is %s",
						 avahi_string_list_to_string(pAirPlay_Protocol->TXTRecords_get()));
	string servicetmp = pAirPlay_Protocol->ServiceType_get();
	if (servicetmp == "_airplay._tcp")
	  {
 	    if ((ret = avahi_entry_group_add_service_strlst(pAirPlay_Service->m_pAvahi_Entry_Group,
							    AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 
							    (AvahiPublishFlags)0,
							    pAirPlay_Service->m_cAirPlayName,
							    pAirPlay_Protocol->ServiceType_get(),
							    NULL, NULL, 
							    pAirPlay_Protocol->Port_get(),
							    pAirPlay_Protocol->TXTRecords_get())) > 0)
	      {
		if (ret == AVAHI_ERR_COLLISION)
		  goto collision;
		
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not add service %s",pAirPlay_Service->m_cName);
	      }
	  }
	else
	  {
	    // Alternate Naming done here.
 	    if ((ret = avahi_entry_group_add_service_strlst(pAirPlay_Service->m_pAvahi_Entry_Group,
							    AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 
							    (AvahiPublishFlags)0,
							    pAirPlay_Service->m_cName,
							    pAirPlay_Protocol->ServiceType_get(),
							    NULL, NULL, 
							    pAirPlay_Protocol->Port_get(),
							    pAirPlay_Protocol->TXTRecords_get())) > 0)
	      {
		if (ret == AVAHI_ERR_COLLISION)
		  goto collision;
		
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not add service %s",pAirPlay_Service->m_cName);
	      }	    
	  }
      }
    
    
    /* Tell the server to register the service */
    if ((ret = avahi_entry_group_commit(pAirPlay_Service->m_pAvahi_Entry_Group)) < 0) {
      DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to commit entry group: %s\n", avahi_strerror(ret));
      goto fail;
    }
  }

  return;

 collision: // this is why I hate gotos, seriously... 

  /* A service name collision with a local service happened. Let's
   * pick a new name */

  n = avahi_alternative_service_name(pAirPlay_Service->m_cName);
  avahi_free(pAirPlay_Service->m_cName);
  pAirPlay_Service->m_cName = n;
  
  DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Service Name Collision. renaming to %s",pAirPlay_Service->m_cName);
  avahi_entry_group_reset(pAirPlay_Service->m_pAvahi_Entry_Group);

  create_services(c, userdata);
  return;

 fail:
    avahi_threaded_poll_stop(pAirPlay_Service->m_pAvahi_Threaded_Poll);
    avahi_client_free(pAirPlay_Service->m_pAvahi_Client);
    avahi_threaded_poll_free(pAirPlay_Service->m_pAvahi_Threaded_Poll);
}


/**
 * The client callback as required by AvahiClient
 */
static void client_callback(AvahiClient *c, AvahiClientState state, void * userdata)
{
  DCE::AirPlay_Service* pAirPlay_Service = (DCE::AirPlay_Service *)userdata;

  pAirPlay_Service->sanityCheck();

  switch(state)
    {
    case AVAHI_CLIENT_S_RUNNING:
      create_services(c, pAirPlay_Service);
      break;
    case AVAHI_CLIENT_FAILURE:
      avahi_threaded_poll_stop(pAirPlay_Service->m_pAvahi_Threaded_Poll);
      avahi_client_free(c);
      avahi_threaded_poll_free(pAirPlay_Service->m_pAvahi_Threaded_Poll);
      pAirPlay_Service->m_pAirPlay_Streamer->m_bQuit_set(true);
      break;
    case AVAHI_CLIENT_S_COLLISION:
    case AVAHI_CLIENT_S_REGISTERING:
      DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"client_callback - registering.");
      if (pAirPlay_Service->m_pAvahi_Entry_Group)
	{
	  avahi_entry_group_reset(pAirPlay_Service->m_pAvahi_Entry_Group);
	}
      break;
    case AVAHI_CLIENT_CONNECTING:
      break;
    }
}

/**
 * Start the Avahi poll loop. Use after everything has been
 * initialized, and we are ready to publish our services.
 */
static bool startAvahi(DCE::AirPlay_Service* pAirPlay_Service)
{
  int error;

  if (!(pAirPlay_Service->m_pAvahi_Threaded_Poll = avahi_threaded_poll_new()))
    {
      DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not initialize a new thread poll for avahi.");
      return false;
    }

  if (!pAirPlay_Service->m_cName)
    {
      DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Name not set. Bailing.");
      return false;
    }

  pAirPlay_Service->m_pAvahi_Client = 
    avahi_client_new(avahi_threaded_poll_get(pAirPlay_Service->m_pAvahi_Threaded_Poll), 
		     (AvahiClientFlags)0, 
		     client_callback, 
		     pAirPlay_Service,
		     &error);

  if (!pAirPlay_Service->m_pAvahi_Client)
    {
      DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not create Avahi Client object, Error: %s",avahi_strerror(error));
    }

  avahi_threaded_poll_start(pAirPlay_Service->m_pAvahi_Threaded_Poll);

  return true;
  
}

static bool stopAvahi(DCE::AirPlay_Service* pAirPlay_Service)
{
    avahi_threaded_poll_stop(pAirPlay_Service->m_pAvahi_Threaded_Poll);
    avahi_client_free(pAirPlay_Service->m_pAvahi_Client);
    avahi_threaded_poll_free(pAirPlay_Service->m_pAvahi_Threaded_Poll);
  return true;
}
