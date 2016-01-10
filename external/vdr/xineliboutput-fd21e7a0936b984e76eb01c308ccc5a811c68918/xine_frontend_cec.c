/*
 * xine_frontend_cec.c: Forward (local) HDMI-CEC keys to VDR (server)
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_frontend_cec.c,v 1.3 2014-04-29 10:21:18 phintuka Exp $
 *
 */

#include "features.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __FreeBSD__
#include <string.h>
#endif
#include <pthread.h>
#include <unistd.h>

#ifdef HAVE_LIBCEC
#include <libcec/cecc.h>
#endif

#define LOG_MODULENAME "[hdmi-cec]  "
#include "logdefs.h"

#include "xine_frontend.h"
#include "xine_frontend_cec.h"

#ifdef HAVE_LIBCEC

/* static data */
static volatile int exit_req = 0;
static pthread_t cec_thread;
static int cec_hdmi_port = 0;
static int cec_dev_type = 0; /* 0 - TV, 5 - AVR */


static const struct keymap_item {
  const uint8_t map;
  const char    key[12];
} keymap[] = {
  [CEC_USER_CONTROL_CODE_SELECT]                   = {0, "Ok"},
  [CEC_USER_CONTROL_CODE_UP]                       = {0, "Up"},
  [CEC_USER_CONTROL_CODE_DOWN]                     = {0, "Down"},
  [CEC_USER_CONTROL_CODE_LEFT]                     = {0, "Left"},
  [CEC_USER_CONTROL_CODE_RIGHT]                    = {0, "Right"},
  [CEC_USER_CONTROL_CODE_RIGHT_UP]                 = {1, "RIGHT_UP"},
  [CEC_USER_CONTROL_CODE_RIGHT_DOWN]               = {1, "RIGHT_DOWN"},
  [CEC_USER_CONTROL_CODE_LEFT_UP]                  = {1, "LEFT_UP"},
  [CEC_USER_CONTROL_CODE_LEFT_DOWN]                = {1, "LEFT_DOWN"},
  [CEC_USER_CONTROL_CODE_ROOT_MENU]                = {0, "Menu"},
  [CEC_USER_CONTROL_CODE_SETUP_MENU]               = {0, "Menu"},
  [CEC_USER_CONTROL_CODE_CONTENTS_MENU]            = {0, "Recordings"},
  [CEC_USER_CONTROL_CODE_FAVORITE_MENU]            = {1, "FAVORITE"},
  [CEC_USER_CONTROL_CODE_EXIT]                     = {0, "Back"},
  [CEC_USER_CONTROL_CODE_NUMBER0]                  = {0, "0"},
  [CEC_USER_CONTROL_CODE_NUMBER1]                  = {0, "1"},
  [CEC_USER_CONTROL_CODE_NUMBER2]                  = {0, "2"},
  [CEC_USER_CONTROL_CODE_NUMBER3]                  = {0, "3"},
  [CEC_USER_CONTROL_CODE_NUMBER4]                  = {0, "4"},
  [CEC_USER_CONTROL_CODE_NUMBER5]                  = {0, "5"},
  [CEC_USER_CONTROL_CODE_NUMBER6]                  = {0, "6"},
  [CEC_USER_CONTROL_CODE_NUMBER7]                  = {0, "7"},
  [CEC_USER_CONTROL_CODE_NUMBER8]                  = {0, "8"},
  [CEC_USER_CONTROL_CODE_NUMBER9]                  = {0, "9"},
  [CEC_USER_CONTROL_CODE_DOT]                      = {1, "DOT"},
  [CEC_USER_CONTROL_CODE_ENTER]                    = {0, "Ok"},
  [CEC_USER_CONTROL_CODE_CLEAR]                    = {1, "CLEAR"},
  [CEC_USER_CONTROL_CODE_NEXT_FAVORITE]            = {1, "FAVORITE+"},
  [CEC_USER_CONTROL_CODE_CHANNEL_UP]               = {0, "Channel+"},
  [CEC_USER_CONTROL_CODE_CHANNEL_DOWN]             = {0, "Channel-"},
  [CEC_USER_CONTROL_CODE_PREVIOUS_CHANNEL]         = {0, "Previous"},
  [CEC_USER_CONTROL_CODE_SOUND_SELECT]             = {0, "Audio"},
  [CEC_USER_CONTROL_CODE_INPUT_SELECT]             = {1, "INP_SELECT"},
  [CEC_USER_CONTROL_CODE_DISPLAY_INFORMATION]      = {0, "Info"},
  [CEC_USER_CONTROL_CODE_HELP]                     = {1, "HELP"},
  [CEC_USER_CONTROL_CODE_PAGE_UP]                  = {1, "PAGE_UP"},
  [CEC_USER_CONTROL_CODE_PAGE_DOWN]                = {1, "PAGE_DOWN"},
  [CEC_USER_CONTROL_CODE_POWER]                    = {0, "Power"},
  [CEC_USER_CONTROL_CODE_VOLUME_UP]                = {0, "Volume+"},
  [CEC_USER_CONTROL_CODE_VOLUME_DOWN]              = {0, "Volume-"},
  [CEC_USER_CONTROL_CODE_MUTE]                     = {0, "Mute"},
  [CEC_USER_CONTROL_CODE_PLAY]                     = {0, "Play"},
  [CEC_USER_CONTROL_CODE_STOP]                     = {0, "Stop"},
  [CEC_USER_CONTROL_CODE_PAUSE]                    = {0, "Pause"},
  [CEC_USER_CONTROL_CODE_RECORD]                   = {0, "Record"},
  [CEC_USER_CONTROL_CODE_REWIND]                   = {0, "FastRew"},
  [CEC_USER_CONTROL_CODE_FAST_FORWARD]             = {0, "FastFwd"},
  [CEC_USER_CONTROL_CODE_EJECT]                    = {1, "EJECT"},
  [CEC_USER_CONTROL_CODE_FORWARD]                  = {0, "Next"},
  [CEC_USER_CONTROL_CODE_BACKWARD]                 = {0, "Previous"},
  //[CEC_USER_CONTROL_CODE_STOP_RECORD]            = {0, ""}, //0x4D,
  //[CEC_USER_CONTROL_CODE_PAUSE_RECORD]           = {0, ""}, //0x4E,
  [CEC_USER_CONTROL_CODE_ANGLE]                    = {1, "ANGLE"},
  [CEC_USER_CONTROL_CODE_SUB_PICTURE]              = {0, "Subtitles"},
  //[CEC_USER_CONTROL_CODE_VIDEO_ON_DEMAND]        = {0, ""}, //0x52,
  [CEC_USER_CONTROL_CODE_ELECTRONIC_PROGRAM_GUIDE] = {0, "Schedule"},
  [CEC_USER_CONTROL_CODE_TIMER_PROGRAMMING]        = {0, "Timers"},
  //[CEC_USER_CONTROL_CODE_INITIAL_CONFIGURATION]    = {0, ""},
  //[CEC_USER_CONTROL_CODE_PLAY_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_PAUSE_PLAY_FUNCTION]      = {0, ""},
  //[CEC_USER_CONTROL_CODE_RECORD_FUNCTION]          = {0, ""},
  //[CEC_USER_CONTROL_CODE_PAUSE_RECORD_FUNCTION]    = {0, ""},
  //[CEC_USER_CONTROL_CODE_STOP_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_MUTE_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_RESTORE_VOLUME_FUNCTION]  = {0, ""},
  //[CEC_USER_CONTROL_CODE_TUNE_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_SELECT_MEDIA_FUNCTION]       = {0, ""},
  //[CEC_USER_CONTROL_CODE_SELECT_AV_INPUT_FUNCTION]    = {0, ""},
  [CEC_USER_CONTROL_CODE_SELECT_AUDIO_INPUT_FUNCTION] = {0, "Audio"},
  [CEC_USER_CONTROL_CODE_POWER_TOGGLE_FUNCTION]       = {0, "Power"},
  [CEC_USER_CONTROL_CODE_POWER_OFF_FUNCTION]          = {0, "Power"},
  //[CEC_USER_CONTROL_CODE_POWER_ON_FUNCTION]           = {0, ""},
  [CEC_USER_CONTROL_CODE_F1_BLUE]                  = {0, "Blue"},
  [CEC_USER_CONTROL_CODE_F2_RED]                   = {0, "Red"},
  [CEC_USER_CONTROL_CODE_F3_GREEN]                 = {0, "Green"},
  [CEC_USER_CONTROL_CODE_F4_YELLOW]                = {0, "Yellow"},
  [CEC_USER_CONTROL_CODE_F5]                       = {0, "User1"},
  [CEC_USER_CONTROL_CODE_DATA]                     = {1, "DATA"},
  //[CEC_USER_CONTROL_CODE_AN_RETURN]              = {0, ""},
  [CEC_USER_CONTROL_CODE_AN_CHANNELS_LIST]         = {0, "Channels"},
  [0xff] = {0, ""},
};

