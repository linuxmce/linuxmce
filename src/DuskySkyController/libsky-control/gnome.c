/**
 * Sky gnome packet decoder
 *
 * Based on documentation at:
 *
 * http://www.heenan.me.uk/control-sky-from-pc/gnome-protocol.html
 *
 * Copright (C) Joseph Heenan, 2005-2008. All rights reserved.
 *
 * $Id: gnome.c,v 1.2 2008-03-30 21:45:21 joseph Exp $
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "buffer.h"
#include "gnome.h"

/* #define DBUGF(x) printf x */
#define DBUGF(x)

typedef enum State
{
    WaitingForStart,
    PacketStart,
    ItemStart,
    ItemLen,
    Item,
    Checksum
}
State;

struct Gnome
{
    State        state;
    Buffer      *buffer;
    const char  *items[64];
    const char  *data[64];
    int          dataLen[64];
    int          numItems;
    unsigned int check;
    unsigned int length;
    unsigned int itemLen;
};

/* convert single hex char */
static int converthex(unsigned char c)
{
    assert(isxdigit(c));
    c = toupper(c);
    if (isdigit(c))
        return c-'0';
    else
        return c-'A'+10;
}

Gnome *Gnome_create(Buffer_ReadFunc readFunc, void *readData)
{
    Gnome *gnome;

    gnome = calloc(1, sizeof(*gnome));
    if (gnome == NULL)
        return NULL;

    gnome->buffer = Buffer_create(2048, readFunc, readData);
    if (gnome->buffer == NULL)
    {
        free(gnome);
        return NULL;
    }

    gnome->state = WaitingForStart;

    return gnome;
}

void Gnome_destroy(Gnome *gnome)
{
    if (gnome == NULL)
        return;

    Buffer_destroy(gnome->buffer);
    free(gnome);
}

/**
 * Read and process one byte from the buffer
 *
 * @retval 0  more to go
 * @retval 1  packet finished
 * @retval -1 error
 */
