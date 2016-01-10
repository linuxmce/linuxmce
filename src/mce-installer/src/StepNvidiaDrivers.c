/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_NvidiaDrivers_forward_clicked(GtkWidget *widget, gpointer data)  {

	gboolean goNext = FALSE;
	gboolean intallNvidiaDrivers = * (gboolean *)data;

	if (intallNvidiaDrivers == TRUE) {			
		GtkWidget* runWindow = run_shell_command("./mce-installer-NvidiaDrivers.sh", "Installing Nvidia Propetary Drivers", "Failed to install nvidia propetary drivers");
		gtk_widget_show_all(runWindow);
		if (gtk_dialog_run(GTK_DIALOG(runWindow)) == 1) {
			goNext = TRUE;
		}

		if (goNext) {
			g_queue_push_head(history, (gpointer)STEPNVIDIADRIVERS);
			displayStepNvidiaDriversEnd();
		}
	} else {
		switch(setting_deviceType) {
			case DT_CORE:
			case DT_HYBRID:
				g_queue_push_head(history, (gpointer)STEPNVIDIADRIVERS);
				start_network_wizard();	
				break;
			case DT_MEDIA_DIRECTOR:
				g_queue_push_head(history, (gpointer)STEPNVIDIADRIVERS);
				displayStepCreateDiskedDevice();
				break;
		}
	}

}

void on_NvidiaDrivers_radio_no_toggled(GtkWidget *widget, gpointer data) {
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gboolean *) data = FALSE;
	}
}

void on_NvidiaDrivers_radio_yes_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gboolean *) data = TRUE;
	}
}


void displayStepNvidiaDrivers(void) {

	printf("NvidiaDrivers\n");

	gboolean* intallNvidiaDrivers;
	intallNvidiaDrivers = (gboolean *)malloc(sizeof(gboolean));
	*intallNvidiaDrivers = TRUE;

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("You have an nVidia Card, but are not using the nVidia proprietary video driver.  Only that driver works well with LinuxMCE and nVidia cards.  Shall I switch to it now?\n\nNote 1: This will reboot the system and you will need to restart the installer\n\nNote 2: Insert Linux MCE CD 1 if you don't have a working internet connection");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);


	// Radio : Yes
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, I want it installed.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_NvidiaDrivers_radio_yes_toggled), (gpointer)intallNvidiaDrivers);

	// Radio : No
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioYes));
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, I want to use the opensource drivers without 3d acceleration.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_NvidiaDrivers_radio_no_toggled), (gpointer)intallNvidiaDrivers);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_NvidiaDrivers_forward_clicked), (gpointer)intallNvidiaDrivers);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
