/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"
#include "Step1A.h"


void on_Step1A_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP1A);

	if (setting_deviceType != DT_MEDIA_DIRECTOR) {
		displayStep1D();
	} else {
		displayStep1B();
	}
}



void on_Step1A_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		gint value = (gint) data;
		printf("Setting deviceType %d\n", value);
		setting_deviceType = value;
	}
}

void displayStep1A(void) {
	printf("Step1A\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("You need to have one system in the home that will be designated as the ‘Master’, or ‘Core’.  This system should be left on all the time because it provides vital services to the home, such as phone service, and because any of the media players in the various rooms in your home need to talk to the Core to coordinate their activities.\n\nI could not find any Core on your network, so I assume you want this system to be your Core?  Is that correct?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, i want to use this system as a Core");
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step1A_radio_toggled), (gpointer)DT_HYBRID);
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioYes));
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, i want a Media Director");
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_Step1A_radio_toggled), (gpointer)DT_MEDIA_DIRECTOR);
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, FALSE, 0);

	if (setting_deviceType == 3) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioNo), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	}
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step1A_forward_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
