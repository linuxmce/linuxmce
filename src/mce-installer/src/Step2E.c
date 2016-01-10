/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

static GtkWidget *entryIP;
static GtkWidget *entryNetmask;
static GtkWidget *entryGateway;
static GtkWidget *entryDNS1;
static GtkWidget *entryDNS2;

void on_Step2E_forward_clicked(GtkWidget *widget, gpointer data) {
	setting_netExtIP      = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryIP)));
	setting_netExtMask    = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryNetmask)));
	setting_netExtGateway = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryGateway)));
	setting_netExtDNS1    = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryDNS1)));
	setting_netExtDNS2    = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryDNS2)));

	g_queue_push_head(history, (gpointer)STEP2E);
	displayStepAptMirror();
}

void foreach_table_child_set_active(GtkWidget *widget, gpointer data) {
	if (GTK_IS_ENTRY(widget)) {
		gtk_entry_set_editable (GTK_ENTRY(widget), (gboolean)data);
		gtk_widget_set_sensitive (GTK_WIDGET(widget), (gboolean)data);
	}
}

void toggle_table_fields(GtkWidget *widget, gboolean data) {
	gtk_container_foreach(GTK_CONTAINER(widget), (GtkCallback)foreach_table_child_set_active, (gpointer)data);
}

void on_Step2E_combo_changed(GtkWidget *widget, gpointer data) {
	GtkWidget *tableNetwork = GTK_WIDGET(data);

	if (gtk_combo_box_get_active(GTK_COMBO_BOX(widget)) == 0) {
		setting_netExtUseDhcp = TRUE;
		toggle_table_fields(tableNetwork, FALSE);
	} else {
		setting_netExtUseDhcp = FALSE;
		toggle_table_fields(tableNetwork, TRUE);
	}
}

void displayStep2E(void) {
	printf("Step2E\n");
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Manual Network Configuration.");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	GtkTable *tableNetwork = GTK_TABLE(gtk_table_new(6,2,FALSE));
		// Configuration Label and Entry	
		GtkWidget *labelConfiguration = gtk_label_new("Configuration:");
		gtk_misc_set_alignment(GTK_MISC(labelConfiguration), 0, .5);

		GtkWidget *comboConfiguration = gtk_combo_box_new_text();
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboConfiguration), "Automatic Configuration (DHCP)");
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboConfiguration), "Static IP address");
		if (setting_netExtUseDhcp) {
			gtk_combo_box_set_active(GTK_COMBO_BOX(comboConfiguration), 0);
		} else {
			gtk_combo_box_set_active(GTK_COMBO_BOX(comboConfiguration), 1);
		}

		g_signal_connect(G_OBJECT(comboConfiguration), "changed", G_CALLBACK(on_Step2E_combo_changed), (gpointer)tableNetwork);
	
		gtk_table_attach_defaults(tableNetwork, labelConfiguration, 0, 1, 0, 1);
		gtk_table_attach_defaults(tableNetwork, comboConfiguration, 1, 2, 0, 1);

		// IP Label and Entry
		GtkWidget *labelIP = gtk_label_new("IP address:");
		gtk_misc_set_alignment(GTK_MISC(labelIP), 0, .5);
		entryIP = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryIP), (setting_netExtIP) ? setting_netExtIP : "");
		gtk_table_attach_defaults(tableNetwork, labelIP, 0, 1, 1, 2);
		gtk_table_attach_defaults(tableNetwork, entryIP, 1, 2, 1, 2);

		// Netmask Label and Entry
		GtkWidget *labelNetmask = gtk_label_new("Subnet mask:");
		gtk_misc_set_alignment(GTK_MISC(labelNetmask), 0, .5);
		entryNetmask = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryNetmask), (setting_netExtMask) ? setting_netExtMask : "");
		gtk_table_attach_defaults(tableNetwork, labelNetmask, 0, 1, 2, 3);
		gtk_table_attach_defaults(tableNetwork, entryNetmask, 1, 2, 2, 3);

		// Gateway Label and Entry
		GtkWidget *labelGateway = gtk_label_new("Gateway address:");
		gtk_misc_set_alignment(GTK_MISC(labelGateway), 0, .5);
		entryGateway = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryGateway), (setting_netExtGateway) ? setting_netExtGateway : "");
		gtk_table_attach_defaults(tableNetwork, labelGateway, 0, 1, 3, 4);
		gtk_table_attach_defaults(tableNetwork, entryGateway, 1, 2, 3, 4);

		// DNS1 Label and Entry
		GtkWidget *labelDNS1 = gtk_label_new("Primary DNS Server:");
		gtk_misc_set_alignment(GTK_MISC(labelDNS1), 0, .5);
		entryDNS1 = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryDNS1), (setting_netExtDNS1) ? setting_netExtDNS1 : "");
		gtk_table_attach_defaults(tableNetwork, labelDNS1, 0, 1, 4, 5);
		gtk_table_attach_defaults(tableNetwork, entryDNS1, 1, 2, 4, 5);
		
		// DNS2 Label and Entry
		GtkWidget *labelDNS2 = gtk_label_new("Secondary DNS Server:");
		gtk_misc_set_alignment(GTK_MISC(labelDNS2), 0, .5);
		entryDNS2 = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryDNS2), (setting_netExtDNS2) ? setting_netExtDNS2 : "");
		gtk_table_attach_defaults(tableNetwork, labelDNS2, 0, 1, 5, 6);
		gtk_table_attach_defaults(tableNetwork, entryDNS2, 1, 2, 5, 6);
	gtk_box_pack_start(GTK_BOX(mainBox), GTK_WIDGET(tableNetwork), TRUE, TRUE, 0);
	
	if (setting_netExtUseDhcp) {
		toggle_table_fields(GTK_WIDGET(tableNetwork), FALSE);
	}


	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step2E_forward_clicked), (gpointer)tableNetwork);
		
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}

