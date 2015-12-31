/*
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"
#include <unistd.h>
#include <sys/types.h>
 
void start_network_wizard(void) {
	// Display wait message
	cleanupContainer(mainBox); 
        cleanupContainer(mainButtonBox); 
			 
	GtkWidget *label = gtk_label_new_for_wizard ("Detecting current network settings, please wait ...");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);

	int hasInternet = 0;

	GtkWidget *runWindow =  run_shell_command("./mce-installer-getnetdata.sh", "Detecting Network Settings", "");
	gtk_widget_show_all(runWindow);
	gtk_dialog_run(GTK_DIALOG(runWindow));

	gchar readbuffer[1024];
	int size=0;
	memset(readbuffer, 0, 1024);

	int output_fd = open("/tmp/get_net_data_output", 0);
	if (output_fd > 0) {
		size = read(output_fd, readbuffer, 1023);
		readbuffer[size-1] = '\0';
	} else {
		size = 0;
	}

	if(size==0 || g_ascii_strcasecmp(readbuffer, "0")==0) {
		printf("No network cards detected\n");
		setting_netIfaceNo=0;

	} else {
		gchar **line = g_strsplit(readbuffer, "\n", 3);

		gchar **tokens;
		setting_netIfaceNo = g_ascii_strtoll(line[0], NULL, 10);

		tokens = g_strsplit(line[1], "|", 2);
		setting_netExtName = tokens[0];
		setting_netIntName = tokens[1];

		// Get external interface settings
		tokens = g_strsplit(line[2], "|", 7);
		setting_netExtIP = tokens[0];
		setting_netExtMask = tokens[1];
		setting_netExtGateway = tokens[2];
		setting_netExtDNS1 = tokens[3];
		setting_netExtDNS2 = tokens[4];
		setting_netExtUseDhcp = g_ascii_strtoll(tokens[5], NULL, 10);
		hasInternet = g_ascii_strtoll(tokens[6], NULL, 10);
	}


	if (setting_netIfaceNo == 0) {
	       displayStep2F();	
	} else {
		if (hasInternet) {
			displayStep2A();
		} else {
			displayStep2C();
		}
	}
}

