/**
 * Software to control Sky box via Dusky Control 'sky controller'
 *
 * Copyright (C) Joseph Heenan, 2005-2008. All Rights Reserved.
 * Copyright (C) Neale Swinnerton, 2005. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR `AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: sky-control.c,v 1.16 2010-12-12 14:18:06 joseph Exp $
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE
#ifdef WIN32
#include "getopt.h"
#include <windows.h>
#else /* WIN32 */
#include <getopt.h>
#include <unistd.h>
#endif /* WIN32 */
#include <time.h>

#include "sky-control-lib.h"
#include "sky-serial.h"
#include "gnome.h"
#include "serial.h"

#define USB_TIMEOUT 1000 /* milliseconds */

/* how long we wait between sending a channel change and expecting a response
 * from the sky boxes RS232 port.
 *
 * 5 seconds isn't long enough for a sky+ box that is in standby to respond
 */
#define FEEDBACK_TIMEOUT 10 /* seconds */

/**
 * delay before sending backup key to dismiss search and scan banner after
 * changing channel
 */
#define PRE_BACKUP_WAIT 2000 /* milliseconds */

/* see http://www.heenan.me.uk/control-sky-from-pc/sky-control-codes.html
 * for a full list of possible buttons
 */
#define SKY_BTN         0x0080 /**< sky box code for 'Sky' button */
#define SERVICES_BTN    0x007E /**< sky box code for 'Services' button */
#define CURSOR_DOWN_BTN 0x0059 /**< sky box code for 'Down' button */
#define SELECT_BTN      0x005C /**< sky box code for 'Select' button */
#define BACKUP_BTN      0x0083 /**< sky box code for 'Backup' button */

#define SKYPLUS   0x0C00 /**< add this to turn a sky code into a sky+ one */
#define SKYPLUSHD 0x5C00 /**< add this to turn a sky code into a sky hd one */

static int debug = 0;

/**
 * Handle for talking to either a usb or serial sky controller
 */
typedef struct sky_handle
{
    sky_usb *usb; /* NULL if serial in use */
    int      serial_fd;
}
sky_handle;

typedef enum sky_box_type
{
    sky_box_type_normal,
    sky_box_type_plus,
    sky_box_type_plushd,
}
sky_box_type;

static void print_syntax(const char *name)
{
    fprintf(stderr,
        "%s syntax:\n"
#ifdef ENABLE_USB
        "[--controller <usb controller number - 1 for 1st controller connected on system, 2 for second, ...>]\n"
#endif
        "[--scontroller <serial device sky controller is connected to>]\n"
        "[--output <output number, as labelled on the controller>]\n"
        "[--sky] - tell --channel to send normal sky codes\n"
        "[--sky+] - tell --channel to send sky+ codes\n"
        "[--sky+hd] - tell --channel to send sky+hd codes\n"
#ifdef ENABLE_USB
        "[--setup] - send compiled in setup command to sky-control\n"
#endif
        "[--debug] - print debug output\n"
        "[--log <logfile name>] - log output to file\n"
        "[--channel <x>] Change to channel 'x' - sends 'Sky', channel num, wait 2 secs,\n"
        "          'Backup'. Channel number may contain S (services), D (down),\n"
        "          X (select) to allow selection of 'other' channels\n"
#ifndef WIN32
        "[--feedback <serial port>] For channel change commands send using\n"
        "          --channel, wait for feedback from the sky box on given serial\n"
        "          port. eg. /dev/ttyS0\n"
#endif /* WIN32 */
        "<cmd1> <cmd2> <cmd3> ... - any number of 4 digit control codes\n\n", name);
    fprintf(stderr, "Examples:\n\n");
#ifdef ENABLE_USB
    fprintf(stderr,
        "send channel 123 to sky digibox on usb controller 1, output 1:\n"
        "%s --controller 1 --output 1 0001 0002 0003\n\n", name);
    fprintf(stderr,
        "send channel 356 to sky+ on usb controller 1, output 2:\n"
        "%s --controller 1 --output 2 0C03 0C05 0C06\n\n", name);
    fprintf(stderr,
        "send complete sequence to change to channel 123 on a Sky Box:\n"
        "%s --sky --channel 123\n\n", name);
    fprintf(stderr,
        "send complete sequence to change to channel 123 on a Sky+ Box:\n"
        "%s --sky+ --channel 123\n\n", name);
#endif
#ifdef WIN32
    fprintf(stderr,
        "send complete sequence to change to channel 123 on a Sky+ Box,\n"
        "using an RS232 controller connected to COM1:\n"
        "%s --scontroller COM1 --sky+ --channel 123\n\n", name);
#else /* WIN32 */
    fprintf(stderr,
        "send complete sequence to change to channel 123 on a Sky+ Box,\n"
        "using an RS232 controller connected to ttyS0:\n"
        "%s --scontroller /dev/ttyS0 --sky+ --channel 123\n\n", name);
#ifdef ENABLE_USB
    fprintf(stderr,
        "send complete sequence to change to channel 123 on a Sky+ Box and\n"
        "wait for feedback from the sky box to confirm the channel change was ok:\n"
        "%s --sky+ --feedback /dev/ttyS0 --channel 123\n\n", name);
#endif
    fprintf(stderr,
        "send complete sequence to change to channel 123 on a Sky+ Box and\n"
        "wait for feedback from the sky box to confirm the channel change was ok,\n"
        "with a serial controller on ttyS0 and the Sky box connected to ttyS1:\n"
        "%s --sky+ --scontroller /dev/ttyS0 --feedback /dev/ttyS1 --channel 123\n\n", name);
#endif /* WIN32 */
}

