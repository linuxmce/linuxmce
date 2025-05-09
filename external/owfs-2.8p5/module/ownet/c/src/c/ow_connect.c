/*
$Id: ow_connect.c,v 1.9 2009/01/08 09:40:12 d1mag Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
    email: palfille@earthlink.net
    Released under the GPL
    See the header file: ow.h for full attribution
    1wire/iButton system from Dallas Semiconductor
*/

#include <config.h>
#include "owfs_config.h"
#include "ow.h"
#include "ow_connection.h"

/* Routines for handling a linked list of connections in and out */
/* typical connection in would be the serial port or USB */

/* Globals */
struct connection_in *head_inbound_list = NULL;
int count_inbound_connections = 0;

struct connection_in *find_connection_in(OWNET_HANDLE handle)
{
	struct connection_in *c_in;
	struct connection_in *c_ret = NULL;

	// step through head_inbound_list linked list
	for (c_in = head_inbound_list; c_in != NULL; c_in = c_in->next) {
		if (c_in->index == handle) {
			if (c_in->state == connection_active) {
				c_ret = c_in;
			}
			break;
		}
	}
	return c_ret;
}

enum bus_mode get_busmode(struct connection_in *in)
{
	if (in == NULL)
		return bus_unknown;
	return in->busmode;
}

int BusIsServer(struct connection_in *in)
{
	return (in->busmode == bus_server) || (in->busmode == bus_zero);
}

/* Make a new head_inbound_list, and place it in the chain */
/* Based on a shallow copy of "in" if not NULL */
struct connection_in *NewIn(void)
{
	struct connection_in *now;

	// step through head_inbound_list linked list
	for (now = head_inbound_list; now != NULL; now = now->next) {
		if (now->state == connection_vacant) {
#if OW_MT
			my_pthread_mutex_init(&(now->bus_mutex), Mutex.pmattr);
#endif							/* OW_MT */
			now->state = connection_pending;
			return now;
		}
	}
	now = malloc(sizeof(struct connection_in));
	if (now != NULL) {
		memset(now, 0, sizeof(struct connection_in));
		now->next = head_inbound_list;	/* put in linked list at start */
		head_inbound_list = now;
		now->index = count_inbound_connections++;
		now->state = connection_pending;
#if OW_MT
		my_pthread_mutex_init(&(now->bus_mutex), Mutex.pmattr);
#endif							/* OW_MT */
	}
	return now;
}

void FreeIn(struct connection_in *target)
{
	if (target == NULL) {
		return;
	}
	switch (target->state) {
	case connection_vacant:
		break;
	case connection_pending:
		if (target->name) {
			free(target->name);
			target->name = NULL;
		}
		break;
	case connection_active:
		switch (get_busmode(target)) {
		case bus_zero:
			if (target->connin.tcp.type)
				free(target->connin.tcp.type);
			if (target->connin.tcp.domain)
				free(target->connin.tcp.domain);
			if (target->connin.tcp.fqdn)
				free(target->connin.tcp.fqdn);
			// fall through
		case bus_server:
			LEVEL_DEBUG("FreeClientAddr\n");
			FreeClientAddr(target);
			break;
		default:
			break;
		}
		if (target->name) {
			free(target->name);
			target->name = NULL;
		}
#if OW_MT
		my_pthread_mutex_destroy(&(target->bus_mutex));
#endif
	}
	target->state = connection_vacant;
}

void FreeInAll( void )
{
	struct connection_in *next ;

	while ( head_inbound_list ) {
		next = head_inbound_list->next;
		//LEVEL_DEBUG("FreeInAll: %p next=%p\n", Inbound_Control.head, Inbound_Control.head->next);
		FreeIn(head_inbound_list);
		free(head_inbound_list);
		head_inbound_list = next;
	}
}

