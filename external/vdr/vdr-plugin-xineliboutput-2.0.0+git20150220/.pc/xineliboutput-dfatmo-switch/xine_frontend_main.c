/*
 * Simple main() routine for stand-alone frontends.
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_frontend_main.c,v 1.105 2014-01-14 08:21:33 phintuka Exp $
 *
 */

#include "features.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <getopt.h>
#include <signal.h>

#include <xine.h>  /* xine_get_version */

#define LOG_MODULENAME "[vdr-fe]    "
#include "logdefs.h"

#include "xine_input_vdr_mrl.h"
#include "xine_frontend.h"
#include "tools/vdrdiscovery.h"
#include "xine_frontend_cec.h"
#include "xine_frontend_lirc.h"
#include "xine_frontend_kbd.h"

/* static data */

/* next symbol is dynamically linked from input plugin */
int SysLogLevel __attribute__((visibility("default"))) = SYSLOGLEVEL_INFO; /* errors and info, no debug */

volatile int   last_signal = 0;
int            gui_hotkeys = 0;

/*
 * SignalHandler()
 */
static void SignalHandler(int signum)
{
  LOGMSG("caught signal %d", signum);

  switch (signum) {
    case SIGHUP:
      last_signal = signum;
    case SIGPIPE:
      break;
    default:
      if (last_signal) {
        LOGMSG("SignalHandler: exit(-1)");
        exit(-1);
      }
      last_signal = signum;
      break;
  }

  signal(signum, SignalHandler);
}

/*
 * strcatrealloc()
 */
static char *strcatrealloc(char *dest, const char *src)
{
  size_t l;

  if (!src || !*src)
    return dest;

  l = (dest ? strlen(dest) : 0) + strlen(src) + 1;
  if(dest) {
    dest = (char *)realloc(dest, l);
    strcat(dest, src);
  } else {
    dest = (char*)malloc(l);
    strcpy(dest, src);
  }
  return dest;
}

/*
 * static data
 */

static const char help_str[] =
"When server address is not given, server is searched from local network.\n"
"If server is not found, localhost (127.0.0.1) is used as default.\n\n"
    "   -H, --help                    Show (this) help message\n"
    "   -A, --audio=audiodriver[:device]\n"
    "                                 Select audio driver and optional port\n"
    "   -V, --video=videodriver[:device]\n"
    "                                 Select video driver and optional port\n"
#ifndef IS_FBFE
    "   -d, --display=displayaddress  X11 display address\n"
    "   -W, --wid=id                  Use existing X11 window\n"
    "                                 Special ID for root window: --wid=root\n"
# ifdef HAVE_XRANDR
    "   -m, --modeswitch              Enable video mode switching\n"
# endif
#endif
    "   -a, --aspect=[auto|4:3|16:9|16:10|default]\n"
    "                                 Display aspect ratio\n"
    "                                 Use script to control HW aspect ratio:\n"
    "                                 --aspect=auto:path_to_script\n"
#ifndef IS_FBFE
    "   -f, --fullscreen              Fullscreen mode\n"
# if defined(HAVE_XRENDER) || defined(HAVE_OPENGL)
    "   -D, --hud[=flag[,flag]]       Head Up Display OSD\n"
    "                                 Optional flags:\n"
# endif
# ifdef HAVE_XRENDER
#   ifdef HAVE_XCOMPOSITE
    "                                 xrender Use XRender instead of compositing\n"
    "                                         (no compositing manager required)\n"
#   endif
#   ifdef HAVE_XSHAPE
    "                                 xshape  Use XShape instead of compositing\n"
    "                                         (no compositing manager required)\n"
#   endif
# endif // HAVE_XRENDER
# ifdef HAVE_OPENGL
    "                                 opengl  Use OpenGL instead of compositing\n"
    "                                         (no compositing manager required)\n"
# endif
# ifdef HAVE_OPENGL
    "   -O, --opengl                  Use OpenGL for video and Head Up Display OSD\n"
# endif
    "   -w, --width=x                 Video window width\n"
    "   -h, --height=x                Video window height\n"
    "   -g, --geometry=WxH[+X+Y]      Set output window geometry (X style)\n"
#endif // !IS_FBFE
    "   -B, --buffers=x               Number of PES buffers\n"
    "   -n, --noscaling               Disable all video scaling\n"
    "   -P, --post=name[:arg=val[,arg=val]]\n"
    "                                 Load and use xine post plugin(s)\n"
    "                                 Examples:\n"
    "                                 --post=upmix\n"
    "                                 --post=upmix;tvtime:enabled=1,cheap_mode=1\n"
    "   -L, --lirc[=devicename]       Use lirc input device\n"
    "                                 Optional lirc socket name can be given\n"
