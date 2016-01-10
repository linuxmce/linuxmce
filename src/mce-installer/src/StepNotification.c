/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

void on_StepNotification_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEPNOTIFICATION);
	displayStep1D();
}

void displayStepNotification(void) {
	printf("StepNotification\n");

	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("LinuxMCE includes some custom kernel modules and other components that Kubuntu's update mechanism doesn't handle and will break.  So Kubuntu's updates have been disabled.\nLinuxMCE has its own upgrade system which will keep both Kubuntu and LinuxMCE current.   You will be notified when upgdates are available.");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Button Back
	/*GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	*/

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepNotification_forward_clicked), NULL);	
		
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
