/*
 * xine_frontend_kbd.c: Forward (local) console key presses to VDR (server)
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_frontend_kbd.c,v 1.4 2012-06-06 07:39:56 phintuka Exp $
 *
 */

#include "features.h"

#include <inttypes.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>


#define LOG_MODULENAME "[console]   "
#include "logdefs.h"

#include "xine_frontend.h"
#include "xine_frontend_kbd.h"

/*
 * stdin (keyboard/slave mode) reading
 */

/* static data */
static pthread_t kbd_thread;
static struct termios tm, saved_tm;

/* xine_frontend_main.c: */
extern int gui_hotkeys;


/*
 * read_key()
 *
 * Try to read single char from stdin.
 *
 * Returns: >=0  char readed
 *           -1  nothing to read
 *           -2  fatal error
 */
#define READ_KEY_ERROR   -2
#define READ_KEY_EAGAIN  -1
static int read_key(void)
{
  unsigned char ch;
  int err;
  struct pollfd pfd;
  pfd.fd = STDIN_FILENO;
  pfd.events = POLLIN;

  errno = 0;
  pthread_testcancel();

  if (1 == (err = poll(&pfd, 1, 50))) {
    pthread_testcancel();

    if (1 == (err = read(STDIN_FILENO, &ch, 1)))
      return (int)ch;

    if (err < 0)
      LOGERR("read_key: read(stdin) failed");
    else
      LOGERR("read_key: read(stdin) failed: no stdin");
    return READ_KEY_ERROR;

  } else if (err < 0 && errno != EINTR) {
    LOGERR("read_key: poll(stdin) failed");
    return READ_KEY_ERROR;
  }

  pthread_testcancel();

  return READ_KEY_EAGAIN;
}

/*
 * read_key_seq()
 *
 * Read a key sequence from stdin.
 * Key sequence is either normal key or escape sequence.
 *
 * Returns the key or escape sequence as uint64_t.
 *
 * Originally copied from vdr:
 * remote.c: General Remote Control handling
 * Copyright (C) 2000, 2003, 2006, 2008 Klaus Schmidinger
 */
#define READ_KEY_SEQ_ERROR 0xffffffff
static uint64_t read_key_seq(void)
{
  /* from vdr, remote.c */
  uint64_t k = 0;
  int key1;

  if ((key1 = read_key()) >= 0) {
    k = key1;
    if (key1 == 0x1B) {
      /* Start of escape sequence */
      if ((key1 = read_key()) >= 0) {
        k <<= 8;
        k |= key1 & 0xFF;
        switch (key1) {
          case 0x4F: /* 3-byte sequence */
            if ((key1 = read_key()) >= 0) {
              k <<= 8;
              k |= key1 & 0xFF;
            }
            break;
          case 0x5B: /* 3- or more-byte sequence */
            if ((key1 = read_key()) >= 0) {
              k <<= 8;
              k |= key1 & 0xFF;
              switch (key1) {
                case 0x31 ... 0x3F: /* more-byte sequence */
                case 0x5B: /* strange, may apparently occur */
                  do {
                    if ((key1 = read_key()) < 0)
                      break; /* Sequence ends here */
                    k <<= 8;
                    k |= key1 & 0xFF;
                  } while (key1 != 0x7E);
                  break;
                default:;
              }
            }
            break;
          default:;
        }
      }
    }
  }

  if (key1 == READ_KEY_ERROR)
    return READ_KEY_SEQ_ERROR;

  return k;
}

/*
 * kbd_receiver_thread()
 *
 * Read key(sequence)s from stdin and pass those to frontend.
 */

static void kbd_receiver_thread_cleanup(void *arg)
{
  if (isatty(STDIN_FILENO)) {
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_tm);
  }
  if (isatty(STDOUT_FILENO)) {
    int status;
    status = system("setterm -cursor on");
    if (status < 0)
      LOGMSG("system(\"setterm -cursor on\") failed\n");
  }
  LOGMSG("Keyboard thread terminated");
}

