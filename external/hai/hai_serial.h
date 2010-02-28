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
 * @file  hai_serial.h
 *
 * @brief HAI Serial Communication Header File
 *    
 * Header file containing definitions for HAI serial functions.
 */

#ifndef _HAI_SERIAL_H_
#define _HAI_SERIAL_H_

/* Includes */

#include "hai_comm.h"

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/* Function prototypes */

#ifdef HAISERIAL_SUPPORT

/**
 * This function opens an HAI serial handle.
 *
 * @param   id          pointer to HAI communication ID
 * @param   dev         dev path for serial port
 * @param   baud        baud rate
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_serial_open(hai_comm_id *id, const char *dev, int baud);
/**
 * This function closes an HAI serial handle.
 *
 * @param   id          pointer to HAI communication ID (open in serial mode)
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_serial_close(const hai_comm_id *id);
/**
 * This function sends a message via an open HAI serial handle.
 *
 * @param   id          pointer to HAI communication ID (open in serial mode)
 * @param   msg         pointer to message buffer to send
 * @param   len         length (in bytes) of data to send
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_serial_send_msg(const hai_comm_id *id, const void *msg, int len);
/**
 * This function receives a message via an open HAI serial handle.
 *
 * @param   id          pointer to HAI communication ID (open in serial mode)
 * @param   msg         pointer to buffer for return data
 * @param   len         pointer to length (in bytes) containing max length
 *                      of buffer on call and actual length of recv. data
 *                      on return
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_serial_recv_msg(const hai_comm_id *id, void *msg, int *len);

#endif

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif /* _HAI_SERIAL_H_ */

