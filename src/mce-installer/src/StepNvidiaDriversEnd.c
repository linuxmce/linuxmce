/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_NvidiaDriversEnd_ok_clicked(GtkWidget *widget, gpointer data)  {
	gtk_main_quit();
}


void displayStepNvidiaDriversEnd(void) {

	printf("NvidiaDriversEnd\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("The driver has been changed, and the installer will now exit.");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_NvidiaDriversEnd_ok_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
