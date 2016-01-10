/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

void on_Step1D_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		gint value = (gint) data;
		printf("Setting to %d\n", value);
		setting_deviceType = value;
	}
}

void on_Step1D_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP1D);

	gtk_widget_set_sensitive (GTK_WIDGET(widget), FALSE);
	start_network_wizard();
}

void displayStep1D(void) {
	printf("Step1D\n");

	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("You can add media players throughout the home wherever you want media. For very large homes with lots of media players, it might be best to answer ‘No’ and dedicate this Core to only providing the back-end services. \n\nDo you also want to use this Core to act as a media player?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

        // Questions
        GSList *group = NULL;
        GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, install all the media playback and remote control\nsoftware and make this computer my first media player.");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
        g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step1D_radio_toggled), (gpointer)DT_HYBRID);
        gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

        group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioYes));
        GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, i'll use other computer(s) and DMA devices(s) to playback\nmedia. Only install core software on this one.");
        g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_Step1D_radio_toggled), (gpointer)DT_CORE);
        gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, FALSE, 0);

        if (setting_deviceType == 1) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioNo), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	}

	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step1D_forward_clicked), NULL);	
		
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