/*
 * libcec callbacks
 */

static int cec_config_changed_cb(void *this_gen, const libcec_configuration config)
{
  LOGDBG("cec_config_changed");
  return 1;
}

static int cec_menu_state_changed_cb(void *this_gen, const cec_menu_state state)
{
  LOGDBG("cec_menu_state_changed");
  return 1;
}

static void cec_source_activated_cb(void *this_gen, const cec_logical_address address, const uint8_t param)
{
  LOGMSG("cec_source_activated: address %d param %d", address, param);
}

static int cec_log_cb(void *this_gen, const cec_log_message message)
{
  if (message.level <= CEC_LOG_ERROR) {
    errno = 0;
    LOGERR("%s", message.message);
  } else if (message.level <= CEC_LOG_NOTICE) {
    LOGMSG("%s", message.message);
  } else if (message.level <= CEC_LOG_DEBUG) {
    LOGDBG("%s", message.message);
  } else {
    LOGVERBOSE("%s", message.message);
  }
  return 1;
}

static int cec_keypress_cb(void *this_gen, const cec_keypress keypress)
{
  frontend_t *fe = (frontend_t*)this_gen;
  static int last_key = -1;

  LOGVERBOSE("keypress 0x%x duration %d", keypress.keycode, keypress.duration);

  if (keypress.keycode == last_key && keypress.duration > 0)
    return 1;
  else if (keypress.duration > 0)
    last_key = -1;
  else
    last_key = keypress.keycode;

  if (keypress.keycode >= sizeof(keymap) / sizeof(keymap[0]) ||
      !keymap[keypress.keycode].key[0]) {
    LOGMSG("unknown keycode 0x%x", keypress.keycode);
    return 1;
  }

  LOGDBG("sending key %s%s", keymap[keypress.keycode].map ? "CEC." : "", keymap[keypress.keycode].key);

  alarm(3);
  fe->send_input_event(fe, keymap[keypress.keycode].map ? "CEC" : NULL,
		       keymap[keypress.keycode].key, 0, 1);
  alarm(0);

  return 1;
}

