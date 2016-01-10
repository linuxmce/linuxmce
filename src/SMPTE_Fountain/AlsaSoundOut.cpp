/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <ctype.h>
#ifdef ENABLE_NLS
#include <locale.h>
#endif
#include <byteswap.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include <alsa/asoundlib.h>
#include <sys/time.h>
#include <math.h>
#include "SMPTEGen.h"

#ifndef min 
#define min(a,b) (((a)<(b))?(a):(b))
#endif 

				 
enum {
  TEST_PINK_NOISE = 1,
  TEST_SINE,
  TEST_WAV
};

#define MAX_CHANNELS	16

const char              *device      = "plug:rearx"; //"rear";       /* playback device */
static snd_pcm_format_t   format      = SND_PCM_FORMAT_S8; /* sample format */
static unsigned int       rate        = 8000;	            /* stream rate */
static unsigned int       channels    = 1;	            /* count of channels */
static unsigned int       speaker     = 0;	            /* count of channels */
static unsigned int       buffer_time = 0;	            /* ring buffer length in us */
static unsigned int       period_time = 0;	            /* period time in us */
static unsigned int       nperiods    = 4;                  /* number of periods */
static snd_pcm_uframes_t  buffer_size;
static snd_pcm_uframes_t  period_size;

long smpte_cur = 0;
long smpte_stop = -1; // test for 3 minutes.
bool alsa_soundout_shutdown = false;
bool smpte_pause = 0;

static const int	channels4[] = {
  0,
  1,
  3,
  2
};
static const int	channels6[] = {
  0,
  4,
  1,
  3,
  2,
  5
}; 
static const int	channels8[] = {
  0,
  4,
  1,
  7,
  3,
  2,
  6,
  5
}; 


static int set_hwparams(snd_pcm_t *handle, snd_pcm_hw_params_t *params, snd_pcm_access_t access) {
  unsigned int rrate;
  int          err;
  snd_pcm_uframes_t     period_size_min;
  snd_pcm_uframes_t     period_size_max;
  snd_pcm_uframes_t     buffer_size_min;
  snd_pcm_uframes_t     buffer_size_max;

  /* choose all parameters */
  err = snd_pcm_hw_params_any(handle, params);
  if (err < 0) {
    printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
    return err;
  }

  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, params, access);
  if (err < 0) {
    printf("Access type not available for playback: %s\n", snd_strerror(err));
    return err;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, params, format);
  if (err < 0) {
    printf("Sample format not available for playback: %s\n", snd_strerror(err));
    return err;
  }

  /* set the count of channels */
  err = snd_pcm_hw_params_set_channels(handle, params, channels);
  if (err < 0) {
    printf("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
    return err;
  }

  /* set the stream rate */
  rrate = rate;
  err = snd_pcm_hw_params_set_rate(handle, params, rate, 0);
  if (err < 0) {
    printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
    return err;
  }

  if (rrate != rate) {
    printf("Rate doesn't match (requested %iHz, get %iHz, err %d)\n", rate, rrate, err);
    return -EINVAL;
  }

  printf("Rate set to %iHz (requested %iHz)\n", rrate, rate);
  /* set the buffer time */
  err = snd_pcm_hw_params_get_buffer_size_min(params, &buffer_size_min);
  err = snd_pcm_hw_params_get_buffer_size_max(params, &buffer_size_max);
  err = snd_pcm_hw_params_get_period_size_min(params, &period_size_min, NULL);
  err = snd_pcm_hw_params_get_period_size_max(params, &period_size_max, NULL);
  printf("Buffer size range from %lu to %lu\n",buffer_size_min, buffer_size_max);
  printf("Period size range from %lu to %lu\n",period_size_min, period_size_max);
  if (period_time > 0) {
    printf("Requested period time %u us\n", period_time);
    err = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time, NULL);
    if (err < 0) {
      printf("Unable to set period time %u us for playback: %s\n",
	     period_time, snd_strerror(err));
      return err;
    }
  }
  if (buffer_time > 0) {
    printf("Requested buffer time %u us\n", buffer_time);
    err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, NULL);
    if (err < 0) {
      printf("Unable to set buffer time %u us for playback: %s\n",
	     buffer_time, snd_strerror(err));
      return err;
    }
  }
  if (! buffer_time && ! period_time) {
    buffer_size = buffer_size_max;
    printf("Using max buffer size %lu\n", buffer_size);
    err = snd_pcm_hw_params_set_buffer_size_near(handle, params, &buffer_size);
    if (err < 0) {
      printf("Unable to set buffer size %lu for playback: %s\n",
	     buffer_size, snd_strerror(err));
      return err;
    }
  }
  if (! buffer_time || ! period_time) {
    printf("Periods = %u\n", nperiods);
    err = snd_pcm_hw_params_set_periods_near(handle, params, &nperiods, NULL);
    if (err < 0) {
      printf("Unable to set nperiods %u for playback: %s\n",
	     nperiods, snd_strerror(err));
      return err;
    }
  }
  snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
  snd_pcm_hw_params_get_period_size(params, &period_size, NULL);
  printf("was set period_size = %lu\n",period_size);
  printf("was set buffer_size = %lu\n",buffer_size);
  if (2*period_size > buffer_size) {
    printf("buffer to small, could not use\n");
    return err;
  }

  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, params);
  if (err < 0) {
    printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
    return err;
  }

  return 0;
}