static int sky_serial_sendcommand(int fd, int output, unsigned int command)
{
    unsigned char buf[4];
    int ret;
    int len ;

    assert(fd != -1);
    assert(output >= 0);
    assert(output < 30);
    assert(command < 0xffff);
    
    if (output == 0 || output == 1)
    {
          /* use older format for first two outputs to support older
           * controllers */
        buf[0] = '0' + output;
        buf[1] = command >> 8;
        buf[2] = command & 0xff;
        len = 3;
        if (debug)
        {
            printf("sending cmd %04x to output %d: bytes %d, %d, %d\n",
                   command, output+1, buf[0], buf[1], buf[2]);
        }
    }
    else
    {
        buf[0] = 'C';
        buf[1] = output;
        buf[2] = command >> 8;
        buf[3] = command & 0xff;
        len = 4;
        if (debug)
        {
            printf("sending cmd %04x to output %d: bytes %d, %d, %d, %d\n",
                   command, output+1, buf[0], buf[1], buf[2], buf[3]);
        }
    }
    
#ifdef WIN32
    ret = serial_write(fd, buf, len);
#else /* WIN32 */
    ret = write(fd, buf, len);
#endif /* WIN32 */
    if (ret != len)
    {
        return -1;
    }

    return 0;
}

static void send_sky_code(sky_handle *sky, int box_num, unsigned int code)
{
    if (debug)
    {
        printf("sending code %04x to output %d\n", code, box_num+1);
    }

#ifdef ENABLE_USB
    if (sky->usb)
    {
        if (sky_usb_sendcommand(sky->usb, box_num, code) != 0)
        {
            perror("sky_usb_sendcommand failed");
            exit(EXIT_FAILURE);
        }
    }
    else
#endif
    {
        if (sky_serial_sendcommand(sky->serial_fd, box_num, code) != 0)
        {
            perror("sky_serial_sendcommand failed");
            exit(EXIT_FAILURE);
        }
    }
}

#ifdef ENABLE_USB
static void send_setup_command(sky_handle *sky, int box_num)
{
    sky_config config;
    
    assert(sky->usb);

    /* delay between sending non-identical key presses
     * the delay is 150 * (interkeydelayhigh * 256 + interkeydelaylow) * 5 ns
     * which if this is 150 and interkeydelayhigh/low are 0x05, 0xdc is 300ms.
     */
    config.interkeydelay     = 150;

    /* delay between sending identical key presses
     * normally set to same or higher value than interkeydelay
     * the delay is 150 * (interkeydelayhigh * 256 + interkeydelaylow) * 5 ns
     * which if this is 150 and interkeydelayhigh/low are 0x05, 0xdc is 300ms.
     */
    config.samekeydelay      = 150;

    config.interkeydelayhigh = 0x05; /* used in above two delays */
    config.interkeydelaylow  = 0xdc; /* used in above two delays */

     /* used for delay between sending bits to sky box - do not change */
    config.interbitdelayhigh = 0x0a;
    config.interbitdelaylow  = 0x8b;

    config.repeatdelay       = 75; /* do not change */
    config.repeatcount       = 1; /* do not change */

    if (sky_usb_sendconfig(sky->usb, box_num, &config) != 0)
    {
        perror("sky_usb_sendconfig failed");
        exit(EXIT_FAILURE);
    }
}
#endif

