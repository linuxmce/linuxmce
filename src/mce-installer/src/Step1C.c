/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"
#include <glib/gprintf.h>

void on_Step1C_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		gint value = (gint) data;
		printf("Setting deviceType =  %d\n", value);
		setting_deviceType = value;
	}
}

void on_Step1C_forward_clicked(GtkWidget *widget, gpointer data) {

	g_queue_push_head(history, (gpointer)STEP1C);
        if (setting_deviceType != 1) {
		displayStepCreateDiskedDevice();
        } else {
                displayStep1D();
        }
}


void displayStep1C(void) {
	printf("Step1C\n");

	setting_deviceType = 3;

	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	gchar *label_message = g_strdup_printf("I see that you already have a Core in this home at the IP address: %s.  You only need one Core in the home.\n\nI assume this will be a media player coordinated by the core. Is this correct?", setting_coreIP);
	GtkWidget *label = gtk_label_new_for_wizard (label_message);
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

        // Questions
        GSList *group = NULL;
        GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, i want to use this system as a Media Director");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
        g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step1C_radio_toggled), (gpointer)DT_MEDIA_DIRECTOR);
        gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

        group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioYes));
        GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, i want to install new core (Not Recommanded)");
        g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_Step1C_radio_toggled), (gpointer)DT_CORE);
        gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, FALSE, 0);


	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step1C_forward_clicked), NULL);
			
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