static int cec_alert_cb(void *this_gen, const libcec_alert type, const libcec_parameter param)
{
  switch (type) {
    case CEC_ALERT_CONNECTION_LOST:
      LOGMSG("ALERT: CEC connection lost");
      break;
    case CEC_ALERT_PERMISSION_ERROR:
      LOGMSG("ALERT: Permission error");
      break;
    case CEC_ALERT_PORT_BUSY:
      LOGMSG("ALERT: Port busy");
      break;
    case CEC_ALERT_PHYSICAL_ADDRESS_ERROR:
      LOGMSG("ALERT: Physical address error");
      break;
    case CEC_ALERT_TV_POLL_FAILED:
      LOGMSG("ALERT: TV poll failed");
      break;

    case CEC_ALERT_SERVICE_DEVICE:
    default:
      break;
  }
  return 0;
}

static int cec_command_cb(void *this_gen, const cec_command command)
{
  LOGMSG("Received command 0x%x from 0x%x", command.opcode, command.initiator);

  switch (command.opcode) {
    case CEC_OPCODE_STANDBY:
    case CEC_OPCODE_SET_MENU_LANGUAGE:
    case CEC_OPCODE_DECK_CONTROL:
    case CEC_OPCODE_PLAY:
    default:
      break;
  }
  return 1;
}

ICECCallbacks callbacks = {
  .CBCecKeyPress             = cec_keypress_cb,
  .CBCecCommand              = cec_command_cb,
  .CBCecLogMessage           = cec_log_cb,
  .CBCecAlert                = cec_alert_cb,
  .CBCecConfigurationChanged = cec_config_changed_cb,
  .CBCecSourceActivated      = cec_source_activated_cb,
  .CBCecMenuStateChanged     = cec_menu_state_changed_cb,
};

/*
 * configuration
 */

