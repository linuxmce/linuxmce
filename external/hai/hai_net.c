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

/* Includes */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "hai_net.h"
#include "aes.h"

/* Defines */

#define HAI_NET_TIMEOUT         3

#ifndef WIN32
#define SOCKET                  int
#endif

/* Typedefs */

typedef struct
{
    val16 sequence;
    val8 type;
    val8 resv0;
} hai_msg_header;

typedef struct
{
     /* val16 protocol_ver; */
    val8 protocol_ver1;
    val8 protocol_ver2;
    val40 session_id;
} hai_ack_new_session;

/* Function prototypes */

static int hai_net_send_unsec_msg(hai_comm_id *id, hai_msg_type type,
    const void *msg, int len);
static int hai_net_recv_unsec_msg(hai_comm_id *id, hai_msg_type *type,
    void *msg, int *len);

/********************************************************************************/

/* Function to open network connection */
HAIEXPORT int hai_net_open(hai_comm_id *id, const char *ip_address, int port,
    const unsigned char *private_key)
{
    struct sockaddr_in local;
    struct hostent *hostp;
    hai_ack_new_session ack_msg;
    char buffer[1024];
    int err, len, i, addr;
    hai_msg_type type;

    /* Check arguments */
    if ((id == NULL) || (ip_address == NULL) || (private_key == NULL))
        return EHAIARGUMENT;

	/* Check ip address */
	if (strlen(ip_address) == 0)
        return EHAIARGUMENT;

    /* Set connection type */
    id->serial_mode = 0;

    /* Lookup address */
    if ((hostp = gethostbyname(ip_address)) != NULL)
        addr = *((int*) hostp->h_addr);
    else
        addr = inet_addr(ip_address);

    /* Setup omni address */
    memset((char*) &(id->omni), 0, sizeof(id->omni));
    id->omni.sin_family = AF_INET;
    id->omni.sin_addr.s_addr = addr;
    id->omni.sin_port = htons((unsigned short) port);

    /* Reset sequence */
    id->tx_sequence = 1;

    /* Open socket */
    if ((id->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        return errno;
    /* Bind socket */
    memset((char*) &local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(INADDR_ANY);
    if(bind(id->s,(struct sockaddr*) &local, sizeof(local)) < 0)
        return errno;

    /* Send new session request */
    if ((err = hai_net_send_unsec_msg(id, CLIENT_REQUEST_NEW_SESSION, NULL, 0))
            != 0)
        return err;

    /* Wait for response */
    len = sizeof(ack_msg);
    if ((err = hai_net_recv_unsec_msg(id, &type, &ack_msg, &len)) != 0)
        return err;
    if (type != CONTROLLER_ACKNOWLEDGE_NEW_SESSION)
        return EHAIRESPONSE;

    /* Save session ID and update private key */
    memcpy(id->private_key, private_key, 16);
    for (i = 0; i < 5; i++)
    {
        id->session_id[i] = ack_msg.session_id[i];
        id->private_key[i + 11] ^= ack_msg.session_id[i];
    }

    /* Send secure connection request */
    if ((err = hai_net_send_msg(id, CLIENT_REQUEST_SECURE_CONNECTION,
            id->session_id, 5)) != 0)
        return err;

    /* Wait for response */
    if ((err = hai_net_recv_msg(id, &type, buffer, &len)) != 0)
        return err;

    /* Test result */
    if (type != CONTROLLER_ACKNOWLEDGE_SECURE_CONNECTION)
        return EHAIRESPONSE;
    for (i =0; i < 5; i++)
    {
        if (id->session_id[i] != (unsigned char) buffer[i])
            return EHAISESSION;
    }

    return 0;
}

/********************************************************************************/

/* Function to close network connection */
HAIEXPORT int hai_net_close(hai_comm_id *id)
{
    int err = 0;
    hai_msg_type type;

	/* Check file handle */
	if (id->s == 0)
		return EHAISESSION;

    /* Send client session terminated */
    if ((err = hai_net_send_unsec_msg(id, CLIENT_SESSION_TERMINATED, NULL, 0))
            != 0)
        goto exit_err;

    /* Wait for response */
    if ((err = hai_net_recv_unsec_msg(id, &type, NULL, NULL)) != 0)
        goto exit_err;
    if (type != CONTROLLER_SESSION_TERMINATED)
        err = EHAIRESPONSE;

    /* Close socket */
exit_err:
    close (id->s);

    return err;
}

/********************************************************************************/

/* Function to send a secure message */
HAIEXPORT int hai_net_send_msg(hai_comm_id *id, hai_msg_type type, const void *msg,
    int len)
{
    char buffer[1024];
    char sec_buffer[1024];
    int tx_len, err, i;
    aes_t aes;

    /* Calc data lenth rounded up to next 16 byte count */
    tx_len = (len + 15) & ~0xF;

    /* Copy data */
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, msg, len);

    /* XOR data */
    for (i = 0; i < (tx_len / 16); i++)
    {
        buffer[0 + (16 * i)] ^= (id->tx_sequence >> 8) & 0xFF;
        buffer[1 + (16 * i)] ^= (id->tx_sequence) & 0xFF;
    }

    /* Encrypt message */
    memset(sec_buffer, 0, sizeof(sec_buffer));
    MakeKey(&aes, (char *)(id->private_key), "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
    Encrypt(&aes, buffer, sec_buffer, tx_len, ECB);

    /* Send message */
    if ((err = hai_net_send_unsec_msg(id, type,
            sec_buffer, tx_len)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

/* Function to receive a secure message */
HAIEXPORT int hai_net_recv_msg(hai_comm_id *id, hai_msg_type *type, void *msg,
    int *len)
{
    char sec_buffer[1024];
    char buffer[1024];
    int cnt, err, rx_len, i;
    aes_t aes;

    /* Wait for response */
    cnt = 1024;
    memset(sec_buffer, 0, sizeof(sec_buffer));
    if ((err = hai_net_recv_unsec_msg(id, type, sec_buffer, &cnt)) != 0)
        return err;

    /* Calc data lenth rounded up to next 16 byte count */
    rx_len = (cnt + 15) & ~0xF;

    /* Fill in results */
    if (len != NULL)
    {
        if (*len < cnt)
            cnt = *len;
        else
            *len = cnt;
    }

    if ((cnt != 0) && (msg != NULL))
    {
        /* Decrypt message */
        memset(buffer, 0, sizeof(buffer));
        MakeKey(&aes, (char *)(id->private_key), "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
        Decrypt(&aes, sec_buffer, buffer, rx_len, ECB);

        /* XOR data */
        for (i = 0; i < (rx_len / 16); i++)
        {
            buffer[0 + (16 * i)] ^= (id->rx_sequence >> 8) & 0xFF;
            buffer[1 + (16 * i)] ^= (id->rx_sequence) & 0xFF;
        }

        /* Copy message */
        memcpy(msg, buffer, cnt);
    }

    return 0;
}

/********************************************************************************/

HAIEXPORT const char *hai_net_strerror(int err)
{
    switch (err)
    {
        case EHAIARGUMENT :
            return "Bad HAI communication function argument";
        case EHAIRESPONSE :
            return "Unexpected HAI response";
        case EHAISESSION :
            return "Bad session ID from HAI";
        case EHAITIMEOUT :
            return "Network time-out";
    }

    return NULL;
}

/********************************************************************************/

/* Function to send an unsecure message */
static int hai_net_send_unsec_msg(hai_comm_id *id, hai_msg_type type,
    const void *msg, int len)
{
    char buffer[1024];
    hai_msg_header *header = (hai_msg_header*) &buffer;
    int packet_len = sizeof(hai_msg_header) + len;

	/* Check file handle */
	if (id->s == 0)
		return EHAISESSION;

    /* Prepare packet */
    SET16(header->sequence, id->tx_sequence);
    SET8(header->type, type);
    SET8(header->resv0, 0);
    if ((len != 0) && (msg != NULL))
        memcpy(buffer + sizeof(hai_msg_header), msg, len);

    /* Send packet */
    if (sendto(id->s, buffer, packet_len, 0, (struct sockaddr *) &(id->omni),
            sizeof(id->omni)) != packet_len)
        return errno;

    /* Increment sequence */
    if (id->tx_sequence < 65535)
        id->tx_sequence++;
    else
        id->tx_sequence = 1;

    return 0;
}

/********************************************************************************/

/* Function to receive an unsecure message */
static int hai_net_recv_unsec_msg(hai_comm_id *id, hai_msg_type *type,
    void *msg, int *len)
{
    char buffer[1024];
    hai_msg_header *header = (hai_msg_header*) &buffer;
    int cnt, recv_len, retval;
    struct timeval tv;
    fd_set rfds;

	/* Check file handle */
	if (id->s == 0)
		return EHAISESSION;

    /* Wait for response */
    FD_ZERO(&rfds);
    FD_SET((SOCKET)(id->s), &rfds);
    tv.tv_sec = HAI_NET_TIMEOUT;
    tv.tv_usec = 0;
    retval = select(id->s + 1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
        return errno;
    else if (retval == 0)
        return EHAITIMEOUT;

    /* Read packet */
    cnt = recvfrom(id->s, buffer, 1024, 0, NULL, NULL);
    id->rx_sequence = GET16(header->sequence);

    /* Fill in results */
    recv_len = cnt - sizeof(hai_msg_header);
    if (type != NULL)
        *type = GET8(header-> type);
    if (len != NULL)
    {
        if (*len < recv_len)
            recv_len = *len;
        else
            *len = recv_len;
    }
    if ((recv_len != 0) && (msg != NULL))
        memcpy(msg, buffer + sizeof(hai_msg_header), recv_len);

    return 0;
}