static void *kbd_receiver_thread(void *fe_gen)
{
  frontend_t *fe = (frontend_t*)fe_gen;
  uint64_t code = 0;
  char str[64];

  if (isatty(STDOUT_FILENO)) {
    int status;
    status = system("setterm -cursor off");
    if (status < 0)
      LOGMSG("system(\"setterm -cursor off\") failed\n");
    status = system("setterm -blank off");
    if (status < 0)
      LOGMSG("system(\"setterm -blank off\") failed\n");
  }

  if (isatty(STDIN_FILENO)) {
    /* Set stdin to deliver keypresses without buffering whole lines */
    tcgetattr(STDIN_FILENO, &saved_tm);
    if (tcgetattr(STDIN_FILENO, &tm) == 0) {
      tm.c_iflag = 0;
      tm.c_lflag &= ~(ICANON | ECHO);
      tm.c_cc[VMIN] = 0;
      tm.c_cc[VTIME] = 0;
      tcsetattr(STDIN_FILENO, TCSANOW, &tm);
    }
  }

  pthread_cleanup_push(kbd_receiver_thread_cleanup, NULL);

  do {
    alarm(0);
    errno = 0;
    code = read_key_seq();
    alarm(3); /* watchdog */
    if (code == 0)
      continue;
    if (code == READ_KEY_SEQ_ERROR)
      break;
    if (code == 27) {
      fe->send_event(fe, "QUIT");
      break;
    }

    if (gui_hotkeys) {
      if (code == 'f' || code == 'F') {
        fe->send_event(fe, "TOGGLE_FULLSCREEN");
        continue;
      }
      if (code == 'p' || code == 'P') {
        fe->send_event(fe, "POWER_OFF");
        continue;
      }
      if (code == 'd' || code == 'D') {
        fe->send_event(fe, "TOGGLE_DEINTERLACE");
        continue;
      }
    }

    snprintf(str, sizeof(str), "%016" PRIX64, code);
    fe->send_input_event(fe, "KBD", str, 0, 0);

  } while (fe->xine_is_finished(fe, 0) != FE_XINE_EXIT);

  alarm(0);

  LOGDBG("Keyboard thread terminating");

  pthread_cleanup_pop(1);

  pthread_exit(NULL);
  return NULL; /* never reached */
}

/*
 * slave_receiver_thread()
 *
 * Read slave mode commands from stdin
 * Interpret and execute valid commands
 */

static void slave_receiver_thread_cleanup(void *arg)
{
  /* restore terminal settings */
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_tm);
  LOGDBG("Slave mode receiver terminated");
}

static void *slave_receiver_thread(void *fe_gen)
{
  frontend_t *fe = (frontend_t*)fe_gen;
  char str[128], *pt;

  tcgetattr(STDIN_FILENO, &saved_tm);

  pthread_cleanup_push(slave_receiver_thread_cleanup, NULL);

  do {
    errno = 0;
    str[0] = 0;

    pthread_testcancel();
    if (!fgets(str, sizeof(str), stdin))
      break;
    pthread_testcancel();

    if (NULL != (pt = strchr(str, '\r')))
      *pt = 0;
    if (NULL != (pt = strchr(str, '\n')))
      *pt = 0;

    if (!strncasecmp(str, "QUIT", 4)) {
      fe->send_event(fe, "QUIT");
      break;
    }
    if (!strncasecmp(str, "FULLSCREEN", 10)) {
      if (strpbrk(str + 10, "01"))
        fe->send_event(fe, str);
      else
        fe->send_event(fe, "TOGGLE_FULLSCREEN");
      continue;
    }
    if (!strncasecmp(str, "DEINTERLACE ", 12)) {
      fe->send_event(fe, str);
      continue;
    }
    if (!strncasecmp(str, "HITK ", 5)) {
      fe->send_input_event(fe, NULL, str+5, 0, 0);
      continue;
    }

    LOGMSG("Unknown slave mode command: %s", str);

  } while (fe->xine_is_finished(fe, 0) != FE_XINE_EXIT);

  LOGDBG("Slave mode receiver terminating");

  pthread_cleanup_pop(1);

  pthread_exit(NULL);
  return NULL; /* never reached */
}

/*
 * kbd_start()
 *
 * Start keyboard/slave mode reader thread
 */
void kbd_start(frontend_t *fe, int slave_mode)
{
  int err;
  if ((err = pthread_create (&kbd_thread,
                             NULL,
                             slave_mode ? slave_receiver_thread : kbd_receiver_thread,
                             (void*)fe)) != 0) {
    fprintf(stderr, "Can't create new thread for keyboard (%s)\n",
            strerror(err));
  }
}

/*
 * kbd_stop()
 *
 * Stop keyboard/slave mode reader thread
 */
void kbd_stop(void)
{
  void *p;

  pthread_cancel(kbd_thread);
  pthread_join(kbd_thread, &p);
}


