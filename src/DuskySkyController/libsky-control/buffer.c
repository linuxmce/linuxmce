/**
 * Generic buffer module
 *
 * Copyright (C) Joseph Heenan, 2005-2008. All rights reserved.
 *
 * $Id: buffer.c,v 1.3 2009-02-08 16:25:21 joseph Exp $
 */

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "buffer.h"

#define DBUGF(x) /* printf x */

struct Buffer
{
    unsigned char   *buffer;   /**< memory used to hold buffered data */
    int              size;     /**< size of 'buffer' */
    unsigned char   *start;    /**< start of current token being parsed */
    unsigned char   *ptr;      /**< next character to be returned by ReadChar */
    unsigned char   *used;     /**< next free character */
    Buffer_ReadFunc  readFunc; /**< func to use to fill buffer */
    void            *readData; /**< parameter for readFunc */
};

Buffer *Buffer_create(int size, Buffer_ReadFunc readFunc, void *readData)
{
    Buffer *b;

    b = calloc(1, sizeof(*b));
    if (b == NULL)
        return NULL;

    b->buffer = malloc(size);
    if (b->buffer == NULL)
    {
        free(b);
        return NULL;
    }

    b->size     = size;
    b->readFunc = readFunc;
    b->readData = readData;

    Buffer_clear(b);

    return b;
}

void Buffer_destroy(Buffer *b)
{
    if (b == NULL)
        return;

    free(b->buffer);
    free(b);
}


const unsigned char *Buffer_getStart(Buffer *buf)
{
    return buf->start;
}

void Buffer_clear(Buffer *buf)
{
    buf->start = buf->buffer;
    buf->ptr   = buf->buffer;
    buf->used  = buf->buffer;
}

void Buffer_updateStart(Buffer *buf)
{
    buf->start = buf->ptr;
}

/**
 * print string, escaping non-printable characters as hex [XX]
 */
static void asciiPrint(FILE *f,
                       const unsigned char *str,
                       const unsigned char *end)
{
    const unsigned char *p;
    assert(end >= str);

    for (p = str; p != end; p++)
    {
        unsigned char c = *p;
        if (isprint(c))
            putc(c, f);
        else
            fprintf(f, "[%02X]", c);
    }
}

void Buffer_dumpContents(Buffer *buffer, FILE *f)
{
    asciiPrint(f, buffer->buffer, buffer->ptr);
}

/**
 * Read more data into buffer
 *
 * Will block if no more data available
 *
 * Returns 0 on success, -1 on error
 */
static int Buffer_fill(Buffer *buf)
{
    int ret;

    if (buf->used >= buf->buffer + buf->size)
    {
        assert(buf->used == buf->buffer + buf->size);

        printf("Buffer full, ignored: [[[");
        asciiPrint(stdout, buf->buffer, buf->used);
        printf("]]]\n");

        Buffer_clear(buf);
    }

    /* read one byte */
    ret = buf->readFunc(buf->readData, buf->used, 1);
    if (ret <= 0)
        return -1;

    DBUGF(("FillRead : %d [%c]\n",
           *buf->used, isprint(*buf->used) ? *buf->used : '?'));
    buf->used += ret;
    DBUGF(("fill buffer: ptr  = %p\n", buf->ptr));
    DBUGF(("fill buffer: used = %p\n", buf->used));

    return 0;
}

unsigned int Buffer_bytesRead(Buffer *buf)
{
    return buf->ptr - buf->start;
}

void Buffer_unread(Buffer *buf)
{
    buf->ptr--;
    assert(buf->ptr >= buf->buffer);
    if (buf->ptr < buf->buffer)
    {
        buf->ptr = buf->buffer;
    }
}

void Buffer_unreadAll(Buffer *buf)
{
    buf->start = buf->buffer;
    buf->ptr   = buf->buffer;
}

int Buffer_readChar(Buffer *buf, unsigned char *out)
{
    if (buf->ptr >= buf->used)
    {
        int ret;

        assert(buf->ptr == buf->used);
        ret = Buffer_fill(buf);
        if (ret != 0)
            return -1;

        assert(buf->ptr <= buf->used);
    }
    *out = *(buf->ptr++);

    return 0;
}
