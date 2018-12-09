/** 
 *
 * AirPlay Protocol - AirTunes
 *
 */

#ifndef AIRPLAYPROTOCOLAIRTUNES_H
#define AIRPLAYPROTOCOLAIRTUNES_H

#include "AirPlay_Protocol.h"
#include <shairport/shairport.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define AIRTUNES_FIFO "/tmp/airtunes_fifo"

namespace DCE
{
  class AirPlay_Protocol_AirTunes : public AirPlay_Protocol
    {
    private:
      pthread_t m_serverThread;
    protected:
    public:
      AirPlay_Protocol_AirTunes(AirPlay_Streamer *pAirPlay_Streamer);
      ~AirPlay_Protocol_AirTunes();

      virtual bool init();
      virtual bool start();
      virtual bool stop();

      void ServerThread();

      char* m_cPortStr;
      char* m_cMacAddress;

      struct AudioOutput m_ao;

      class AudioOutputFunctions
      {
      public:
	// ao functions
	static void ao_initialize(void);
	static int ao_play(ao_device *device, char *output_samples, uint32_t num_bytes);
	static int ao_default_driver_id(void);
	static ao_device* ao_open_live( int driver_id, ao_sample_format *format,
					ao_option *option);
	static int ao_close(ao_device *device);
	/* -- Device Setup/Playback/Teardown -- */
	static int ao_append_option(ao_option **options, const char *key, const char *value);
	static void ao_free_options(ao_option *options);
	static char* ao_get_option(ao_option *options, const char* key);
	static void ao_set_metadata(const char *buffer, unsigned int size);
	static void ao_set_metadata_coverart(const char *buffer, unsigned int size);
      };

      class AirTunesPipe
      {
      private:
	bool m_bIsOpen;
	int m_fifoFd;

      public:

	string m_sPath;

	AirTunesPipe(string sPath);
	~AirTunesPipe();
	bool Open();
	bool Close();
	int Write(unsigned char* cBuffer, size_t iLen);
      };

      struct ao_device_linuxmce
      {
	AirTunesPipe *pipe;
      };

      char* m_argv[];
    };

}

#endif