#ifdef HAVE_LIBCEC
    "   -E, --nocec                   Disable HDMI-CEC input device\n"
    "   -e, --cec[=port[,type]]       Use HDMI-CEC input device\n"
    "                                 port: HDMI port number\n"
    "                                 type: 0 for TV, 5 for AVR\n"
#endif
    "   -C, --config=file             Use config file (default: ~/.xine/config_xineliboutput).\n"
    "   -v, --verbose                 Verbose debug output\n"
    "   -s, --silent                  Silent mode (report only errors)\n"
    "   -l, --syslog                  Write all output to system log\n"
    "   -k, --nokbd                   Disable console keyboard input\n"
#ifndef IS_FBFE
    "   -x, --noxkbd                  Disable X11 keyboard input\n"
#endif
    "   -o, --hotkeys                 Enable frontend GUI hotkeys\n"
    "   -U, --touch                   Enable touch screen remote controller\n"
    "   -p, --shutdown=MIN[:CMD]      Shutdown after MIN minutes of inactivity\n"
    "                                 Use CMD to perform shutdown (default: /sbin/shutdown)\n"
    "   -T, --terminal=dev            Controlling TTY\n"
    "   -b, --daemon                  Run as daemon (disable keyboard,\n"
    "                                 log to syslog and fork to background)\n"
    "   -S, --slave                   Enable slave mode (read commands from stdin)\n"
    "   -R, --reconnect               Automatically reconnect when connection has been lost\n"
    "   -t, --tcp                     Use TCP transport\n"
    "   -u, --udp                     Use UDP transport\n"
    "   -r, --rtp                     Use RTP transport\n\n"
    "                                 If no transport options are given, transports\n"
    "                                 are tried in following order:\n"
    "                                 local pipe, rtp, udp, tcp\n\n";

static const char short_options[] = "HA:V:d:W:a:fg:Dw:h:B:nP:L:C:T:p:vsxlkoOeEbSRtuUr";

static const struct option long_options[] = {
  { "help",       no_argument,       NULL, 'H' },
  { "audio",      required_argument, NULL, 'A' },
  { "video",      required_argument, NULL, 'V' },
  { "aspect",     required_argument, NULL, 'a' },
#ifndef IS_FBFE
  { "display",    required_argument, NULL, 'd' },
  { "wid",        required_argument, NULL, 'W' },
# ifdef HAVE_XRANDR
  { "modeswitch", no_argument,       NULL, 'm' },
# endif
  { "fullscreen", no_argument,       NULL, 'f' },
  { "geometry",   required_argument, NULL, 'g' },
# ifdef HAVE_XRENDER
  { "hud",        optional_argument, NULL, 'D' },
#  ifdef HAVE_OPENGL
  { "opengl",     no_argument,       NULL, 'O' },
#  endif
# endif
  { "width",      required_argument, NULL, 'w' },
  { "height",     required_argument, NULL, 'h' },
#endif
  { "buffers",    required_argument, NULL, 'B' },
  { "noscaling",  no_argument,       NULL, 'n' },
  { "post",       required_argument, NULL, 'P' },
  { "lirc",       optional_argument, NULL, 'L' },
#ifdef HAVE_LIBCEC
  { "nocec",      optional_argument, NULL, 'E' },
  { "cec",        optional_argument, NULL, 'e' },
#endif
  { "config",     required_argument, NULL, 'C' },
  { "terminal",   required_argument, NULL, 'T' },
  { "shutdown",   required_argument, NULL, 'p' },

  { "verbose", no_argument,  NULL, 'v' },
  { "silent",  no_argument,  NULL, 's' },
  { "syslog",  no_argument,  NULL, 'l' },
  { "nokbd",   no_argument,  NULL, 'k' },
  { "noxkbd",  no_argument,  NULL, 'x' },
  { "hotkeys", no_argument,  NULL, 'o' },
  { "touch",   no_argument,  NULL, 'U' },
  { "daemon",  no_argument,  NULL, 'b' },
  { "slave",   no_argument,  NULL, 'S' },

  { "reconnect", no_argument,  NULL, 'R' },
  { "tcp",       no_argument,  NULL, 't' },
  { "udp",       no_argument,  NULL, 'u' },
  { "rtp",       no_argument,  NULL, 'r' },
  { NULL,        no_argument,  NULL,  0  }
};

