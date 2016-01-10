/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


GtkWidget *buttonFinish;

void on_Step6_finish_clicked(GtkWidget *widget, gpointer data) {
	if (setting_ubuntuLiveCdFrom == FROM_CD){
		system("eject");
	}
	gtk_main_quit();
}


void displayStep6(void) {
	printf("Step6\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label;
	if (access("/tmp/mce_installer_error", F_OK) == 0) {
		label = gtk_label_new_for_wizard ("Installation Failed :");
		gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

		gint fdError = open("/tmp/mce_installer_error", O_RDONLY);
		gchar errorMessage[1024];
		memset(errorMessage, 0, 1024);
		read(fdError, errorMessage, 1023);
		label = gtk_label_new_for_wizard (errorMessage);
		gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
		close (fdError);

		label = gtk_label_new_for_wizard ("If you want to send a bug report on this, don't forget to include the contents of /var/log/mce_installer_*.log files.");
		gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	} else {
		if (setting_ubuntuLiveCdFrom == FROM_CD) {
			label = gtk_label_new_for_wizard ("Installation is finished. Before reboot please remove Kubuntu Live CD after hitting 'Close' button. \n\nNOTE: The first time you start LinuxMCE it will take several minutes to verity the new installation and generate the UI’s");
                        gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
		} else {
			label = gtk_label_new_for_wizard ("Installation is finished.  You should now reboot. \n\nNOTE: The first time you start LinuxMCE it will take several minutes to verity the new installation and generate the UI’s");
			gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
		}
	}

	
	// Button Finish
	buttonFinish = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonFinish);
	g_signal_connect(G_OBJECT(buttonFinish), "clicked", G_CALLBACK(on_Step6_finish_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
