/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_Step4_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP4);
	displayStepLinuxMceCD();
}

void on_Step4_finish_clicked(GtkWidget *widget, gpointer data) {

}

void on_Step4_radio_toggled(GtkWidget *widget, gpointer data) {
	setting_startupType = (gint)data;
}

void displayStep4(void) {
	if (setting_deviceType == DT_MEDIA_DIRECTOR ) {
		displayStepInstallDependencies();
		return;
	}

	printf("Step4\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("How do you plan to use this computer ?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radio[3];

	radio[0] = gtk_radio_button_new_with_label(group, "Primarily used as a PC -- Ubuntu starts up by default, and you can\nstart LinuxMCE manually when you need it.");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
	radio[1] = gtk_radio_button_new_with_label(group, "A dedicated LinuxMCE System -- Start LinuxMCE and you can start\nUbuntu manually when you need it.");
//	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
//	radio[2] = gtk_radio_button_new_with_label(group, "Both -- Start both Ubuntu and LinuxMCE.  LinuxMCE will appear by default,\nbut you can switch to Ubuntu by pressing Ctrl+Alt+F7");

	int i;
	for (i=0; i<2; i++) {
		g_signal_connect(G_OBJECT(radio[i]), "toggled", G_CALLBACK(on_Step4_radio_toggled), (gpointer)i);
		gtk_box_pack_start(GTK_BOX(mainBox), radio[i], TRUE, FALSE, i);
		if (setting_startupType == i) { 
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio[i]), TRUE);
		}
	}

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step4_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
