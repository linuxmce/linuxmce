/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


GtkWidget *buttonFinish;

void on_Step2F_finish_clicked(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}

void displayStep2F(void) {
	printf("Step2F\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("You'll need to add at least one network card on this computer before intalling Linux MCE.");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Button Finish
	buttonFinish = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonFinish);
	g_signal_connect(G_OBJECT(buttonFinish), "clicked", G_CALLBACK(on_Step2F_finish_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