static int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams) {
  int err;

  /* get the current swparams */
  err = snd_pcm_sw_params_current(handle, swparams);
  if (err < 0) {
    printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
    return err;
  }

  /* start the transfer when a buffer is full */
  err = snd_pcm_sw_params_set_start_threshold(handle, swparams, buffer_size);
  if (err < 0) {
    printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
    return err;
  }

  /* allow the transfer when at least period_size frames can be processed */
  err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_size);
  if (err < 0) {
    printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
    return err;
  }

  /* align all transfers to 1 sample */
  err = snd_pcm_sw_params_set_xfer_align(handle, swparams, 1);
  if (err < 0) {
    printf("Unable to set transfer align for playback: %s\n", snd_strerror(err));
    return err;
  }

  /* write the parameters to the playback device */
  err = snd_pcm_sw_params(handle, swparams);
  if (err < 0) {
    printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
    return err;
  }

  return 0;
}

/*
 *   Underrun and suspend recovery
 */

static int xrun_recovery(snd_pcm_t *handle, int err) {
  if (err == -EPIPE) {	/* under-run */
    err = snd_pcm_prepare(handle);
    if (err < 0)
      printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
    return 0;
  } 
  else if (err == -ESTRPIPE) {

    while ((err = snd_pcm_resume(handle)) == -EAGAIN)
      sleep(1);	/* wait until the suspend flag is released */

    if (err < 0) {
      err = snd_pcm_prepare(handle);
      if (err < 0)
        printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
    }

    return 0;
  }

  return err;
}

/*
 * Handle WAV files
 */


static int write_buffer(snd_pcm_t *handle, uint8_t *ptr, int cptr)
{
  int err;

  while (cptr > 0) {

    err = snd_pcm_writei(handle, ptr, cptr);

    if (err == -EAGAIN)
      continue;

    if (err < 0) {
      printf("Write error: %d,%s\n", err, snd_strerror(err));
      if (xrun_recovery(handle, err) < 0) {
	printf("xrun_recovery failed: %d,%s\n", err, snd_strerror(err));
	return -1;
      }
      break;	/* skip one period */
    }

    ptr += snd_pcm_frames_to_bytes(handle, err);
    cptr -= err;
  }
  return 0;
}

static int write_loop(snd_pcm_t *handle, int channel, int periods, uint8_t *frames)
{
  int    err, frameoffset=0;

  int framesize = snd_pcm_frames_to_bytes(handle, period_size);
  int smptesize = 320;  // 320 is the typical size of a 8khz smpte frame.

  SMPTEGen smptegen;

  do
  {
		char *smptedata;
		if (smpte_cur > smpte_stop)
		{
			// Null pad this frame.  We don't want to just
			// fill the whole buffer, since another thread
			// might give us a new smpte value by the next iteration. 
			smptedata = new char[smptesize];
			memset(smptedata, 0, smptesize);
		}
		else
		{
			smptedata = smptegen.GetSMPTE(smpte_cur++, smptesize);
			if (smpte_pause)
				smpte_cur--;
		}
		int smptedataoffset=0;
		do
		{
			char *fp = (char *)frames;
			fp+=frameoffset;
			int fr = min(smptesize-smptedataoffset, framesize-frameoffset);
			memcpy(fp, smptedata+smptedataoffset, fr);
			frameoffset+=fr;
			smptedataoffset+=fr;
			if (frameoffset==framesize)
			{
				if ((err = write_buffer(handle, frames, framesize)) < 0)
					break;
				
				printf("SMPTE %s: Frame written to ALSA.\r", SMPTEGen::FromSMPTECode(smpte_cur));
				frameoffset=0;
			}
		} while(smptedataoffset < smptesize);
		delete[] smptedata;
  } while(alsa_soundout_shutdown == false);
  if (frameoffset > 0)
  {
	  write_buffer(handle, frames, frameoffset); 
  }
  snd_pcm_drain(handle);
  snd_pcm_prepare(handle);
  return 0;
}

void *OutputThread(void* param) 
{
  snd_pcm_t            *handle;
  int                   err;
  snd_pcm_hw_params_t  *hwparams;
  snd_pcm_sw_params_t  *swparams;
  uint8_t              *frames;
  alsa_soundout_shutdown = false;

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);
 
  printf("Playback device is %s\n", device);
  printf("Stream parameters are %iHz, %s, %i channels\n", rate, snd_pcm_format_name(format), channels);
  while ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) 
  {
    printf("Playback open error: %d,%s\n", err,snd_strerror(err));
	return NULL;
  }

  if ((err = set_hwparams(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) 
  {
    printf("Setting of hwparams failed: %s\n", snd_strerror(err));
    snd_pcm_close(handle);
	return NULL;
  }
  if ((err = set_swparams(handle, swparams)) < 0) 
  {
    printf("Setting of swparams failed: %s\n", snd_strerror(err));
    snd_pcm_close(handle);
    return NULL;
  }

  frames = (uint8_t *)malloc(snd_pcm_frames_to_bytes(handle, period_size));
  if (frames == NULL) {
    printf("Not enough memory\n");
    return NULL;
  }

  err = write_loop(handle, speaker-1, ((rate*5)/period_size), frames);
  free(frames);
  snd_pcm_close(handle);
  return NULL;
}
