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
 * @file  hai_net.h
 *
 * @brief HAI Network Communication Header File
 *
 * Header file containing definitions for HAI network functions. The
 * implementation is based on the "HAI Network Communication Protocol
 * Description Rev A".
 */

#ifndef _HAI_NET_H_
#define _HAI_NET_H_

/* Includes */

#include "hai_comm.h"

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/* Typedefs */

/**
 * This enumeration is used to indicate the HAI packet type.
 */
typedef enum {
    /** Message type to request a new session */
    CLIENT_REQUEST_NEW_SESSION = 1,
    /** Message type to acknowledge a new session */
    CONTROLLER_ACKNOWLEDGE_NEW_SESSION = 2,
    /** Message type to request a secure connection */
    CLIENT_REQUEST_SECURE_CONNECTION = 3,
    /** Message type to acknowledge a secure connection */
    CONTROLLER_ACKNOWLEDGE_SECURE_CONNECTION = 4,
    /** Message type to terminate a session */
    CLIENT_SESSION_TERMINATED = 5,
    /** Message type to indicate the controller terminated the session */
    CONTROLLER_SESSION_TERMINATED = 6,
    /** Message type to indicate an error creating a new session */
    CONTROLLER_CANNOT_START_NEW_SESSION = 7,
#ifdef USE_TCP
    /** Message type to indicate an Omni-Link II message */
    OMNI_LINK_MESSAGE = 32
#else
    /** Message type to indicate an Omni-Link message */
    OMNI_LINK_MESSAGE = 16,
#endif
} hai_msg_type;

/* Function prototypes */

/**
 * This function opens an HAI network handle.
 *
 * @param   id          pointer to HAI communication ID
 * @param   ip_address  pointer to HAI Omni IP address
 * @param   port        IP port
 * @param   private_key pointer to key
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_net_open(hai_comm_id *id, const char *ip_address, int port,
	const unsigned char *private_key);
/**
 * This function closes an HAI network handle.
 *
 * @param   id          pointer to HAI communication ID (open in network mode)
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_net_close(hai_comm_id *id);
/**
 * This function sends a message via an open HAI network handle.
 *
 * @param   id          pointer to HAI communication ID (open in network mode)
 * @param   type        type of network packet
 * @param   msg         pointer to message buffer to send
 * @param   len         length (in bytes) of data to send
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_net_send_msg(hai_comm_id *id, hai_msg_type type, const void *msg,
    int len);
/**
 * This function receives a message via an open HAI network handle.
 *
 * @param   id          pointer to HAI communication ID (open in network mode)
 * @param   type        pointer to return type of network packet
 * @param   msg         pointer to buffer for return data
 * @param   len         pointer to length (in bytes) containing max length
 *                      of buffer on call and actual length of recv. data
 *                      on return
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref CommErrs "HAI Communication Error Codes".
 */
HAIIMPORT int hai_net_recv_msg(hai_comm_id *id, hai_msg_type *type, void *msg,
    int *len);
/**
 * This function return an error string associated with @a err.
 *
 * @param   err         error code
 *
 * @return Returns a text string which describes @a err.
 */
HAIIMPORT const char *hai_net_strerror(int err);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif /* _HAI_NET_H_ */

