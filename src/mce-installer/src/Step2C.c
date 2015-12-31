/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

static gboolean retryNetworkWizard = FALSE;

void on_Step2C_forward_clicked(GtkWidget *widget, gpointer data) {
	if (retryNetworkWizard) {
	start_network_wizard();
	} else {
		g_queue_push_head(history, (gpointer)STEP2C);
		displayStep2E();
	}
}

void on_Step2C_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		gboolean value = (gboolean) data;
		retryNetworkWizard = value;
	}
}

void displayStep2C(void) {
	printf("Step2C\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard("WARNING:\n\n I could not connect to the internet using you current network setup, this probably means that you internet connection is broken or that you haven't configured it until now.\nIf is a hardware problem (cable unpluged, ..) fix it now and select the coresponding option before pressing 'Forward' and i'll check again.");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Configure internet connection manually.");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step2C_radio_toggled), (gpointer)FALSE);
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioYes));
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "Try again, i just fixed my network connection.");
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_Step2C_radio_toggled), (gpointer)TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, FALSE, 0);

	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step2C_forward_clicked), NULL);

	if (retryNetworkWizard) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioNo), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	}

	// Redraw window
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
