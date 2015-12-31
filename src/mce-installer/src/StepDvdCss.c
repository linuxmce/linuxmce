/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_StepDvdCss_forward_clicked(GtkWidget *widget, gpointer data)  {

	gboolean goNext = TRUE;
	setting_installDvdCss = * (gint *)data;
	//GtkWidget* runWindow;

	/*switch(setting_installDvdCss) {
		case 1:
			goNext = FALSE;
                	runWindow = run_shell_command("./mce-installer-DvdCss.sh", "Installing DVD CSS", "Failed to cache the DVD CSS");
                	gtk_widget_show_all(runWindow);
                	if (gtk_dialog_run(GTK_DIALOG(runWindow)) == 1) {
                        	goNext = TRUE;
                	}	
			break;
		case 2:
			goNext = FALSE;
                	runWindow = run_shell_command("./mce-installer-Lame.sh", "Installing LAME", "Failed to cache the LAME");
                	gtk_widget_show_all(runWindow);
                	if (gtk_dialog_run(GTK_DIALOG(runWindow)) == 1) {
                        	goNext = TRUE;
                	}	
			break;
		case 3:
			goNext = FALSE;
                	runWindow = run_shell_command("./mce-installer-DvdCssandLame.sh", "Installing DVD CSS and LAME", "Failed to cache the DVD CSS and LAME");
                	gtk_widget_show_all(runWindow);
                	if (gtk_dialog_run(GTK_DIALOG(runWindow)) == 1) {
                        	goNext = TRUE;
                	}	
			break;
	}*/
	if (goNext) {
		g_queue_push_head(history, (gpointer)STEPDVDCSS);
		displayStep4();
	}
}

void on_StepDvdCss_radio_no_toggled(GtkWidget *widget, gpointer data) {
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gint *) data = 0;
	}
}

void on_StepDvdCss_radio_yes_dvdcss_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gint *) data = 1;
	}
}


void on_StepDvdCss_radio_yes_lame_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gint *) data = 2;
	}
}


void on_StepDvdCss_radio_yes_both_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gint *) data = 3;
	}
}

void displayStepDvdCss(void) {

	printf("StepDvdCss\n");

	gboolean libdvdcss, lame;
	setting_installDvdCss = 0; lame = FALSE; libdvdcss = FALSE;

	if (access("/usr/share/doc/libdvdcss2/changelog.gz", F_OK) == 0) {
		libdvdcss = TRUE;
	}

        if (access("/usr/share/doc/lame/changelog.Debian.gz", F_OK) == 0) {
		lame = TRUE;
	}

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label; 
	GSList *group = NULL;
	
	if ((libdvdcss == FALSE) && (lame == FALSE)) {
		label = gtk_label_new_for_wizard ("The software to play encrypted, commercial dvd’s (libdvdcss) and the sofware to create compressed audio files (lame), are not installed.  Is this legal in your area and should it be installed?");
		gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
		setting_installDvdCss = 1;
		// Radio : Yes libdvdcss
		GtkWidget *radioYesDvdCss = gtk_radio_button_new_with_label(group, "Yes, I want libdvdcss installed.");
		gtk_box_pack_start(GTK_BOX(mainBox), radioYesDvdCss, TRUE, TRUE, 0);
		g_signal_connect(G_OBJECT(radioYesDvdCss), "toggled", G_CALLBACK(on_StepDvdCss_radio_yes_dvdcss_toggled), (gpointer)&setting_installDvdCss);

		// Radio : Yes lame
		group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioYesDvdCss));
		GtkWidget *radioYesLame = gtk_radio_button_new_with_label(group, "Yes, I want lame installed.");
		gtk_box_pack_start(GTK_BOX(mainBox), radioYesLame, TRUE, TRUE, 0);
		g_signal_connect(G_OBJECT(radioYesLame), "toggled", G_CALLBACK(on_StepDvdCss_radio_yes_lame_toggled), (gpointer)&setting_installDvdCss);

		// Radio : Yes both
		group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioYesLame));
		GtkWidget *radioYesBoth = gtk_radio_button_new_with_label(group, "Yes, I want both packages installed.");
		gtk_box_pack_start(GTK_BOX(mainBox), radioYesBoth, TRUE, TRUE, 0);
		g_signal_connect(G_OBJECT(radioYesBoth), "toggled", G_CALLBACK(on_StepDvdCss_radio_yes_both_toggled), (gpointer)&setting_installDvdCss);
		group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioYesBoth));
	} else if (libdvdcss == FALSE) {	
		// Radio : Yes libdvdcss
		label = gtk_label_new_for_wizard ("The software to play encrypted, commercial dvd’s (libdvdcss) is not installed.  Is this legal in your area and should it be installed?");
		gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
		setting_installDvdCss = 1;
		GtkWidget *radioYesDvdCss = gtk_radio_button_new_with_label(group, "Yes, I want libdvdcss installed.");
		gtk_box_pack_start(GTK_BOX(mainBox), radioYesDvdCss, TRUE, TRUE, 0);
		g_signal_connect(G_OBJECT(radioYesDvdCss), "toggled", G_CALLBACK(on_StepDvdCss_radio_yes_dvdcss_toggled), (gpointer)&setting_installDvdCss);
		group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioYesDvdCss));
	} else if (lame == FALSE) {	
		// Radio : Yes lame
		label = gtk_label_new_for_wizard ("The sofware to create compressed audio files (lame) is not installed.  Is this legal in your area and should it be installed?");
		gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
		setting_installDvdCss = 2;
		GtkWidget *radioYesLame = gtk_radio_button_new_with_label(group, "Yes, I want lame installed.");
		gtk_box_pack_start(GTK_BOX(mainBox), radioYesLame, TRUE, TRUE, 0);
		g_signal_connect(G_OBJECT(radioYesLame), "toggled", G_CALLBACK(on_StepDvdCss_radio_yes_lame_toggled), (gpointer)&setting_installDvdCss);
		group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioYesLame));
	}

	// Radio : No
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, it's not allowed here.  Don't install it.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_StepDvdCss_radio_no_toggled), (gpointer)&setting_installDvdCss);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepDvdCss_forward_clicked), (gpointer)&setting_installDvdCss);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