#ifndef WIN32
typedef struct ReadData
{
    int    fd; /* serial port fd */
    time_t timeout; /* time to give up at */
}
ReadData;

static int readFunc(void *data, unsigned char *ptr, int bytes)
{
    ReadData *readData = data;
    int fd = readData->fd;
    int ret;
    fd_set readfds;
    struct timeval timeout;

    timeout.tv_sec = readData->timeout - time(NULL);
    timeout.tv_usec = 0;
    if (timeout.tv_sec < 0)
    {
        timeout.tv_sec = 0;
    }

    /* block if no data */
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (ret == 0)
    {
        printf("Time out waiting for data from sky box\n");
        return -1;
    }
    else if (ret != 1)
    {
        perror("select");
        return -1;
    }

    ret = read(fd, ptr, bytes);

    return ret;
}

/**
 * Wait for confirmation from the sky box
 *
 * Uses the gnome data the sky box RS232 port to confirm if the channel
 * selection has been correctly made or not
 *
 * @param chan_str      Channel that is being used
 * @param feedback_port device to read from (eg. /dev/ttyS0)
 */
static void wait_for_feedback(const char *chan_str, int fd)
{
    Gnome *gnome;

    int numItems;
    const char **items;
    const char **data;
    const int   *dataLen;

    int done = 0;
    int ret;
    ReadData readData;

    int i;

    readData.fd = fd;
    readData.timeout = time(NULL) + FEEDBACK_TIMEOUT;

    gnome = Gnome_create(readFunc, &readData);

    while (!done)
    {
        ret = Gnome_readPacket(gnome, &numItems, &items, &data, &dataLen);
        if (ret == -1)
        {
            printf("Failed reading response from sky box.\n");
            exit(1);
        }

        for (i = 0; i < numItems; i++)
        {
            if (debug)
            {
                printf("%.4s : %.*s [%d %d]\n",
                       items[i], dataLen[i],
                       data[i], dataLen[i], *data[i]);
            }
            if (strncmp(items[i], "CE00", 4) == 0)
            {
                int len = dataLen[i];
                char chan[5];
                strncpy(chan, data[i], len);
                chan[len] = 0;

                if (len == 4 && *chan == 127)
                {
                    /* sky box sends channel '0101' as '<del>101' -
                     * fix the <del> back into a 0 */
                    *chan = '0';
                }
                if (chan[len - 1] != '-')
                {
                    /* full channel number received */
                    if (len != (int)strlen(chan_str) ||
                        strncmp(chan, chan_str, len) != 0)
                    {
                        printf("Tuning failed, channel '%s' selected\n",
                               chan);
                        done = -1;
                    }
                    else
                    {
                        printf("Tuning successful, channel '%s' selected\n",
                               chan);
                        /** waiting for the channel to tune was failing
                         * in my test script that changed from 101 -> 102
                         * and back continuously, as data was received
                         * for the previous channel - so we just check
                         * we've sent the correct channel number.
                         * It may be worth ignoring other data until
                         * this point or slightly after.
                         */
                        done = 1;
                    }
                }
            }
            else if (strncmp(items[i], "CEER", 4) == 0)
            {
                printf("Channel error: %.*s\n", dataLen[i], data[i]);
                done = -1;
            }
            else if (strncmp(items[i], "SSCN", 4) == 0)
            {
                if (dataLen[i] == (int)strlen(chan_str) &&
                    strncmp(data[i], chan_str, dataLen[i]) == 0)
                {
                    printf("Current channel '%.*s' [correct]\n",
                           dataLen[i], data[i]);
                }
                else
                {
                    printf("Current channel '%.*s' [incorrect]\n",
                           dataLen[i], data[i]);
                }
            }
            else if (strncmp(items[i], "SSCA", 4) == 0)
            {
                printf("Channel description '%.*s'\n",
                       dataLen[i], data[i]);
            }
            else if (strncmp(items[i], "PUCP", 4) == 0 ||
                     strncmp(items[i], "PUSP", 4) == 0)
            {
                printf("Error '%.*s'\n",
                       dataLen[i], data[i]);
            }
            else
            {
#ifdef PRINT_UNHANDLED_DATA
                printf("%.4s : %.*s [%d %d]\n",
                 items[i], dataLen[i],
                 data[i], dataLen[i], *data[i]);
#endif /* PRINT_UNHANDLED_DATA */
            }
        }
    }

    Gnome_destroy(gnome);

    if (done == -1)
    {
        printf("Failed\n");
        exit(1);
    }
}
#endif /* WIN32 */

