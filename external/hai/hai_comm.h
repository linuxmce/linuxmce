/*******************************************************************************/
/* Copyright (C) 2004-2005  Chuck Cannon                                       */
/*                                                                             */
/* This program is free software; you can redistribute it and/or               */
/* modify it under the terms of the GNU General Public License                 */
/* as published by the Free Software Foundation; either version 2              */
/* of the License, or (at your option) any later version.                      */
/*                                                                             */
/* This program is distributed in the hope that it will be useful,             */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of              */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               */
/* GNU General Public License for more details.                                */
/*                                                                             */
/* You should have received a copy of the GNU General Public License           */
/* along with this program; if not, write to the Free Software                 */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */
/*******************************************************************************/

/**
 * @file  hai_comm.h
 *
 * @brief HAI Communication Header File
 *    
 * Header file containing definitions for generic HAI communication functions.
 */

/**
 * \example hai.c
 *
 * This file contains example usage for all of the HAI lib functions.
 */

#ifndef _HAI_COMM_H_
#define _HAI_COMM_H_

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/* Includes */

#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <errno.h>

/* Defines */

/** Define HAIEXPORT for MSVC DLL building */
#ifdef WIN32
#ifdef _USRDLL
#define HAIEXPORT __declspec(dllexport)
#define HAIIMPORT __declspec(dllexport)
#else
#define HAIEXPORT
#define HAIIMPORT
#endif
#else
#define HAIEXPORT
#define HAIIMPORT
#endif

/** Define max system error code (if not defined) */
#ifndef __ELASTERROR
#define __ELASTERROR    2000
#endif

/**
 * @name HAI Communication Error Codes
 * @anchor CommErrs
 *
 * These defines are used to report network/serial errors.
 *
 * @{
 */

/** Error code for invalid argument */
#define EHAIARGUMENT    __ELASTERROR + 1
/** Error code for unexpected response */
#define EHAIRESPONSE    __ELASTERROR + 2
/** Error code for invalid session ID */
#define EHAISESSION     __ELASTERROR + 3
/** Error code for netowrk time-out */
#define EHAITIMEOUT     __ELASTERROR + 4
/** Define for max HAI comm error code */
#define __ELASTHAI      __ELASTERROR + 10
/** @} */

/**
 * @name Omni Packet Access Macros
 *
 * These macros are used to access omni data elements.
 *
 * @{
 */

/** Macro to read a \a val8. */
#define GET8(a)         (a)
/** Macro to write a \a val8. */
#define SET8(a,b)       (a) = ((b) & 0xFF)
/** Macro to read a \a val16. */
#define GET16(a)        (((a)[0] << 8) | (a)[1])
/** Macro to write a \a val16. */
#define SET16(a,b)      {(a)[0] = ((b) >> 8) & 0xFF; \
                        (a)[1] = (b) & 0xFF;}
/** Macro to read a \a val24. */
#define GET24(a)        (((a)[0] << 16) | ((a)[1] << 8) | (a)[2])
/** Macro to write a \a val24. */
#define SET24(a,b)      {(a)[0] = ((b) >> 16) & 0xFF; \
                        (a)[1] = ((b) >> 8) & 0xFF; \
                        (a)[2] = (b) & 0xFF;}
/** Macro to read a \a val32. */
#define GET32(a)        (((a)[0] << 24) | ((a)[1] << 16) \
                        | ((a)[2] << 8) | (a)[3])
/** Macro to write a \a val32. */
#define SET32(a,b)      {(a)[0] = ((b) >> 24) & 0xFF; \
                        (a)[1] = ((b) >> 16) & 0xFF; \
                        (a)[2] = ((b) >> 8) & 0xFF; \
                        (a)[3] = (b) & 0xFF;}
/** @} */

/* Typedefs */

/**
 * @name Omni Data Typedefs
 *
 * These types are used to store omni data elements. Access should
 * only be via the omni packet access macros.
 *
 * @{
 */

/** Type used to contain single-byte omni data */
typedef unsigned char val8;
/** Type used to contain 2-byte omni data */
typedef unsigned char val16[2];
/** Type used to contain 3-byte omni data */
typedef unsigned char val24[3];
/** Type used to contain 4-byte omni data */
typedef unsigned char val32[4];
/** Type used to contain 5-byte omni data */
typedef unsigned char val40[5];
/** @} */

/**
 * Structure for HAI communication handle. Do not access elements directly.
 *
 * @brief HAI Communication Handle
 */
typedef struct
{
    /** Open socket or serial device handle */
    int s;
    /** Structure containing network address of Omni */
    struct sockaddr_in omni;
    /** Next transmit sequence number */
    int tx_sequence;
    /** Next expected receive sequence number */
    int rx_sequence;
    /** Session ID */
    unsigned char session_id[5];
    /** Private key */
    unsigned char private_key[16];
    /** Flag to indicate serial mode */
    int serial_mode;
} hai_comm_id;

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif /* _HAI_COMM_H_ */