static int Gnome_readByte(Gnome *gnome)
{
    unsigned char byte;
    int ret;

    ret = Buffer_readChar(gnome->buffer, &byte);
    if (ret != 0)
        return -1;

    gnome->check += byte;
    DBUGF(("%d: %02X [%c]\n", gnome->state, byte, isprint(byte)?byte:'?'));

    switch (gnome->state)
    {
        case WaitingForStart:
            /* looking for LF that indicates start of packet */
            if (byte == 0x0A)
            {
                Buffer_unread(gnome->buffer);
                if (Buffer_bytesRead(gnome->buffer) > 0)
                {
                    /* there's more than just the 0x0A */
                    printf("Unexpected/invalid characters: [[[");
                    Buffer_dumpContents(gnome->buffer, stdout);
                    Buffer_clear(gnome->buffer);
                    printf("]]]\n");
                }
                Buffer_clear(gnome->buffer);
                gnome->state = PacketStart;
                gnome->check = byte;
                DBUGF(("moved to state %d\n", gnome->state));
            }
            break;

        case PacketStart:
        {
            const unsigned char *buf = Buffer_getStart(gnome->buffer);
            /* waiting for 3 length bytes */
            if (!isdigit(byte))
            {
                /* invalid - unread characters in case one is an 0x0A
                 * and try again */
                Buffer_unreadAll(gnome->buffer);
                gnome->state = WaitingForStart;
                break;
            }
            if (Buffer_bytesRead(gnome->buffer) < 3)
            {
                /* still waiting */
                break;
            }
            gnome->length = (buf[0]-'0') * 100 + (buf[1]-'0') * 10 +
              buf[2]-'0';
            DBUGF(("Packet length = %u [%.3s]\n", gnome->length, buf));
            if (gnome->length > 999)
            {
                Buffer_unreadAll(gnome->buffer);
                gnome->state = WaitingForStart;
                break;
            }
            Buffer_updateStart(gnome->buffer);
            gnome->numItems = 0;
            gnome->state = ItemStart;
            break;
        }

        case ItemStart:
            if (Buffer_bytesRead(gnome->buffer) < 4)
            {
                /* still waiting */
                break;
            }
            gnome->items[gnome->numItems] =
                (const char *) Buffer_getStart(gnome->buffer);
            DBUGF(("item: %.4s\n", Buffer_getStart(gnome->buffer)));
            Buffer_updateStart(gnome->buffer);
            gnome->state = ItemLen;
            break;

        case ItemLen:
        {
            const char *buf = (const char *) Buffer_getStart(gnome->buffer);
            /* waiting for 3 length bytes */
            if (!isdigit(byte))
            {
                /* unread characters in case it's a 0x0A */
                Buffer_unreadAll(gnome->buffer);
                gnome->state = WaitingForStart;
                break;
            }
            if (Buffer_bytesRead(gnome->buffer) < 3)
            {
                /* still waiting */
                break;
            }
            gnome->itemLen = (buf[0]-'0') * 100 + (buf[1]-'0') * 10 +
                              buf[2]-'0';
            gnome->itemLen -= 4 + 3; /* 4 char item name + 3 char length */
            DBUGF(("Item length = %d\n", gnome->itemLen));
            if (gnome->itemLen > gnome->length)
            {
                Buffer_unreadAll(gnome->buffer);
                gnome->state = WaitingForStart;
                break;
            }
            gnome->length -= gnome->itemLen + 7;
            Buffer_updateStart(gnome->buffer);
            if (gnome->itemLen > 0)
            {
                gnome->state = Item;
                break;
            }

            /* zero length item */
            gnome->data[gnome->numItems] =
              (const char *) Buffer_getStart(gnome->buffer);
            gnome->dataLen[gnome->numItems] = gnome->itemLen;
            gnome->numItems++;

            if (gnome->length <= 5)
            {
                /* end of packet */
                gnome->state = Checksum;
            }
            else
            {
                gnome->state = ItemStart;
            }
            break;
        }

        case Item:
            if (Buffer_bytesRead(gnome->buffer) < gnome->itemLen)
            {
                /* still waiting */
                break;
            }
            gnome->data[gnome->numItems] =
              (const char *) Buffer_getStart(gnome->buffer);
            gnome->dataLen[gnome->numItems] = gnome->itemLen;
            gnome->numItems++;
            DBUGF(("data: %.*s\n",
                   gnome->itemLen, Buffer_getStart(gnome->buffer)));
            Buffer_updateStart(gnome->buffer);
            DBUGF(("remaining length: %d\n", gnome->length));
            if (gnome->length <= 5)
            {
                /* end of packet */
                gnome->state = Checksum;
            }
            else
            {
                gnome->state = ItemStart;
            }
            break;

        case Checksum:
        {
            unsigned int calc;
            const unsigned char *p = Buffer_getStart(gnome->buffer);

            gnome->check -= byte;
            if (!isxdigit(byte))
            {
                /* unread characters in case it's a 0x0A */
                Buffer_unreadAll(gnome->buffer);
                gnome->state = WaitingForStart;
                break;
            }
            if (Buffer_bytesRead(gnome->buffer) < 2)
            {
                /* still waiting */
                break;
            }

            calc = converthex(*p) * 0x10 + converthex(*(p+1));
            gnome->check &= 0xff;
            DBUGF(("checksum: %.*s [%x,%x]\n", 2, gnome->buffer.ptr-2,
                   gnome->check & 0xff, calc));
            if (calc != gnome->check)
            {
                printf("Checksum mismatch: %x vs %x\n", calc, gnome->check);
                /* incorrect checksum */
                Buffer_unreadAll(gnome->buffer);
                gnome->state = WaitingForStart;
                return 0;
            }

            Buffer_clear(gnome->buffer);
            gnome->state = WaitingForStart;
            /* complete & correct packet */
            return 1;
        }
    }

    return 0;
}

int Gnome_readPacket(Gnome        *gnome,
                     int          *numItems,
                     const char ***items,
                     const char ***data,
                     const int   **dataLen)
{
    int ret;

    do
    {
        ret = Gnome_readByte(gnome);
    }
    while (ret == 0);

    if (ret == -1)
    {
        *numItems = 0;
        *items    = NULL;
        *data     = NULL;
        *dataLen  = NULL;

        return -1;
    }

    *numItems = gnome->numItems;
    *items    = gnome->items;
    *data     = gnome->data;
    *dataLen  = gnome->dataLen;

    return 0;
}