static void send_sky_channel(sky_handle   *sky,
                             int           box_num,
                             sky_box_type  box_type,
                             const char   *chan_str,
                             const char   *feedback_port)
{
    const char *p;
    unsigned int add = 0;
    int singlechannel = 1;
    int fd = -1;

    if (box_type == sky_box_type_plus)
        add = SKYPLUS;
    else if (box_type == sky_box_type_plushd)
        add = SKYPLUSHD;

    if (debug)
    {
        printf("sending channel '%s'\n", chan_str);
    }

    for (p = chan_str; *p != 0; p++)
    {
        if (!isdigit(*p) && *p != 'S' && *p != 'D' && *p != 'X')
        {
            fprintf(stderr, "Invalid channel number '%s'\n", chan_str);
            exit(EXIT_FAILURE);
        }
    }

    if (feedback_port != NULL)
    {
        fd = serial_openport(feedback_port, 57600, 1);
        if (fd == -1)
        {
            perror("Opening feedback serial port failed");
            exit(1);
        }
    }

    send_sky_code(sky, box_num, SKY_BTN + add);

    for (p = chan_str; *p != 0; p++)
    {
        unsigned int code;

        if (*p == 'S')
        {
            code = SERVICES_BTN;
            singlechannel = 0;
        }
        else if (*p == 'D')
        {
            code = CURSOR_DOWN_BTN;
            singlechannel = 0;
        }
        else if (*p == 'X')
        {
            code = SELECT_BTN;
            singlechannel = 0;
        }
        else
        {
            code = *p - '0';
        }

        code += add;

        send_sky_code(sky, box_num, code);
    }

    /* if the command is a simple channel selection, and the user has
     * specified a serial port, check the gnome data returned from the sky
     * box to see if the channel selected ok.
     *
     * See http://www.heenan.me.uk/control-sky-from-pc/gnome-protocol.html
     * for protocol information.
     */
    if (singlechannel && feedback_port != NULL)
    {
        /* now a trick - to force the output of the gnome data even if we're
         * already on the right channel, send '9' then 'backup'
         */
        send_sky_code(sky, box_num, 9 + add);
        send_sky_code(sky, box_num, BACKUP_BTN + add);
#ifndef WIN32
        wait_for_feedback(chan_str, fd);
#endif /* WIN32 */
    }
    else
    {
        /* wait a bit, then send 'backup' to dismiss the channel info
         * banner
         */
        if (debug)
        {
            printf("Waiting %d ms...\n", PRE_BACKUP_WAIT);
        }

#ifdef WIN32
        Sleep(PRE_BACKUP_WAIT);
#else /* WIN32 */
        usleep(PRE_BACKUP_WAIT * 1000);
#endif /* WIN32 */

        send_sky_code(sky, box_num, BACKUP_BTN + add);
    }
}

