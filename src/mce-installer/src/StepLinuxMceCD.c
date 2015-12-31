/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

void on_StepLinuxMceCD_forward_clicked(GtkWidget *widget, gpointer data)  {

	gboolean goNext = TRUE;

	if (setting_linuxmceCdFrom != FROM_NET) {
		goNext = FALSE;
		GtkWidget* runWindow;
		if (setting_linuxmceCdFrom == FROM_ISO) {
			runWindow = run_shell_command("./mce-installer-LinuxMceCD.sh", "Verifing if the iso path is correct", "There is no iso in specified path");
	                gtk_widget_show_all(runWindow);
		} else {
			runWindow = run_shell_command("./mce-installer-LinuxMceCD.sh", "Caching LinuxMCE Disc 1", "Failed to cache LinuxMCE Disc 1");
			gtk_widget_show_all(runWindow);
		}

		if (gtk_dialog_run(GTK_DIALOG(runWindow)) == 1) {
			goNext = TRUE;
		}
	}

	if (goNext) {
		g_queue_push_head(history, (gpointer)STEPLINUXMCECD);
	//	displayStep3();
		displayStepUbuntuExtraCD();
	}
}

void on_StepLinuxMceCD_file_set(GtkWidget *widget, gpointer data) {
	GtkWidget *radioUseISO = GTK_WIDGET(data);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioUseISO), TRUE);
	setting_linuxmceCdIsoPath = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
	printf("setting_linuxmceCdIsoPath = %s\n", setting_linuxmceCdIsoPath);
}

void on_StepLinuxMceCD_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
	setting_linuxmceCdFrom = (gint)data;
		printf("setting_linuxmceCdFrom = %d\n", (gint)data);
	}
}

void displayStepLinuxMceCD(void) {
	printf("StepLinuxMceCD\n");
	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Please insert the LinuxMCE Disc 1 in the drive or locate the iso image on your hard drive");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Radio button install using CD
	GSList *group = NULL;
	GtkWidget *radioUseCD = gtk_radio_button_new_with_label(group, "It's in the CD drive.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseCD, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseCD), "toggled", G_CALLBACK(on_StepLinuxMceCD_radio_toggled), (gpointer)FROM_CD);
	if (setting_linuxmceCdFrom == FROM_CD) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioUseCD), TRUE);
	}

	// Radio button install using ISO
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioUseCD));
	GtkWidget *radioUseISO = gtk_radio_button_new_with_label(group, "I have an iso image on my harddrive.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseISO, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseISO), "toggled", G_CALLBACK(on_StepLinuxMceCD_radio_toggled), (gpointer)FROM_ISO);
	if (setting_linuxmceCdFrom == FROM_ISO) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioUseISO), TRUE);
	}
	
/*	// Radio button install from Internet
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioUseCD));
	GtkWidget *radioUseNet = gtk_radio_button_new_with_label(group, "I don't have it. Download packages from the Internet.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseNet, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseNet), "toggled", G_CALLBACK(on_StepLinuxMceCD_radio_toggled), (gpointer)FROM_NET);
	if (setting_linuxmceCdFrom == FROM_NET) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioUseNet), TRUE);
	}
*/
	// File chooser button
	GtkWidget *fileChooser = gtk_file_chooser_button_new("Choose ISO image for LinuxMCE Disc 1", GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_box_pack_start(GTK_BOX(mainBox), fileChooser, TRUE, TRUE, 0);
	if (setting_linuxmceCdIsoPath) {
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(fileChooser), setting_linuxmceCdIsoPath);
	}
	g_signal_connect(G_OBJECT(fileChooser), "selection-changed", G_CALLBACK(on_StepLinuxMceCD_file_set), radioUseISO);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepLinuxMceCD_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
