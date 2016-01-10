/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include <stdio.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

#define MAX_PORTS 4
#define MAX_RESETS 5

raw1394handle_t 	handle = NULL;
dc1394_cameracapture	camera;
nodeid_t		*camera_nodes;

int main(void) {
	struct raw1394_portinfo ports[MAX_PORTS];
	int numPorts = MAX_PORTS;
	int port, reset;
	int camCount = 0;
	int found[MAX_PORTS];

	// Get the number of ports (cards)
	handle = raw1394_new_handle();
	if ( handle == NULL ) {
		fprintf(stderr, "ERROR: Unable to aquire raw1394 handle, posible driver not loaded.\n");
		exit (-1);
	}
	numPorts = raw1394_get_port_info(handle, ports, numPorts);
	raw1394_destroy_handle(handle);
	handle = NULL;

	for (reset = 0; reset < MAX_RESETS; reset++) {
		// We'll try to get a handle on each firewire port
		for (port = 0; port < numPorts; port++) {

			if (found[port] == 1) {
				continue;
			}
			
			if (handle != NULL) {
				dc1394_destroy_handle(handle);
			}
			handle = dc1394_create_handle(port);

			if (handle == NULL) {
				fprintf(stderr, "ERROR: Unable to aquirte dc1394 handle for port %d, are the drivers still loaded ?", port);
				exit (-1);
			}

		
			// And search for firewire cammeras on them	
			camCount = 0;
			camera_nodes = dc1394_get_camera_nodes(handle, &camCount, 0);

			if (camCount > 0) {
				dc1394_camerainfo info;
				
				camera.node = camera_nodes[0];

				if (dc1394_get_camera_info(handle, camera_nodes[0], &info) == DC1394_SUCCESS) {
					found[port] = 1;
					
					// camera can not be root--highest order node */
					if (camera.node == raw1394_get_nodecount(handle)-1) {
						// reset and retry
						raw1394_reset_bus(handle);
						sleep(2);
						found[port] = 0;
					}
				}
				
				if ( found[port] == 1 ) {
					printf("\n\n*** Found a firewire cammera on port : %d\n", port);
					dc1394_print_camera_info(&info);
				}

				dc1394_free_camera_nodes(camera_nodes);
			}

		}
	}
}