int main(int argc, char *argv[])
{
    int                device_num  = 0; /* which usb controller to use */
    const char        *device_port = NULL; /* serial port for controller */
    int                box_num     = 0; /* which output on controller to use */
    const char        *channel_num = NULL; /* channel number to send to box */
    const char        *feedback_port = NULL; /* serial port to read sky box
                                              * feedback from */
    sky_box_type       box_type    = sky_box_type_normal; /* or sky+ or sky+hd */
    int                setup       = 0; /* non-zero to send a configuration command too */
    sky_handle         sky = { NULL, -1 };

    enum options
    {
        option_box,
        option_output,
        option_channel,
        option_controller,
        option_debug,
        option_feedback,
        option_log,
        option_scontroller,
        option_setup,
        option_sky,
        option_skyplus,
        option_skyplushd
    };
    static const struct option options_list[] =
    {
        { "box",         required_argument, NULL, option_box          },
        { "output",      required_argument, NULL, option_output       },
        { "channel",     required_argument, NULL, option_channel      },
        { "scontroller", required_argument, NULL, option_scontroller  },
        { "debug",       no_argument,       NULL, option_debug        },
        { "feedback",    required_argument, NULL, option_feedback     },
        { "log",         required_argument, NULL, option_log          },
#ifdef ENABLE_USB
        { "controller",  required_argument, NULL, option_controller   },
        { "setup",       no_argument,       NULL, option_setup        },
#endif
        { "sky",         no_argument,       NULL, option_sky          },
        { "sky+",        no_argument,       NULL, option_skyplus      },
        { "sky+hd",      no_argument,       NULL, option_skyplushd    },
        { NULL, 0, NULL, 0 }
    };

    if (argc < 2)
    {
        print_syntax(argv[0]);
        return EXIT_FAILURE;
    }

    while (1)
    {
        int c;

        c = getopt_long(argc, argv, "", options_list, NULL);
        if (c == -1)
            break;

        switch (c)
        {
            case option_controller:
                device_num = atoi(optarg) - 1;
                break;

            case option_box:
                /* deprecated - takes output number starting from 0 */
            case option_output:
                /* new form of --box - takes output number as labelled on
                 * controller */
                box_num = atoi(optarg);
                if (c == option_output)
                    box_num -= 1;
            
                if (box_num < 0 || box_num > 30)
                {
                    printf("output number '%d' out of range.\n", box_num+1);
                    print_syntax(argv[0]);
                    return EXIT_FAILURE;
                }
                break;

            case option_feedback:
                feedback_port = optarg;
                break;

            case option_sky:
                box_type = sky_box_type_normal;
                break;

            case option_skyplus:
                box_type = sky_box_type_plus;
                break;
            
            case option_skyplushd:
                box_type = sky_box_type_plushd;
                break;

            case option_scontroller:
                device_port = optarg;
                break;

            case option_channel:
                channel_num = optarg;
                break;

            case option_debug:
                debug = 1;
                break;

            case option_setup:
                setup = 1;
                break;

            case option_log:
                if (freopen(optarg, "a", stderr) == NULL)
                {
                    perror("opening log file %s for writing failed");
                    return EXIT_FAILURE;
                }
                if (freopen(optarg, "a", stdout) == NULL)
                {
                    perror("opening log file %s for writing failed");
                    return EXIT_FAILURE;
                }
                break;

            default:
                print_syntax(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (device_port)
    {
        /* serial */
        sky.serial_fd = serial_openport(device_port, 57600, 1);
        if (sky.serial_fd == -1)
        {
            perror("Opening serial port failed");
            exit(EXIT_FAILURE);
        }
        if (debug)
        {
            unsigned char version[8];
            int numOutputs;
            
            // this will exit with an error if it fails to query the version
            numOutputs = SkySerial_getVersion(sky.serial_fd, version,
                                              500, 500, 1);
            
            printf("Found serial Sky Controller, version: %s, %d outputs\n",
                   version, numOutputs);
        }
    }
    else
    {
#ifdef ENABLE_USB
        if (sky_usb_initialise() != 0)
        {
            fprintf(stderr, "sky_usb_initalise failed\n");
            exit(EXIT_FAILURE);
        }

        sky.usb = sky_usb_create(debug, USB_TIMEOUT);
        if (sky.usb == NULL)
        {
            fprintf(stderr, "unable to create sky controller handle %d\n",
                    device_num);
            exit(EXIT_FAILURE);
        }

        if (sky_usb_finddevice(sky.usb, device_num) != 0)
        {
            fprintf(stderr, "unable to find USB sky controller number %d\n",
                    device_num);
            fprintf(stderr, "Please check the controller is connected.\n");
            if (device_num == 0)
            {
                fprintf(stderr, "If you are using a serial controller please specify the port it is connected to use the --scontroller command line switch.\n");
            }
            exit(EXIT_FAILURE);
        }

        if (debug)
        {
            sky_usb_printinfo(sky.usb);
        }

        if (setup)
        {
            send_setup_command(&sky, box_num);
        }
#else
        fprintf(stderr, "This version of sky-control was compiled with USB support disabled. Please use --scontroller to specify the port a serial controller is connected.\n");
        exit(EXIT_FAILURE);
#endif
    }

    if (channel_num)
    {
        send_sky_channel(&sky, box_num, box_type, channel_num, feedback_port);
    }
    else
    {
        while (optind < argc)
        {
            const char    *cmd = argv[optind++];
            unsigned int   code;

            if (sscanf(cmd, "%x", &code) != 1 || code > 0xFFFF)
            {
                fprintf(stderr, "Invalid command code: '%s'\n", cmd);
                exit(EXIT_FAILURE);
            }

            send_sky_code(&sky, box_num, code);
        }
    }

#ifdef ENABLE_USB
    if (!device_port)
    {
        sky_usb_destroy(sky.usb);

        sky_usb_finalise();
    }
#endif

    return EXIT_SUCCESS;
}
