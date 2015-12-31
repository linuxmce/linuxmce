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

#ifndef SKY_SERIAL_H
#define SKY_SERIAL_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * Reads a line from the given fd
 * 
 * Returns when a full line is found, a timeout occurs or the buffer
 * is full.
 * 
 * @return 0 for success, -2 for timeout
 */
int SkySerial_readline(int fd, unsigned char *data, size_t size, int timeout);

/**
 * Read version number from sky controller
 * 
 * @param version 7 bytes for version string to be returned in
 * 
 * @param initialtimeout  timeout for first response, in ms
 * @param timeout         timeout for subsequence responses, in ms
 * 
 * @return number of outputs, or -1 if not determined
 */
int SkySerial_getVersion(int fd, unsigned char *version,
                         int initialtimeout, int timeout,
                         bool noresponseFatal);

#endif /* SKY_SERIAL_H */
