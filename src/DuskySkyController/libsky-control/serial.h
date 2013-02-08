/**
 * Serial port abstraction
 *
 * Copyright (C) Joseph Heenan, 1999-2008. All rights reserved.
 *
 * $Id: serial.h,v 1.6 2010-06-19 18:21:49 joseph Exp $
 */

#ifndef SERIAL_H
#define SERIAL_H

/**
 * Open serial port
 *
 * @param port     Serial device to use (eg. /dev/ttyS0)
 * @param baudrate Baudrate to use
 * @param stopbits Stops bits
 *
 * @return fd for serial port or -1 on failure
 */
int serial_openport(const char *port, int baudrate, int stopbits);

int serial_write(int fd, const char *ptr, int bytes);

int serial_read(int fd, unsigned char *ptr, int bytes);

/**
 * Read from serial port with timeout
 * 
 * Note: returns as soon as any data read, does not wait for buffer
 * to fill.
 * 
 * @param fd    fd to read from
 * @param ptr   ptr to write data to
 * @param bytes maximum number of bytes to read
 * @param msecs timeout in milliseconds
 * 
 * @retval -2 Timed out
 * @retval -1 Error
 * @return Otherwise, returns number of bytes read
 */
int serial_readtimeout(int fd, unsigned char *ptr, int bytes,
                       int msecs);

void serial_close(int fd);

/**
 * Connect stdin to serial out & serial in to stdout
 *
 * @param fd         Serial port fd to use
 * @param escapechar Character that user presents to exit terminal mode
 */
void serial_terminal(int fd, unsigned char escapechar);

#endif
