/*
    SDP lookup routine for the PlutoMO rfcomm port 

    Copyright (C) 2008 Harald Klein <hari@vt100.at>

    This program is distributed according to the terms of the Pluto Public License, available at:
    http://plutohome.com/index.php?section=public_license

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
*/

#include "sdpbrowse.h"
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

int sdp_lookup_pluto_rfcomm_channel(bdaddr_t target)
{
    uuid_t svc_uuid;
    int err;
    sdp_list_t *response_list = NULL, *search_list, *attrid_list;
    sdp_session_t *session = 0;
    char servicename[512];
    servicename[0] = 0;

    // the service name we are searching for
    char *searchstring = "Pluto";

    // default to channel 19, thats used by the symbian orbiter
    int rfcomm_channel = 19;


    // connect to the SDP server running on the remote machine
    session = sdp_connect(BDADDR_ANY, &target, SDP_RETRY_IF_BUSY);

    if (session != NULL) {
        // specify the UUID of the public browse group
        sdp_uuid16_create(&svc_uuid, PUBLIC_BROWSE_GROUP);

	// search_list = sdp_list_append( NULL, &svc_uuid );
	search_list = sdp_list_append(NULL, &svc_uuid);

	// specify that we want a list of all the matching applications' attributes
	uint32_t range = 0x0000ffff;
	attrid_list = sdp_list_append(NULL, &range);

	// get a list of service records of the public browse group
	err = sdp_service_search_attr_req(session, search_list,
					  SDP_ATTR_REQ_RANGE, attrid_list,
					  &response_list);

	sdp_list_t *r = response_list;

	// go through each of the service records
	for (; r; r = r->next) {
	    sdp_record_t *rec = (sdp_record_t *) r->data;
	    sdp_list_t *proto_list;

	    // get a list of the protocol sequences
	    if (sdp_get_access_protos(rec, &proto_list) == 0) {
	        sdp_list_t *p = proto_list;

		// go through each protocol sequence
		for (; p; p = p->next) {
		    sdp_list_t *pds = (sdp_list_t *) p->data;

		    // go through each protocol list of the protocol sequence
		    for (; pds; pds = pds->next) {

		        // check the protocol attributes
		        sdp_data_t *d = (sdp_data_t *) pds->data;
			int proto = 0;
			for (; d; d = d->next) {
			    switch (d->dtd) {
			    case SDP_UUID16:
			    case SDP_UUID32:
			    case SDP_UUID128:
			        proto = sdp_uuid_to_proto(&d->val.uuid);
				break;
			    case SDP_UINT8:
			        if (proto == RFCOMM_UUID) {
				    sdp_get_service_name(rec, servicename,
							 sizeof(servicename));
				    if (strcmp(servicename, searchstring) == 0) {
				        printf("FOUND SERVICE PlutoMO: %i\n",(int) d->val.int8);
					rfcomm_channel = (int) d->val.int8;
				    }
				}
				break;
			    }
			}
		    }
		    sdp_list_free((sdp_list_t *) p->data, 0);
		}
		sdp_list_free(proto_list, 0);
	    
	    }
	    // printf("found service record 0x%x\n", rec->handle);
	    sdp_record_free(rec);
	}

	sdp_close(session);
    } else {
        printf("Did not get session from sdp_connect!!!");
    }
    return rfcomm_channel;
}
