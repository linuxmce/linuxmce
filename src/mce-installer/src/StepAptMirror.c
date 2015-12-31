/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

//GHashTable *mirrors;
GtkWidget *txt_entry;

void on_StepAptMirror_forward_clicked(GtkWidget *widget, gpointer data)  {
	g_queue_push_head(history, (gpointer)STEPAPTMIRROR);	
	if (flag_mirror==1){	
		other_mirror = g_strdup_printf("%s",gtk_entry_get_text(txt_entry));
		setting_installMirror=other_mirror;
		//printf("am ramas cu ohter mirror: >>>%s<<<<", setting_installMirror);
	}
	
	if (g_str_has_prefix(setting_installMirror,"---")) {
		GtkWidget *dialog, *dg_label, *dg_OK;
                dialog = gtk_dialog_new();
                dg_label = gtk_label_new ("Please choose a mirror");
                dg_OK = gtk_button_new_with_label("OK");
                gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
                gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 70);
                //gtk_signal_connect (GTK_OBJECT (dg_OK), "clicked", GTK_SIGNAL_FUNC (on_StepUbuntuLiveCD_OK_clicked), NULL);
                gtk_signal_connect_object(GTK_OBJECT (dg_OK), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(dialog));
                gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), dg_OK);
                gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), dg_label);
                gtk_widget_show_all (dialog);
                gtk_widget_grab_focus(dialog);

	} else {
		displayStep3();
	} 

}

void on_StepAptMirror_combo_changed(GtkWidget *widget, gpointer data) {
	//setting_installMirror = g_hash_table_lookup(mirrors, gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	setting_installMirror = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	//printf("Am schimbat in >> %s << \n", setting_installMirror);
}

void on_StepAptMirror_toggled(GtkWidget *widget, gpointer data) {

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
		flag_mirror = 1;
	} else {
		flag_mirror = 0;
		other_mirror="";
	}
}

void displayStepAptMirror(void) {
	printf("StepAptMirror\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Please choose a mirror");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	GtkTable *tableMirror = GTK_TABLE(gtk_table_new(3,2,TRUE));
	//Mirror label and combo box
	GtkWidget *labelMirror = gtk_label_new("Mirror:");
	gtk_misc_set_alignment(GTK_MISC(labelMirror), 0, .5);

	//read the list of mirrors
	FILE *fmirror = fopen("./mirrors", "r");
	char line[1024];
	//gchar **tokens;
	//mirrors = g_hash_table_new(g_str_hash, g_str_equal);
	GtkWidget *comboMirror = gtk_combo_box_new_text();
	while (1) {
		fgets(line,1024,fmirror);
		if (feof(fmirror)) break;
		//printf("Read %s--\n",line);
		line[strlen(line)-1] = '\0';
		//tokens = g_strsplit(line,"|",0);
		//g_hash_table_insert(mirrors,nr,line);
		//printf("name: %s -- mirror: %s\n",tokens[0], tokens[1]);
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboMirror),line);
	}

	//GtkWidget *comboMirror = gtk_combo_box_new_text();
	//gtk_combo_box_append_text(GTK_COMBO_BOX(comboMirror),"Mirror 1");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(comboMirror),"Mirror 2");

	gtk_combo_box_set_active(GTK_COMBO_BOX(comboMirror),0);
	setting_installMirror = "---Please choose a mirror---";
	//setting_installMirror = g_hash_table_lookup(mirrors, gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboMirror)));
	//printf("Initial %s\n", setting_installMirror);

	g_signal_connect(G_OBJECT(comboMirror), "changed", G_CALLBACK(on_StepAptMirror_combo_changed), (gpointer)tableMirror);

	gtk_table_attach_defaults(tableMirror, labelMirror, 0, 1, 0, 1);
	gtk_table_attach_defaults(tableMirror, comboMirror, 1, 2, 0, 1);

	//GSList *group = NULL;
	GtkWidget *check;
	check = gtk_check_button_new_with_label("Other mirror:");
	//group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_signal_connect(G_OBJECT(check),"toggled",G_CALLBACK(on_StepAptMirror_toggled), (gpointer)0);

	txt_entry = gtk_entry_new();

	if(flag_mirror==1){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),TRUE);
		gtk_entry_set_text(txt_entry,other_mirror);
	}

	gtk_table_attach_defaults(tableMirror, check, 0, 1, 1, 2);
	gtk_table_attach_defaults(tableMirror, txt_entry, 1, 2, 1, 2);

	gtk_box_pack_start(GTK_BOX(mainBox), GTK_WIDGET(tableMirror), TRUE, TRUE, 0);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepAptMirror_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