static void libcec_config_clear(libcec_configuration *p)
{
  memset(p, 0, sizeof(*p));

  p->iPhysicalAddress = CEC_PHYSICAL_ADDRESS_TV;
  p->baseDevice = CEC_DEFAULT_BASE_DEVICE;
  p->iHDMIPort = CEC_DEFAULT_HDMI_PORT;
  p->tvVendor = CEC_VENDOR_UNKNOWN;
  p->clientVersion = CEC_CLIENT_VERSION_CURRENT;
  p->serverVersion = CEC_SERVER_VERSION_CURRENT;
  p->bAutodetectAddress = CEC_DEFAULT_SETTING_AUTODETECT_ADDRESS;
  p->bGetSettingsFromROM = CEC_DEFAULT_SETTING_GET_SETTINGS_FROM_ROM;
  p->bUseTVMenuLanguage = CEC_DEFAULT_SETTING_USE_TV_MENU_LANGUAGE;
  p->bActivateSource = CEC_DEFAULT_SETTING_ACTIVATE_SOURCE;
  p->bPowerOffScreensaver = CEC_DEFAULT_SETTING_POWER_OFF_SCREENSAVER;
  p->bPowerOnScreensaver = CEC_DEFAULT_SETTING_POWER_ON_SCREENSAVER;
  p->bPowerOffOnStandby = CEC_DEFAULT_SETTING_POWER_OFF_ON_STANDBY;
  p->bShutdownOnStandby = CEC_DEFAULT_SETTING_SHUTDOWN_ON_STANDBY;
  p->bSendInactiveSource = CEC_DEFAULT_SETTING_SEND_INACTIVE_SOURCE;
  p->iFirmwareVersion = CEC_FW_VERSION_UNKNOWN;
  p->bPowerOffDevicesOnStandby = CEC_DEFAULT_SETTING_POWER_OFF_DEVICES_STANDBY;
  memcpy(p->strDeviceLanguage, CEC_DEFAULT_DEVICE_LANGUAGE, 3);
  p->iFirmwareBuildDate = CEC_FW_BUILD_UNKNOWN;
  p->bMonitorOnly = 0;
  p->cecVersion = CEC_DEFAULT_SETTING_CEC_VERSION;
  p->adapterType = ADAPTERTYPE_UNKNOWN;
  p->iDoubleTapTimeoutMs = CEC_DOUBLE_TAP_TIMEOUT_MS;
  p->comboKey = CEC_USER_CONTROL_CODE_STOP;
  p->iComboKeyTimeoutMs = CEC_DEFAULT_COMBO_TIMEOUT_MS;

  memset(p->strDeviceName, 0, sizeof(p->strDeviceName));

  //deviceTypes.Clear();
  int i;
  for (i = 0; i < sizeof(p->deviceTypes.types) / sizeof(p->deviceTypes.types[0]); i++)
    p->deviceTypes.types[i] = CEC_DEVICE_TYPE_RESERVED;
  //logicalAddresses.Clear();
  p->logicalAddresses.primary = CECDEVICE_UNREGISTERED;
  memset(p->logicalAddresses.addresses, 0, sizeof(p->logicalAddresses.addresses));
  //wakeDevices.Clear();
  p->wakeDevices.primary = CECDEVICE_UNREGISTERED;
  memset(p->wakeDevices.addresses, 0, sizeof(p->wakeDevices.addresses));
  //powerOffDevices.Clear();
  p->powerOffDevices.primary = CECDEVICE_UNREGISTERED;
  memset(p->powerOffDevices.addresses, 0, sizeof(p->powerOffDevices.addresses));

  #if CEC_DEFAULT_SETTING_POWER_OFF_SHUTDOWN == 1
  p->powerOffDevices.primary = CECDEVICE_BROADCAST;
  #endif
  #if CEC_DEFAULT_SETTING_ACTIVATE_SOURCE == 1
  p->wakeDevices.primary = CECDEVICE_TV;
  #endif

  p->callbackParam = NULL;
  p->callbacks = NULL;
}

static int cec_parse_edid(uint8_t *edid, int size)
{
  /* get cec physical address from edid vendor-specific block */
  int i;
  for (i = 0; i < size; i++) {
    if (edid[i] == 0x03 && edid[i + 1] == 0x0c && edid[i + 2] == 0x00) {
      /* hdmi marker found */
      LOGMSG("Got CEC physical address from edid: %d.%d.%d.%d",
             edid[i + 3] >> 4 & 0xf,
             edid[i + 3]      & 0xf,
             edid[i + 4] >> 4 & 0xf,
             edid[i + 4]      & 0xf);

      return (edid[i + 3] << 8) | edid[i + 4];
    }
  }
  return -1;
}

