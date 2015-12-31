/**
 * Helper functions for serial sky controllers
 * 
 * Copyright (c) 2009-2011, Joseph Heenan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Joseph Heenan nor the names of any other
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Joseph Heenan ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "sky-serial.h"
#include "serial.h"

#define DBUGF(x) //printf x

/**
 * Reads a line from the given fd
 * 
 * Returns when a full line is found, a timeout occurs or the buffer
 * is full.
 * 
 * @return 0 for success, -2 for timeout
 */
int SkySerial_readline(int fd, unsigned char *data, size_t size, int timeout)
{
    unsigned char *p = data;
    int ret;
    unsigned char *end = data+size-1; /* -1 for NUL */
    
    do
    {
        ret = serial_readtimeout(fd, p, 1, timeout);
        if (ret == -2)
            return -2;
        if (ret != 1)
        {
            fprintf(stderr, "Failed to read from serial port [%d]\n",
                    __LINE__);
            exit(EXIT_FAILURE);
        }
        p++;
    }
    while (p[-1] != '\n' && p < end);
    *p = 0;
    if (p[-1] == '\n')
      *--p = 0;
    if (p[-1] == '\r')
      *--p = 0;
    return 0;
}


/**
 * Read version number from sky controller
 * 
 * @param version 7 bytes for version string to be returned in
 * 
 * @param initialtimeout  timeout for first response, in ms
 * @param timeout         timeout for subsequence responses, in ms
 * 
 * @return number of outputs
 */
int SkySerial_getVersion(int fd, unsigned char *version,
                          int initialtimeout, int timeout,
                          bool noresponseFatal)
{
    unsigned char data[128];
    int ret;
    int delay = initialtimeout;
    bool response = false;
    int numOutputs = -1;
    
    *version = 0;
    
    /* Eight 'q's should be enough to ensure a response even if the
     * controller is already in program mode or ascii send or has had
     * a single 'C' and not the rest of the 'C' command */
    ret = serial_write(fd, "qqqqqqqq", 8);
    if (ret != 8)
    {
        fprintf(stderr, "Failed to write to serial port [%d]\n", __LINE__);
        exit(EXIT_FAILURE);
    }
    
    while (1)
    {
        ret = SkySerial_readline(fd, data, sizeof(data), delay);
        delay = timeout;
        if (ret != 0)
        {
            /* timeout */
            if (*version == 0)
            {
                if (response)
                {
                    fprintf(stderr, "Found an RS232 device, but it doesn't "
                            "respond to 'q' like a sky controller would\n");
                    exit(EXIT_FAILURE);
                }

                fprintf(stderr, "No response from sky controller for "
                        "'q' command\n");
                if (noresponseFatal)
                    exit(EXIT_FAILURE);
            }
            /* otherwise, we've been successful */
            return numOutputs;
        }
        response = true;

        DBUGF(("getVersion: read from serial: '%s'\n", data));
        if (data[0] == 'V' && isdigit(data[1]) &&
            data[2] == '.' && isdigit(data[3]) &&
            data[4] == '.' && isdigit(data[5]) &&
            data[6] == 0)
        {
            strcpy((char *)version, (char *)data);
        }
        
        unsigned char *p = data;
        while (isdigit(*p))
          p++;
        if (p != data && strcmp((char *)p, " output") == 0)
        {
            (void) sscanf((char *)data, "%d", &numOutputs);
        }
    }
    
    return numOutputs;
}
