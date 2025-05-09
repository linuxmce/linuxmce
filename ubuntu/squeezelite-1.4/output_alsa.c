/* 
 *  Squeezelite - lightweight headless squeezebox emulator
 *
 *  (c) Adrian Smith 2012, 2013, triode1@btinternet.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Output using Alsa

#include "squeezelite.h"

#if ALSA

#include <alsa/asoundlib.h>
#include <sys/mman.h>
#include <malloc.h>

#define MAX_DEVICE_LEN 128

static snd_pcm_format_t fmts[] = { SND_PCM_FORMAT_S32_LE, SND_PCM_FORMAT_S24_LE, SND_PCM_FORMAT_S24_3LE, SND_PCM_FORMAT_S16_LE,
								   SND_PCM_FORMAT_UNKNOWN };

#if SL_LITTLE_ENDIAN
#define NATIVE_FORMAT SND_PCM_FORMAT_S32_LE
#else
#define NATIVE_FORMAT SND_PCM_FORMAT_S32_BE
#endif

// ouput device
static struct {
	char device[MAX_DEVICE_LEN + 1];
	snd_pcm_format_t format;
	snd_pcm_uframes_t buffer_size;
	snd_pcm_uframes_t period_size;
	unsigned rate;
	bool mmap;
	bool reopen;
	u8_t *write_buf;
} alsa;

static snd_pcm_t *pcmp = NULL;

extern u8_t *silencebuf;
#if DSD
extern u8_t *silencebuf_dop;
#endif

static log_level loglevel;

static bool running = true;

extern struct outputstate output;
extern struct buffer *outputbuf;

#define LOCK   mutex_lock(outputbuf->mutex)
#define UNLOCK mutex_unlock(outputbuf->mutex)

void list_devices(void) {
	void **hints, **n;
	if (snd_device_name_hint(-1, "pcm", &hints) >= 0) {
		n = hints;
		printf("Output devices:\n");
		while (*n) {
			char *name = snd_device_name_get_hint(*n, "NAME");
			char *desc = snd_device_name_get_hint(*n, "DESC");
			if (name) printf("  %-30s", name);
			if (desc) {
				char *s1 = strtok(desc, "\n");
				char *s2 = strtok(NULL, "\n");
				if (s1) printf(" - %s", s1);
				if (s2) printf(" - %s", s2);
			}
			printf("\n");
			if (name) free(name);
			if (desc) free(desc);
			n++;
		}
		snd_device_name_free_hint(hints);
	}
	printf("\n");
}

static void *alsa_error_handler(const char *file, int line, const char *function, int err, const char *fmt, ...) {
	va_list args;
	if ((loglevel >= lINFO && err == 0) || loglevel >= lDEBUG) {
		fprintf(stderr, "%s ALSA %s:%d ", logtime(), function, line);
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		fprintf(stderr, "\n");
		fflush(stderr);
	}
	return NULL;
}

static void alsa_close(void) {
	int err;
	if ((err = snd_pcm_close(pcmp)) < 0) {
		LOG_INFO("snd_pcm_close error: %s", snd_strerror(err));
	}
}

bool test_open(const char *device, unsigned rates[]) {
	int err;
	snd_pcm_t *pcm;
	snd_pcm_hw_params_t *hw_params;
	hw_params = (snd_pcm_hw_params_t *) alloca(snd_pcm_hw_params_sizeof());
	memset(hw_params, 0, snd_pcm_hw_params_sizeof());

	// open device
	if ((err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		LOG_ERROR("playback open error: %s", snd_strerror(err));
		return false;
	}

	// get max params
	if ((err = snd_pcm_hw_params_any(pcm, hw_params)) < 0) {
		LOG_ERROR("hwparam init error: %s", snd_strerror(err));
		return false;
	}

	// find supported sample rates to enable client side resampling of non supported rates
	unsigned i, ind;
	unsigned ref[] TEST_RATES;

	for (i = 0, ind = 0; ref[i]; ++i) {
		if (snd_pcm_hw_params_test_rate(pcm, hw_params, ref[i], 0) == 0) {
			rates[ind++] = ref[i];
		}
	}

	if ((err = snd_pcm_close(pcm)) < 0) {
		LOG_ERROR("snd_pcm_close error: %s", snd_strerror(err));
		return false;
	}

	return true;
}

static bool pcm_probe(const char *device) {
	int err;
	snd_pcm_t *pcm;

	if ((err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		return false;
	}

	if ((err = snd_pcm_close(pcm)) < 0) {
		LOG_ERROR("snd_pcm_close error: %s", snd_strerror(err));
	}

	return true;
}

static int alsa_open(const char *device, unsigned sample_rate, unsigned alsa_buffer, unsigned alsa_period) {
	int err;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_hw_params_alloca(&hw_params);

	// close if already open
	if (pcmp) alsa_close();

	// reset params
	alsa.rate = 0;
	alsa.period_size = 0;
	strcpy(alsa.device, device);

	if (strlen(device) > MAX_DEVICE_LEN - 4 - 1) {
		LOG_ERROR("device name too long: %s", device);
		return -1;
	}

	bool retry;
	do {
		// open device
		if ((err = snd_pcm_open(&pcmp, alsa.device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
			LOG_ERROR("playback open error: %s", snd_strerror(err));
			return err;
		}

		// init params
		memset(hw_params, 0, snd_pcm_hw_params_sizeof());
		if ((err = snd_pcm_hw_params_any(pcmp, hw_params)) < 0) {
			LOG_ERROR("hwparam init error: %s", snd_strerror(err));
			return err;
		}

		// open hw: devices without resampling, if sample rate fails try plughw: with resampling
		bool hw = !strncmp(alsa.device, "hw:", 3);
		retry = false;

		if ((err = snd_pcm_hw_params_set_rate_resample(pcmp, hw_params, !hw)) < 0) {
			LOG_ERROR("resampling setup failed: %s", snd_strerror(err));
			return err;
		}

		if ((err = snd_pcm_hw_params_set_rate(pcmp, hw_params, sample_rate, 0)) < 0) {
			if (hw) {
				strcpy(alsa.device + 4, device);
				memcpy(alsa.device, "plug", 4);
				LOG_INFO("reopening device %s in plug mode as %s for resampling", device, alsa.device);
				snd_pcm_close(pcmp);
				retry = true;
			}
		}

	} while (retry);

	// set access 
	if (!alsa.mmap || snd_pcm_hw_params_set_access(pcmp, hw_params, SND_PCM_ACCESS_MMAP_INTERLEAVED) < 0) {
		if ((err = snd_pcm_hw_params_set_access(pcmp, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
			LOG_ERROR("access type not available: %s", snd_strerror(err));
			return err;
		}
		alsa.mmap = false;
	}

	// set the sample format
	snd_pcm_format_t *fmt = alsa.format ? &alsa.format : (snd_pcm_format_t *)fmts;
	do {
		if (snd_pcm_hw_params_set_format(pcmp, hw_params, *fmt) >= 0) {
			LOG_INFO("opened device %s using format: %s sample rate: %u mmap: %u", alsa.device, snd_pcm_format_name(*fmt), sample_rate, alsa.mmap);
			alsa.format = *fmt;
			break;
		}
		if (alsa.format) {
			LOG_ERROR("unable to open audio device requested format: %s", snd_pcm_format_name(alsa.format));
			return -1;
		}
		++fmt; 
		if (*fmt == SND_PCM_FORMAT_UNKNOWN) {
			LOG_ERROR("unable to open audio device with any supported format");
			return -1;
		}
	} while (*fmt != SND_PCM_FORMAT_UNKNOWN);

	// set the output format to be used by _scale_and_pack
	switch(alsa.format) {
	case SND_PCM_FORMAT_S32_LE:
		output.format = S32_LE; break;
	case SND_PCM_FORMAT_S24_LE: 
		output.format = S24_LE; break;
	case SND_PCM_FORMAT_S24_3LE:
		output.format = S24_3LE; break;
	case SND_PCM_FORMAT_S16_LE: 
		output.format = S16_LE; break;
	default: 
		break;
	}

	// set channels
	if ((err = snd_pcm_hw_params_set_channels (pcmp, hw_params, 2)) < 0) {
		LOG_ERROR("channel count not available: %s", snd_strerror(err));
		return err;
	}

	// set period size - value of < 50 treated as period count, otherwise size in bytes
	if (alsa_period < 50) {
		unsigned count = alsa_period;
		if ((err = snd_pcm_hw_params_set_periods_near(pcmp, hw_params, &count, 0)) < 0) {
			LOG_ERROR("unable to set period count %s", snd_strerror(err));
			return err;
		}
	} else {
		snd_pcm_uframes_t size = alsa_period;
		int dir = 0;
		if ((err = snd_pcm_hw_params_set_period_size_near(pcmp, hw_params, &size, &dir)) < 0) {
			LOG_ERROR("unable to set period size %s", snd_strerror(err));
			return err;
		}
	}

	// set buffer size - value of < 500 treated as buffer time in ms, otherwise size in bytes
	if (alsa_buffer < 500) {
		unsigned time = alsa_buffer * 1000;
		int dir = 0;
		if ((err = snd_pcm_hw_params_set_buffer_time_near(pcmp, hw_params, &time, &dir)) < 0) {
			LOG_ERROR("unable to set buffer time %s", snd_strerror(err));
			return err;
		}
	} else {
		snd_pcm_uframes_t size = alsa_buffer;
		if ((err = snd_pcm_hw_params_set_buffer_size_near(pcmp, hw_params, &size)) < 0) {
			LOG_ERROR("unable to set buffer size %s", snd_strerror(err));
			return err;
		}
	}

	// get period_size
	if ((err = snd_pcm_hw_params_get_period_size(hw_params, &alsa.period_size, 0)) < 0) {
		LOG_ERROR("unable to get period size: %s", snd_strerror(err));
		return err;
	}

	// get buffer_size
	if ((err = snd_pcm_hw_params_get_buffer_size(hw_params, &alsa.buffer_size)) < 0) {
		LOG_ERROR("unable to get buffer size: %s", snd_strerror(err));
		return err;
	}

	LOG_INFO("buffer: %u period: %u -> buffer size: %u period size: %u", alsa_buffer, alsa_period, alsa.buffer_size, alsa.period_size);

	// ensure we have two buffer sizes of samples before starting output
	output.start_frames = alsa.buffer_size * 2;

	// create an intermediate buffer for non mmap case for all but NATIVE_FORMAT
	// this is used to pack samples into the output format before calling writei
	if (!alsa.mmap && !alsa.write_buf && alsa.format != NATIVE_FORMAT) {
		alsa.write_buf = malloc(alsa.buffer_size * BYTES_PER_FRAME);
		if (!alsa.write_buf) {
			LOG_ERROR("unable to malloc write_buf");
			return -1;
		}
	}

	// set params
	if ((err = snd_pcm_hw_params(pcmp, hw_params)) < 0) {
		LOG_ERROR("unable to set hw params: %s", snd_strerror(err));
		return err;
	}

	// dump info
	if (loglevel == lSDEBUG) {
		static snd_output_t *debug_output;
		snd_output_stdio_attach(&debug_output, stderr, 0);
		snd_pcm_dump(pcmp, debug_output);
	}

	// this indicates we have opened the device ok
	alsa.rate = sample_rate;

	return 0;
}

static int _write_frames(frames_t out_frames, bool silence, s32_t gainL, s32_t gainR,
						 s32_t cross_gain_in, s32_t cross_gain_out, s32_t **cross_ptr) {

	const snd_pcm_channel_area_t *areas;
	snd_pcm_uframes_t offset;
	void  *outputptr;
	s32_t *inputptr;
	int err;

	if (alsa.mmap) {
		snd_pcm_uframes_t alsa_frames = (snd_pcm_uframes_t)out_frames;
		
		snd_pcm_avail_update(pcmp);
		
		if ((err = snd_pcm_mmap_begin(pcmp, &areas, &offset, &alsa_frames)) < 0) {
			LOG_WARN("error from mmap_begin: %s", snd_strerror(err));
			return -1;
		}
		
		out_frames = (frames_t)alsa_frames;
	}

	if (!silence) {
		// applying cross fade is delayed until this point as mmap_begin can change out_frames
		if (output.fade == FADE_ACTIVE && output.fade_dir == FADE_CROSS && *cross_ptr) {
			_apply_cross(outputbuf, out_frames, cross_gain_in, cross_gain_out, cross_ptr);
		}
	}

	inputptr = (s32_t *) (silence ? silencebuf : outputbuf->readp);

	IF_DSD(
		if (output.dop) {
			if (silence) {
				inputptr = (s32_t *) silencebuf_dop;
			}
			update_dop_marker((u32_t *) inputptr, out_frames);
		}
	)

	if (alsa.mmap || alsa.format != NATIVE_FORMAT) {

		outputptr = alsa.mmap ? (areas[0].addr + (areas[0].first + offset * areas[0].step) / 8) : alsa.write_buf;

		_scale_and_pack_frames(outputptr, inputptr, out_frames, gainL, gainR, output.format);

	} else {

		outputptr = (void *)inputptr;

		if (!silence) {

			if (gainL != FIXED_ONE || gainR!= FIXED_ONE) {
				_apply_gain(outputbuf, out_frames, gainL, gainR);
			}
		}
	}

	if (alsa.mmap) {

		snd_pcm_sframes_t w = snd_pcm_mmap_commit(pcmp, offset, out_frames);
		if (w < 0 || w != out_frames) {
			LOG_WARN("mmap_commit error");
			return -1;
		}

	} else {

		snd_pcm_sframes_t w = snd_pcm_writei(pcmp, outputptr, out_frames);
		if (w < 0) {
			//if (w != -EAGAIN && ((err = snd_pcm_recover(pcmp, w, 1)) < 0)) {
			if (((err = snd_pcm_recover(pcmp, w, 1)) < 0)) {
				static unsigned recover_count = 0;
				LOG_WARN("recover failed: %s [%u]", snd_strerror(err), ++recover_count);
				if (recover_count >= 10) {				
					recover_count = 0;
					alsa_close();
					pcmp = NULL;
				}
			}
			return -1;
		} else {
			if (w != out_frames) {
				LOG_WARN("writei only wrote %u of %u", w, out_frames);
			}						
			out_frames = w;
		}
	}

	return (int)out_frames;
}

static void *output_thread(void *arg) {
	bool start = true;
	bool output_off = false, probe_device = (arg != NULL);
	int err;

	while (running) {

		// disabled output - player is off
		while (output_off) {
			usleep(100000);
			LOCK;
			output_off = (output.state == OUTPUT_OFF);
			UNLOCK;
			if (!running) return 0;
		}

		// wait until device returns - to allow usb audio devices to be turned off
		if (probe_device) {
			while (!pcm_probe(output.device)) {
				LOG_DEBUG("waiting for device %s to return", output.device);
				sleep(5);
			}
			probe_device = false;
		}

		if (!pcmp || alsa.rate != output.current_sample_rate) {
			LOG_INFO("open output device: %s", output.device);
			LOCK;

			// FIXME - some alsa hardware requires opening twice for a new sample rate to work
			// this is a workaround which should be removed
			if (alsa.reopen) {
				alsa_open(output.device, output.current_sample_rate, output.buffer, output.period);
			}

			if (!!alsa_open(output.device, output.current_sample_rate, output.buffer, output.period)) {
				output.error_opening = true;
				UNLOCK;
				sleep(5);
				continue;
			}
			output.error_opening = false;
			start = true;
			UNLOCK;
		}

		snd_pcm_state_t state = snd_pcm_state(pcmp);

		if (state == SND_PCM_STATE_XRUN) {
			LOG_INFO("XRUN");
			if ((err = snd_pcm_recover(pcmp, -EPIPE, 1)) < 0) {
				LOG_INFO("XRUN recover failed: %s", snd_strerror(err));
			}
			start = true;
			continue;
		} else if (state == SND_PCM_STATE_SUSPENDED) {
			if ((err = snd_pcm_recover(pcmp, -ESTRPIPE, 1)) < 0) {
				LOG_INFO("SUSPEND recover failed: %s", snd_strerror(err));
			}
		} else if (state == SND_PCM_STATE_DISCONNECTED) {
			LOG_INFO("Device %s no longer available", output.device);
			alsa_close();
			pcmp = NULL;
			probe_device = true;
			continue;
		}

		snd_pcm_sframes_t avail = snd_pcm_avail_update(pcmp);

		if (avail < 0) {
			if ((err = snd_pcm_recover(pcmp, avail, 1)) < 0) {
				if (err == -ENODEV) {
					LOG_INFO("Device %s no longer available", output.device);
					alsa_close();
					pcmp = NULL;
					probe_device = true;
					continue;
				}
				LOG_WARN("recover failed: %s", snd_strerror(err));
			}
			start = true;
			continue;
		}

		if (avail < alsa.period_size) {
			if (start) {
				if (alsa.mmap && ((err = snd_pcm_start(pcmp)) < 0)) {
					if ((err = snd_pcm_recover(pcmp, err, 1)) < 0) {
						if (err == -ENODEV) {
							LOG_INFO("Device %s no longer available", output.device);
							alsa_close();
							pcmp = NULL;
							probe_device = true;
							continue;
						}
						LOG_INFO("start error: %s", snd_strerror(err));
					}
				} else {
					start = false;
				}
			} else {
				if ((err = snd_pcm_wait(pcmp, 1000)) < 0) {
					if ((err = snd_pcm_recover(pcmp, err, 1)) < 0) {
						LOG_INFO("pcm wait error: %s", snd_strerror(err));
					}
					start = true;
				}
			}
			continue;
		}

		// restrict avail in writei mode as write_buf is restricted to period_size
		if (!alsa.mmap) {
			avail = min(avail, alsa.period_size);
		}

		// avoid spinning in cases where wait returns but no bytes available (seen with pulse audio)
		if (avail == 0) {
			LOG_SDEBUG("avail 0 - sleeping");
			usleep(10000);
			continue;
		}

		LOCK;

		// turn off if requested
		if (output.state == OUTPUT_OFF) {
			UNLOCK;
			LOG_INFO("disabling output");
			alsa_close();
			pcmp = NULL;
			output_off = true;
			vis_stop();
			continue;
		}

		// measure output delay
		snd_pcm_sframes_t delay;
		if ((err = snd_pcm_delay(pcmp, &delay)) < 0) {
			if (err == -EPIPE) {
				// EPIPE indicates underrun - attempt to recover
				UNLOCK;
				continue;
			} else {
				LOG_DEBUG("snd_pcm_delay returns: %d", err);
			}
		} else {
			output.device_frames = delay;
			output.updated = gettime_ms();
		}

		// process frames
		_output_frames(avail);

		UNLOCK;
	}

	return 0;
}

static pthread_t thread;

void output_init_alsa(log_level level, const char *device, unsigned output_buf_size, char *params, unsigned rates[], unsigned rt_priority) {

	unsigned alsa_buffer = ALSA_BUFFER_TIME;
	unsigned alsa_period = ALSA_PERIOD_COUNT;
	char *alsa_sample_fmt = NULL;
	bool alsa_mmap = true;
	bool alsa_reopen = false;

	char *t = next_param(params, ':');
	char *c = next_param(NULL, ':');
	char *s = next_param(NULL, ':');
	char *m = next_param(NULL, ':');
	char *r = next_param(NULL, ':');

	if (t) alsa_buffer = atoi(t);
	if (c) alsa_period = atoi(c);
	if (s) alsa_sample_fmt = s;
	if (m) alsa_mmap = atoi(m);
	if (r) alsa_reopen = atoi(r);

	loglevel = level;

	LOG_INFO("init output");

	memset(&output, 0, sizeof(output));

	alsa.mmap = alsa_mmap;
	alsa.write_buf = NULL;
	alsa.format = 0;
	alsa.reopen = alsa_reopen;
	output.format = 0;
	output.buffer = alsa_buffer;
	output.period = alsa_period;
	output.start_frames = 0;
	output.write_cb = &_write_frames;

	if (alsa_sample_fmt) {
		if (!strcmp(alsa_sample_fmt, "32"))	alsa.format = SND_PCM_FORMAT_S32_LE;
		if (!strcmp(alsa_sample_fmt, "24")) alsa.format = SND_PCM_FORMAT_S24_LE;
		if (!strcmp(alsa_sample_fmt, "24_3")) alsa.format = SND_PCM_FORMAT_S24_3LE;
		if (!strcmp(alsa_sample_fmt, "16")) alsa.format = SND_PCM_FORMAT_S16_LE;
	}

	LOG_INFO("requested alsa_buffer: %u alsa_period: %u format: %s mmap: %u", output.buffer, output.period, 
			 alsa_sample_fmt ? alsa_sample_fmt : "any", alsa.mmap);

	snd_lib_error_set_handler((snd_lib_error_handler_t)alsa_error_handler);

	output_init_common(level, device, output_buf_size, rates);

#if LINUX
	// RT linux - aim to avoid pagefaults by locking memory: 
	// https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example
	if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
		LOG_INFO("unable to lock memory: %s", strerror(errno));
	} else {
		LOG_INFO("memory locked");
	}

   	mallopt(M_TRIM_THRESHOLD, -1);
   	mallopt(M_MMAP_MAX, 0);

	touch_memory(silencebuf, MAX_SILENCE_FRAMES * BYTES_PER_FRAME);
	touch_memory(outputbuf->buf, outputbuf->size);
#endif

	// start output thread
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + OUTPUT_THREAD_STACK_SIZE);
	pthread_create(&thread, &attr, output_thread, rates[0] ? "probe" : NULL);
	pthread_attr_destroy(&attr);

	// try to set this thread to real-time scheduler class, only works as root or if user has permission
	struct sched_param param;
	param.sched_priority = rt_priority;
	if (pthread_setschedparam(thread, SCHED_FIFO, &param) != 0) {
		LOG_DEBUG("unable to set output sched fifo: %s", strerror(errno));
	} else {
		LOG_DEBUG("set output sched fifo rt: %u", param.sched_priority);
	}
}

void output_close_alsa(void) {
	LOG_INFO("close output");

	LOCK;
	running = false;
	UNLOCK;

	pthread_join(thread, NULL);

	if (alsa.write_buf) free(alsa.write_buf);

	output_close_common();
}

#endif // ALSA

