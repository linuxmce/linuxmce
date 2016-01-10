/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_Step0A_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP0A);

	if (setting_installType == INSTALL_TYPE_ISO) {
		displayStep0B();
	} else {
		displayStep3();
	}
}



void on_Step0A_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		setting_installType = (int)data;
	}
}

void displayStep0A(void) {
	printf("Step0A\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	GtkWidget *label = gtk_label_new_for_wizard ("Please insert the LinuxMCE Install CD");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

	// NEXT OPTIONS ARE DISABLED SINCE HITING THE FRAGILE LINUXMCE SERVER TO OFTER WOULD CAUSE IT TO GO DOWN
	// Wizard text
/*	GtkWidget *label = gtk_label_new_for_wizard ("What method do you want to use to install Linux MCE ?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radioOne = gtk_radio_button_new_with_label(group, "Download the needed packages form the Internet.");
	g_signal_connect(G_OBJECT(radioOne), "toggled", G_CALLBACK(on_Step0A_radio_toggled), (gpointer)INSTALL_TYPE_NET);
	gtk_box_pack_start(GTK_BOX(mainBox), radioOne, TRUE, FALSE, 0);
	if (setting_installType == INSTALL_TYPE_NET) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioOne), TRUE);
	}	
	
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioOne));
	GtkWidget *radioTwo = gtk_radio_button_new_with_label(group, "Download and use an ISO from the one of LinuxMCE mirrors.");
	g_signal_connect(G_OBJECT(radioTwo), "toggled", G_CALLBACK(on_Step0A_radio_toggled), (gpointer)INSTALL_TYPE_ISO);
	gtk_box_pack_start(GTK_BOX(mainBox), radioTwo, TRUE, FALSE, 0);
	if (setting_installType == INSTALL_TYPE_ISO) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioTwo), TRUE);
	}	

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioOne));
	GtkWidget *radioThree = gtk_radio_button_new_with_label(group, "Use the Linux MCE CDROM that i allready have in my drive.");
	g_signal_connect(G_OBJECT(radioThree), "toggled", G_CALLBACK(on_Step0A_radio_toggled), (gpointer)INSTALL_TYPE_CD);
	gtk_box_pack_start(GTK_BOX(mainBox), radioThree, TRUE, FALSE, 0);
	if (setting_installType == INSTALL_TYPE_CD) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioThree), TRUE);
	}	
*/


	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step0A_forward_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