#define PRINTF(x...) do { if(SysLogLevel>1) printf(x); } while(0)
#define EXIT(x...)   do { fprintf(stderr, x); exit(-1); } while(0)

int main(int argc, char *argv[])
{
  int ftcp = 0, fudp = 0, frtp = 0, reconnect = 0, firsttry = 1;
  int fullscreen = 0, hud = 0, opengl = 0, xpos = 0, ypos = 0, width = 720, height = 576;
  int pes_buffers = 250;
  int scale_video = 1, aspect = 1, modeswitch = 0;
  int daemon_mode = 0, nokbd = 0, noxkbd = 0, slave_mode = 0;
  int repeat_emu = 0;
  int touchscreen = 0;
  int window_id = WINDOW_ID_NONE;
  int xmajor, xminor, xsub;
  int c;
  int xine_finished = FE_XINE_ERROR;
  int inactivity_timer = 0;
  char *mrl = NULL;
  char *video_driver = NULL;
  char *audio_driver = NULL;
  char *static_post_plugins = NULL;
  char *lirc_dev = NULL;
  int   cec_hdmi_port = 0, cec_dev_type = 0;
  char *p;
  const char *audio_device = NULL;
  const char *video_port = NULL;
  const char *aspect_controller = NULL;
  const char *tty = NULL;
  const char *exec_name = argv[0];
  const char *config_file = NULL;
  const char *power_off_cmd = NULL;

  extern const fe_creator_f fe_creator;
  frontend_t *fe = NULL;

  LogToSysLog = 0;

  if (strrchr(argv[0],'/'))
    exec_name = strrchr(argv[0],'/')+1;

  xine_get_version(&xmajor, &xminor, &xsub);
  printf("%s %s  (build with xine-lib %d.%d.%d, using xine-lib %d.%d.%d)\n\n",
         exec_name,
         FE_VERSION_STR,
         XINE_MAJOR_VERSION, XINE_MINOR_VERSION, XINE_SUB_VERSION,
         xmajor, xminor, xsub);

  /* Parse arguments */
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (c) {
    default:
    case 'H': printf("\nUsage: %s [options] [" MRL_ID "[+udp|+tcp|+rtp]:[//host[:port]]] \n"
                     "\nAvailable options:\n", exec_name);
              printf("%s", help_str);
              list_xine_plugins(NULL, SysLogLevel>2);
              exit(0);
    case 'A': audio_driver = strdup(optarg);
              if (NULL != (p = strchr(audio_driver, ':'))) {
                *p = 0;
                audio_device = p + 1;
              }
              PRINTF("Audio driver: %s\n", audio_driver);
              if (audio_device)
                PRINTF("Audio device: %s\n", audio_device);
              break;
    case 'V': video_driver = strdup(optarg);
              if (NULL != (p = strchr(video_driver, ':'))) {
                *p = 0;
                video_port = p + 1;
              }
              PRINTF("Video driver: %s\n", video_driver);
              if (video_port)
                PRINTF("Video port: %s\n", video_port);
              break;
#ifndef IS_FBFE
    case 'W': if (!strcmp(optarg, "root"))
                window_id = WINDOW_ID_ROOT;
              else
                window_id = atoi(optarg);
              break;
    case 'm': modeswitch = 1;
#ifdef HAVE_XRANDR
              PRINTF("Video mode switching enabled\n");
#else
              PRINTF("Video mode switching not supported\n");
#endif
              break;
    case 'd': video_port = optarg;
              break;
    case 'x': noxkbd = 1;
              break;
#endif
    case 'a': if (!strncmp(optarg, "auto", 4))
                aspect = 0;
              if (!strncmp(optarg, "4:3", 3))
                aspect = 2;
              if (!strncmp(optarg, "16:9", 4))
                aspect = 3;
              if (!strncmp(optarg, "16:10", 5))
                aspect = 4;
              if (aspect == 0 && optarg[4] == ':')
                aspect_controller = optarg + 5;
              PRINTF("Aspect ratio: %s\n",
                     aspect==0?"Auto":aspect==2?"4:3":aspect==3?"16:9":
                     aspect==4?"16:10":"Default");
              if (aspect_controller)
                PRINTF("Using %s to switch aspect ratio\n", aspect_controller);
              break;
#ifndef IS_FBFE
    case 'f': fullscreen=1;
              PRINTF("Fullscreen mode\n");
              break;
    case 'D':
# ifdef HAVE_XRENDER
              hud = HUD_COMPOSITE;
              PRINTF("HUD OSD mode\n");
# else
              EXIT("HUD OSD not supported\n");
# endif
              if (optarg && strstr(optarg, "opengl")) {
# ifdef HAVE_OPENGL
                hud |= HUD_OPENGL;
                PRINTF("OpenGL HUD OSD mode\n");
                break;
# else
                EXIT("OpenGL HUD OSD not supported\n");
# endif
              }
              if (optarg && strstr(optarg, "xrender")) {
# ifdef HAVE_XCOMPOSITE
                hud |= HUD_XRENDER;
                PRINTF("XRender HUD OSD mode\n");
                break;
# else
                EXIT("XRender HUD OSD not supported\n");
# endif
              }
              if (optarg && strstr(optarg, "xshape")) {
# ifdef HAVE_XSHAPE
                hud |= HUD_XSHAPE;
                PRINTF("XShape HUD OSD mode\n");
# else
                EXIT("XShape HUD OSD not supported\n");
# endif
              }
              break;
    case 'O':
# ifdef HAVE_OPENGL
              opengl = 1;
              PRINTF("Using OpenGL to draw video and OSD\n");
# else
              EXIT("OpenGL not supported\n");
# endif
              break;
    case 'w': width = atoi(optarg);
              PRINTF("Width: %d\n", width);
              break;
    case 'g': sscanf (optarg, "%dx%d+%d+%d", &width, &height, &xpos, &ypos);
              PRINTF("Geometry: %dx%d+%d+%d\n", width, height, xpos, ypos);
              break;
    case 'h': height = atoi(optarg);
              PRINTF("Height: %d\n", height);
              break;
#endif // !IS_FBFE
    case 'B': pes_buffers = atoi(optarg);
              PRINTF("Buffers: %d\n", pes_buffers);
              break;
    case 'T': tty = optarg;
              if (access(tty, R_OK | W_OK) < 0) {
                EXIT("Can't access terminal: %s\n", tty);
              }
              PRINTF("Terminal: %s\n", tty);
              break;
    case 'n': scale_video = 0;
              PRINTF("Video scaling disabled\n");
              break;
    case 'p': inactivity_timer = atoi(optarg);
              power_off_cmd = strchr(optarg, ':');
              power_off_cmd = power_off_cmd ? power_off_cmd+1 : "/sbin/shutdown";
              PRINTF("Shutdown after %d minutes of inactivity using %s\n", inactivity_timer, power_off_cmd);
              break;
    case 'P': if (static_post_plugins)
                strcatrealloc(static_post_plugins, ";");
              static_post_plugins = strcatrealloc(static_post_plugins, optarg);
              PRINTF("Post plugins: %s\n", static_post_plugins);
              break;
    case 'C': config_file = optarg;
              PRINTF("Config file: %s\n", config_file);
              break;
    case 'L': lirc_dev = strdup(optarg ? : "/dev/lircd");
              if (strstr(lirc_dev, ",repeatemu")) {
                *strstr(lirc_dev, ",repeatemu") = 0;
                repeat_emu = 1;
              }
              PRINTF("LIRC device:  %s%s\n", lirc_dev,
                     repeat_emu?", emulating key repeat":"");
              break;
    case 'E': cec_hdmi_port = -1;
              break;
    case 'e': cec_hdmi_port = 0;
#ifdef HAVE_LIBCEC
              if (optarg)
                sscanf(optarg, "%d,%d", &cec_hdmi_port, &cec_dev_type);
              PRINTF("HDMI-CEC enabled. Connected to HDMI port %d (type %d)\n", cec_hdmi_port, cec_dev_type);
#else
              EXIT("HDMI-CEC support not compiled in\n");
#endif
              break;
    case 'v': SysLogLevel = (SysLogLevel<SYSLOGLEVEL_DEBUG) ? SYSLOGLEVEL_DEBUG : SysLogLevel+1;
              PRINTF("Verbose mode\n");
              break;
    case 's': SysLogLevel = 1;
              PRINTF("Silent mode\n");
              break;
    case 'S': slave_mode = 1;
              PRINTF("Slave mode\n");
              break;
    case 'l': LogToSysLog = 1;
              openlog(exec_name, LOG_PID|LOG_CONS, LOG_USER);
              break;
    case 'k': nokbd = 1;
              PRINTF("Keyboard input disabled\n");
              break;
    case 'o': gui_hotkeys = 1;
              PRINTF("GUI hotkeys enabled\n"
                     "  mapping keyboard f,F     -> fullscreen toggle\n"
                     "          keyboard d,D     -> deinterlace toggle\n"
                     "          keyboard p,P     -> power off\n"
                     "          LIRC Deinterlace -> deinterlace toggle\n"
                     "          LIRC Fullscreen  -> fullscreen toggle\n"
                     "          LIRC PowerOff    -> power off\n"
                     "          LIRC Quit        -> exit\n");
              break;
    case 'U': touchscreen = 1;
              PRINTF("Touchscreen input enabled\n");
              PRINTF("Display is divided to 4x3 buttons:\n");
              PRINTF("  Menu   Up     Back   Ok  \n");
              PRINTF("  Left   Down   Right      \n");
              PRINTF("  Red    Green  Yellow Blue\n");
              break;
    case 'b': nokbd = daemon_mode = 1;
              PRINTF("Keyboard input disabled\n");
              break;
    case 'R': reconnect = 1;
              PRINTF("Automatic reconnection enabled\n");
              break;
    case 't': ftcp = 1;
              PRINTF("Protocol: TCP\n");
              break;
    case 'u': fudp = 1;
              PRINTF("Protocol: UDP\n");
              break;
    case 'r': frtp = 1;
              PRINTF("Protocol: RTP\n");
              break;
    case 1:   printf("arg 1 (%s)\n", long_options[optind].name); exit(0);
    }
  }

  if (optind < argc) {
    mrl = strdup(argv[optind]);
    PRINTF("VDR Server: %s\n", mrl);
    while (++optind < argc)
      printf("Unknown argument: %s\n", argv[optind]);
  }

  PRINTF("\n");

  if (tty) {
    /* claim new controlling terminal */
    stdin  = freopen(tty, "r", stdin);
    stdout = freopen(tty, "w", stdout);
    stderr = freopen(tty, "w", stderr);
  }