static int detect_hdmi_address(frontend_t *fe_gen)
{
  if (cec_hdmi_port <= 0) {
    frontend_t *fe = (frontend_t*)fe_gen;
    if (fe->fe_display_edid) {
      int cec_hdmi_address;
      int size = 0;
      uint8_t *edid;
      edid = fe->fe_display_edid(fe, &size);
      if (edid) {
        cec_hdmi_address = cec_parse_edid(edid, size);
        free(edid);

        if (cec_hdmi_address > 0) {
          return cec_hdmi_address;
        }
      }
    }
    LOGMSG("WARNING: CEC HDMI port not given and edid reading/parsing failed");
  }
  return 0;
}

static int libcec_init(void *fe_gen)
{
  libcec_configuration config;

  libcec_config_clear(&config);

  config.clientVersion = CEC_CLIENT_VERSION_CURRENT;
  strncpy(config.strDeviceName, "VDR", sizeof(config.strDeviceName));

  config.iPhysicalAddress = detect_hdmi_address(fe_gen);
  config.iHDMIPort = cec_hdmi_port;
  config.baseDevice = cec_dev_type;

  config.bActivateSource = 0;
  config.callbackParam = fe_gen;
  config.callbacks = &callbacks;

  config.deviceTypes.types[0] = CEC_DEVICE_TYPE_PLAYBACK_DEVICE;
  config.deviceTypes.types[1] = CEC_DEVICE_TYPE_RECORDING_DEVICE;
  config.deviceTypes.types[2] = CEC_DEVICE_TYPE_TUNER;
  //config.deviceTypes.types[3] = CEC_DEVICE_TYPE_AUDIO_SYSTEM;

  if (!cec_initialise(&config)) {
    LOGMSG("cec_initialize() failed");
    return 0;
  }

  cec_init_video_standalone();

  return 1;
}

/*
 *
 */

static int libcec_open(void)
{
  cec_adapter devices[10];
  int count = cec_find_adapters(devices, 10, NULL);
  if (count < 1) {
    LOGMSG("No HDMI-CEC adapters found");
    return 0;
  }

  LOGMSG("%d adapters found. Opening %s", count, devices[0].comm);

  if (!cec_open(devices[0].comm, 3000)) {
    LOGMSG("error opening CEC adapter");
    return 0;
  }

  LOGMSG("opened adapter %s", devices[0].comm);

  return 1;
}

static int libcec_check_device(void)
{
  if (!cec_ping_adapters()) {
    LOGMSG("cec_ping_adapters() failed");
    return 0;
  }

  return 1;
}

static void cleanup(void *p)
{
  cec_close();
  cec_destroy();
}

static void *cec_receiver_thread(void *fe_gen)
{

  LOGDBG("started");

  pthread_cleanup_push(cleanup, NULL);

  enum { INIT, WAIT_DEVICE, RUNNING } state = INIT;

  while (!exit_req) {

    pthread_testcancel();

    switch (state) {
    case INIT:
      if (!libcec_init(fe_gen)) {
	return NULL;
      }
      state = WAIT_DEVICE;
      break;
    case WAIT_DEVICE:
      if (libcec_open()) {
	state = RUNNING;
      }
      usleep(5000*1000);
      break;
    case RUNNING:
      if (!libcec_check_device()) {
        state = WAIT_DEVICE;
      }
      usleep(1000*1000);
      break;
    }
  }

  pthread_cleanup_pop(1);

  pthread_exit(NULL);
  return NULL; /* never reached */
}

#endif /* HAVE_LIBCEC */

/*
 * interface
 */

void cec_start(struct frontend_s *fe, int hdmi_port, int dev_type)
{
#ifdef HAVE_LIBCEC
  if (hdmi_port >= 0) {
    int err;

    exit_req = 0;
    cec_hdmi_port = hdmi_port;
    cec_dev_type = dev_type;

    if ((err = pthread_create (&cec_thread,
                               NULL, cec_receiver_thread,
                               (void*)fe)) != 0) {
      fprintf(stderr, "can't create new thread for HDMI-CEC (%s)\n",
              strerror(err));
    }
  }
#endif /* HAVE_LIBCEC */
}

void cec_stop(void)
{
#ifdef HAVE_LIBCEC
  if (!exit_req) {
    void *p;
    exit_req = 1;
    pthread_cancel (cec_thread);
    pthread_join (cec_thread, &p);
  }
#endif /* HAVE_LIBCEC */
}
