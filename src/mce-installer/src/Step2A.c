/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_Step2A_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP2A);
	if (setting_netExtKeep) {
		displayStepAptMirror();
	} else {
		displayStep2E();
	}
}	

void on_Step2A_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		gboolean value = (gboolean) data;
		setting_netExtKeep = value;
	}
}

void displayStep2A(void) {
	printf("Step2A\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	gchar *message = g_strdup_printf("Your external network interface appears to be setup correctly, You have %d network cards, and the card <i>%s</i> is connected to your internet connection with this settings:", setting_netIfaceNo, setting_netExtName);
	GtkWidget *label = gtk_label_new_for_wizard("");
	gtk_label_set_markup(GTK_LABEL(label), message);
	g_free(message);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	GtkTable *tableNetwork = GTK_TABLE(gtk_table_new(6,2,FALSE));
	         // Configuration Label and Entry        
	         GtkWidget *labelConfiguration = gtk_label_new("Configuration:");
	         gtk_misc_set_alignment(GTK_MISC(labelConfiguration), 0, .5);
		 GtkWidget *labelConfigurationv;
		 if (setting_netExtUseDhcp) {
		         labelConfigurationv = gtk_label_new("Automatic Configuration (DHCP)");
		 } else {
		         labelConfigurationv = gtk_label_new("Static IP address");
		 }
		 gtk_misc_set_alignment(GTK_MISC(labelConfigurationv), 0, .5);
	
	         gtk_table_attach_defaults(tableNetwork, labelConfiguration, 0, 1, 0, 1);
	         gtk_table_attach_defaults(tableNetwork, labelConfigurationv, 1, 2, 0, 1);
	
	                 // IP Label and Entry
	         GtkWidget *labelIP = gtk_label_new("IP address:");
	         gtk_misc_set_alignment(GTK_MISC(labelIP), 0, .5);
	         GtkWidget *labelIPv = gtk_label_new(setting_netExtIP);
	         gtk_misc_set_alignment(GTK_MISC(labelIPv), 0, .5);
	         gtk_table_attach_defaults(tableNetwork, labelIP, 0, 1, 1, 2);
	         gtk_table_attach_defaults(tableNetwork, labelIPv, 1, 2, 1, 2);
	
	                 // Netmask Label and Entry
	         GtkWidget *labelNetmask = gtk_label_new("Subnet mask:");
	         gtk_misc_set_alignment(GTK_MISC(labelNetmask), 0, .5);
	         GtkWidget *labelNetmaskv = gtk_label_new(setting_netExtMask);
	         gtk_misc_set_alignment(GTK_MISC(labelNetmaskv), 0, .5);
	         gtk_table_attach_defaults(tableNetwork, labelNetmask, 0, 1, 2, 3);
	         gtk_table_attach_defaults(tableNetwork, labelNetmaskv, 1, 2, 2, 3);
	
	                 // Gateway Label and Entry
	         GtkWidget *labelGateway = gtk_label_new("Gateway address:");
	         gtk_misc_set_alignment(GTK_MISC(labelGateway), 0, .5);
	         GtkWidget *labelGatewayv = gtk_label_new(setting_netExtGateway);
	         gtk_misc_set_alignment(GTK_MISC(labelGatewayv), 0, .5);
	         gtk_table_attach_defaults(tableNetwork, labelGateway, 0, 1, 3, 4);
	         gtk_table_attach_defaults(tableNetwork, labelGatewayv, 1, 2, 3, 4);
	
	                 // DNS1 Label and Entry
	         GtkWidget *labelDNS1 = gtk_label_new("Primary DNS Server:");
	         gtk_misc_set_alignment(GTK_MISC(labelDNS1), 0, .5);
	         GtkWidget *labelDNS1v = gtk_label_new(setting_netExtDNS1);
	         gtk_misc_set_alignment(GTK_MISC(labelDNS1v), 0, .5);
	         gtk_table_attach_defaults(tableNetwork, labelDNS1, 0, 1, 4, 5);
	         gtk_table_attach_defaults(tableNetwork, labelDNS1v, 1, 2, 4, 5);
	
	        // DNS2 Label and Entry
	        GtkWidget *labelDNS2 = gtk_label_new("Secondary DNS Server:");
	        gtk_misc_set_alignment(GTK_MISC(labelDNS2), 0, .5);
	        GtkWidget *labelDNS2v = gtk_label_new(setting_netExtDNS2);
	        gtk_misc_set_alignment(GTK_MISC(labelDNS2v), 0, .5);
	        gtk_table_attach_defaults(tableNetwork, labelDNS2, 0, 1, 5, 6);
	        gtk_table_attach_defaults(tableNetwork, labelDNS2v, 1, 2, 5, 6);
	gtk_box_pack_start(GTK_BOX(mainBox), GTK_WIDGET(tableNetwork), TRUE, TRUE, 0);
	
	GtkWidget *label2 = gtk_label_new_for_wizard("\nDo you want to keep your current configuration for this card ?");
	gtk_box_pack_start(GTK_BOX(mainBox), label2, TRUE, TRUE, 0);
	 
	// Questions
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, keep current configuration for external network card,");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step2A_radio_toggled), (gpointer)TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioYes));
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, I’ll set my network options manually.");
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_Step2A_radio_toggled), (gpointer)FALSE);
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, FALSE, 0);

	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step2A_forward_clicked), NULL);

	if (setting_netExtKeep) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioNo), TRUE);
	}

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