#if 1
  /* backward compability */
  if (mrl && ( !strncmp(mrl, MRL_ID ":tcp:",  MRL_ID_LEN+5) ||
               !strncmp(mrl, MRL_ID ":udp:",  MRL_ID_LEN+5) ||
               !strncmp(mrl, MRL_ID ":rtp:",  MRL_ID_LEN+5) ||
               !strncmp(mrl, MRL_ID ":pipe:", MRL_ID_LEN+6)))
    mrl[4] = '+';
#endif

  /* If server address not given, try to find server automatically */
  if (!mrl ||
      !strcmp(mrl, MRL_ID ":") ||
      !strcmp(mrl, MRL_ID "+tcp:") ||
      !strcmp(mrl, MRL_ID "+udp:") ||
      !strcmp(mrl, MRL_ID "+rtp:") ||
      !strcmp(mrl, MRL_ID "+pipe:")) {
    char address[1024] = "";
    int port = -1;
    PRINTF("VDR server not given, searching ...\n");
    if (udp_discovery_find_server(&port, &address[0])) {
      PRINTF("Found VDR server: host %s, port %d\n", address, port);
      if (mrl) {
        char *tmp = mrl;
        mrl = NULL;
        if (asprintf(&mrl, "%s//%s:%d", tmp, address, port) < 0)
          return -1;
        free(tmp);
      } else
        if (asprintf(&mrl, MRL_ID "://%s:%d", address, port) < 0)
          return -1;
    } else {
      PRINTF("---------------------------------------------------------------\n"
             "WARNING: MRL not given and server not found from local network.\n"
             "         Trying to connect to default port on local host.\n"
             "---------------------------------------------------------------\n");
      mrl = strdup(MRL_ID "://127.0.0.1");
    }
  }

  if (mrl &&
      strncmp(mrl, MRL_ID ":", MRL_ID_LEN+1) &&
      strncmp(mrl, MRL_ID "+", MRL_ID_LEN+1)) {
    char *mrl2 = mrl;
    PRINTF("WARNING: MRL does not start with \'" MRL_ID ":\' (%s)\n", mrl);
    if (asprintf(&mrl, MRL_ID "://%s", mrl) < 0)
      return -1;
    free(mrl2);
  }

  {
    char *tmp = NULL, *mrl2 = mrl;
    if (frtp && !strstr(mrl, "rtp:"))
      tmp = strdup(MRL_ID "+rtp:");
    else if (fudp && !strstr(mrl, "udp:"))
      tmp = strdup(MRL_ID "+udp:");
    else if (ftcp && !strstr(mrl, "tcp:"))
      tmp = strdup(MRL_ID "+tcp:");
    if (tmp) {
      mrl = strcatrealloc(tmp, strchr(mrl, '/'));
      free(mrl2);
    }
  }

  if (daemon_mode) {
    PRINTF("Entering daemon mode\n\n");
    if (daemon(1, 0) == -1) {
      fprintf(stderr, "%s: %m\n", exec_name);
      LOGERR("daemon() failed");
      return -2;
    }
  }

  /* Create front-end */
  fe = (*fe_creator)();
  if (!fe) {
    fprintf(stderr, "Error initializing frontend\n");
    return -3;
  }

  /* Initialize display */
  if (!fe->fe_display_open(fe, xpos, ypos, width, height, fullscreen, hud, opengl, modeswitch,
                           "", aspect, noxkbd, gui_hotkeys, touchscreen,
                           video_port, scale_video,
                           aspect_controller, window_id)) {
    fprintf(stderr, "Error opening display\n");
    fe->fe_free(fe);
    return -4;
  }

  /* Initialize xine */
  if (!fe->xine_init(fe, audio_driver, audio_device, video_driver,
                     pes_buffers, static_post_plugins, config_file)) {
    fprintf(stderr, "Error initializing xine\n");
    list_xine_plugins(fe, SysLogLevel>2);
    fe->fe_free(fe);
    return -5;
  }
  if (power_off_cmd) {
    fe->shutdown_init(fe, power_off_cmd, inactivity_timer * 60);
  }

  if (SysLogLevel > 2)
    list_xine_plugins(fe, SysLogLevel>2);

  /* signal handlers */

  if (signal(SIGHUP,  SignalHandler) == SIG_IGN) signal(SIGHUP,  SIG_IGN);
  if (signal(SIGINT,  SignalHandler) == SIG_IGN) signal(SIGINT,  SIG_IGN);
  if (signal(SIGTERM, SignalHandler) == SIG_IGN) signal(SIGTERM, SIG_IGN);
  if (signal(SIGPIPE, SignalHandler) == SIG_IGN) signal(SIGPIPE, SIG_IGN);

  do {

    if (!firsttry) {
      PRINTF("Connection to server lost. Reconnecting after two seconds...\n");
      sleep(2);
      PRINTF("Reconnecting...\n");
    }

    /* Connect to VDR xineliboutput server */
    if (!fe->xine_open(fe, mrl)) {
      /*print_xine_log(((fe_t *)fe)->xine);*/
      if (!firsttry) {
        PRINTF("Error opening %s\n", mrl);
        continue;
      }
      fprintf(stderr, "Error opening %s\n", mrl);
      fe->fe_free(fe);
      return -6;
    }

    if (!fe->xine_play(fe)) {
      if (!firsttry) {
        PRINTF("Error playing %s\n", argv[1]);
        continue;
      }
      fprintf(stderr, "Error playing %s\n", argv[1]);
      fe->fe_free(fe);
      return -7;
    }

    if (firsttry) {

      /* Start LIRC forwarding */
      lirc_start(fe, lirc_dev, repeat_emu);

      cec_start(fe, cec_hdmi_port, cec_dev_type);

      /* Start keyboard listener thread */
      if (!nokbd) {
        PRINTF("\n\nPress Esc to exit\n\n");
        kbd_start(fe, slave_mode);
      }
    }

    /* Main loop */

    fflush(stdout);
    fflush(stderr);

    while (!last_signal && fe->fe_run(fe))
      ;
    xine_finished = fe->xine_is_finished(fe,0);

    fe->xine_close(fe);
    firsttry = 0;

    /* HUP reconnects */
    if (last_signal == SIGHUP)
      last_signal = 0;

  } while (!last_signal && xine_finished != FE_XINE_EXIT && reconnect);

  /* Clean up */

  PRINTF("Terminating...\n");

  fe->send_event(fe, "QUIT");

  /* stop input threads */
  lirc_stop();
  cec_stop();
  if (!nokbd)
    kbd_stop();

  fe->fe_free(fe);

  free(static_post_plugins);
  free(mrl);
  free(audio_driver);
  free(video_driver);
  free(lirc_dev);

  return xine_finished==FE_XINE_EXIT ? 0 : 1;
}
