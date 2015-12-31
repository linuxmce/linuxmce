/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_StepUI_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEPUI);
	if ((access("/usr/share/doc/libdvdcss2/changelog.gz", F_OK) == 0) && 
	    (access("/usr/share/doc/lame/changelog.Debian.gz", F_OK) == 0)) {	
		displayStep4();
	} else {
	        displayStepDvdCss();
	}
}

void on_StepUI_finish_clicked(GtkWidget *widget, gpointer data) {

}

void on_StepUI_radio_toggled(GtkWidget *widget, gpointer data) {
	setting_UI = (gint)data;
	printf("UI: %d\n", setting_UI);

}

void on_StepUI_application_clicked(GtkWidget *widget, gpointer data) {
	if (access("/usr/bin/xine", F_OK) != 0) {
		GtkWidget* runWindow = run_shell_command("./mce-installer-UI-InstallXine.sh", "Installing Xine Player", "Failed to install Xine Player");
		gtk_widget_show_all(runWindow);
		gtk_dialog_run(GTK_DIALOG(runWindow));
	}

	int aux_pid = fork();
	int aux_status;

	if ( aux_pid == 0 ) {
		if (fork() == 0) {
			execl("UIdiag", "UIdiag", NULL);
			_exit(0);
		}
		_exit (0);
	} else if (aux_pid > 0 ) {
		waitpid (aux_pid, &aux_status, 0);
	}
}

void on_StepUI_url_clicked(GtkWidget *widget, gpointer data) {
	int aux_pid = fork();
	int aux_status;

	if ( aux_pid == 0 ) {
		if (fork() == 0) {
			execl("/usr/bin/konqueror", "/usr/bin/konqueror", "http://wiki.linuxmce.com/index.php/Graphics_Test", NULL);
			_exit(0);
		}
		_exit (0);
	} else if (aux_pid > 0 ) {
		waitpid (aux_pid, &aux_status, 0);
	}
}

void displayStepUI(void) {
	printf("StepUI\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);
	
	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("What interface do you want to use ?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
	
	// Info Text
	GtkWidget *expander_info = gtk_frame_new("Info");
	gtk_box_pack_start(GTK_BOX(mainBox), expander_info, TRUE, TRUE, 0);
	//GtkWidget *expander_info = mainBox;

	GtkWidget *box_info = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(box_info), 5);
	gtk_container_add(GTK_CONTAINER(expander_info), box_info);

	GtkWidget *label_info = gtk_label_new_for_wizard ("Before making your selection, use the 'Graphics Test' application to confirm which UI your hardware can handle.  Also read the online documentation on the subject.");
	gtk_box_pack_start(GTK_BOX(box_info), label_info, TRUE, TRUE, 0);

	GtkWidget *info_url = gtk_button_new_with_label ("Read Online Documentation");
	g_signal_connect(G_OBJECT(info_url), "clicked", G_CALLBACK(on_StepUI_url_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(box_info), info_url, TRUE, TRUE, 0);

	GtkWidget *info_app = gtk_button_new_with_label ("Run Graphical Test Application");
	g_signal_connect(G_OBJECT(info_app), "clicked", G_CALLBACK(on_StepUI_application_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(box_info), info_app, TRUE, TRUE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radio[3];

	radio[0] = gtk_radio_button_new_with_label(group, "Basic UI (UI1)");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
	radio[1] = gtk_radio_button_new_with_label(group, "UI2 with with medium settings (masking)");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
	radio[2] = gtk_radio_button_new_with_label(group, "UI2 with alpha blending/transparency");

	int i;
	for (i=0; i<3; i++) {
		g_signal_connect(G_OBJECT(radio[i]), "toggled", G_CALLBACK(on_StepUI_radio_toggled), (gpointer)i);
		gtk_box_pack_start(GTK_BOX(mainBox), radio[i], TRUE, FALSE, i);
		if (setting_UI == i) { 
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio[i]), TRUE);
		}
	}

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepUI_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
