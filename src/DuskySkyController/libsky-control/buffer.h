/**
 * Generic buffer module
 *
 * Copyright (C) Joseph Heenan, 2005-2008. All rights reserved.
 *
 * $Id: buffer.h,v 1.2 2008-03-30 21:45:21 joseph Exp $
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>

typedef struct Buffer Buffer;

/**
 * Pointer for function used to fill buffer
 *
 * This function should block if there is no more data.
 *
 * @param data  readData passed to Buffer_create
 * @param ptr   Pointer to write data into
 * @param bytes Number of bytes to read
 *
 * @return -1 in case of error, otherwise number of bytes read
 */
typedef int (*Buffer_ReadFunc)(void *data, unsigned char *ptr, int bytes);

/**
 * Create a buffer
 *
 * @param size Maximum size of data to be held, in bytes
 * @param readFunc Function to use to read data into the buffer
 * @param readData Data to supply to readFunc
 *
 * @return Pointer to newly created, or NULL on failure.
 */
Buffer *Buffer_create(int size, Buffer_ReadFunc readFunc, void *readData);

/**
 * Destroy a buffer
 *
 * Frees up all memory associated with buffer.
 *
 * @param buf Buffer to be destroyed
 */
void Buffer_destroy(Buffer *buf);

/**
 * Clear all data from a buffer
 *
 * @param buf Buffer to be cleared
 */
void Buffer_clear(Buffer *buf);

/**
 * Return the start of the current token
 *
 * Returns a pointer to the data that was about to be read the last time
 * Buffer_updateStart() was called.
 *
 * @param buf Buffer to query
 * @return Pointer to start of current token
 */
const unsigned char *Buffer_getStart(Buffer *buf);

/**
 * Update the start of the current token
 *
 * @param buf Buffer to remove data from
 */
void Buffer_updateStart(Buffer *buf);

/**
 * Dump the whole buffer contents, including any removed data
 *
 * @param buf Buffer to dump
 * @param f   File handle to print output to
 */
void Buffer_dumpContents(Buffer *buf, FILE *f);

/**
 * Get length of current token in bytes
 *
 * @param buf Buffer to query token length
 * @return number of bytes read from token so far
 */
unsigned int Buffer_bytesRead(Buffer *buf);

/**
 * Move the read pointer back one byte
 *
 * Has the effect of making the next call to Buffer_readChar return the
 * previous character again.
 *
 * @param buf Buffer to unread from
 */
void Buffer_unread(Buffer *buf);

/**
 * Unread whole buffer contents
 *
 * Makes the buffer behaves as if no characters have been read since last
 * Buffer_clear() call.
 *
 * @param buf Buffer to reset
 */
void Buffer_unreadAll(Buffer *buf);

/**
 * Remove the first character from the buffer and return it.
 *
 * @param[in]  buf Buffer to read from
 * @param[out] out For character read to be returned in
 *
 * @return 0 for success, -1 on failure
 */
int Buffer_readChar(Buffer *buf, unsigned char *out);

#endif /* BUFFER_H */
