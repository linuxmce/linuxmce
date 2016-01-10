/**
 *
 * Abstract interface for reading packets from sky gnome
 *
 * $Id: gnome.h,v 1.2 2008-03-30 21:45:21 joseph Exp $
 *
 * Copyright 2005 Joseph Heenan, all rights reserved.
 */

#ifndef GNOME_H
#define GNOME_H

#include "buffer.h"

typedef struct Gnome Gnome;

/**
 * Create a gnome object
 *
 * @param readFunc Function to use to get the data to be parsed
 * @param readData Pointer to be passed to readFunc
 *
 * @return Pointer to new gnome object, or NULL on failure
 */
Gnome *Gnome_create(Buffer_ReadFunc readFunc, void *readData);

/**
 * Destroy a gnome object
 *
 * @param gnome Gnome to be destroyed
 */
void Gnome_destroy(Gnome *gnome);

/**
 * Read a single packet
 *
 * This will block until a whole, correct packet is received.
 *
 * If a timeout is required, implement it inside 'readFunc'.
 *
 * @param[in]  gnome    Gnome object
 * @param[out] numItems Number of entries in the items / data arrays
 * @param[out] items    Array of pointers to start of items, all of length 4
 * @param[out] data     Array of pointers to start of items, length in dataLen
 * @param[out] dataLen  Array of lengths of strings in data
 *
 * @return 0 for success, -1 if a fatal error occurred
 */
int Gnome_readPacket(Gnome        *gnome,
                     int          *numItems,
                     const char ***items,
                     const char ***data,
                     const int   **dataLen);

#endif /* GNOME_H */
