#include "RunShellCommand.h"
#include "common.h"

GtkWidget *runWindow;
pid_t      runApplicationPid;
//gint       runTagProgessBar;
gchar     *runErrorMessage;
static gboolean connectedToReaper;

gint run_shell_command_progress_pulse(gpointer data) {
	GtkProgressBar *pbar = GTK_PROGRESS_BAR(data);
	gtk_progress_bar_pulse(pbar);
	return 1;
}

void run_shell_reaper_child_exited(VteTerminal *terminal, gint pid, gint errcode, gpointer data) {
	if (runApplicationPid == pid) {
//		printf("Exited with error code : %d\n", errcode);
//		g_source_remove((gint)runTagProgessBar);

		gdk_threads_enter();
		gtk_widget_hide_all(GTK_WIDGET(runWindow));

		if (errcode != 0 ) {
			if (strcmp(runErrorMessage,"") != 0) {
			GtkWidget* runErrorDialog = gtk_dialog_new_with_buttons ("Message", GTK_WINDOW(mainWindow), 
					GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
			gtk_window_set_title(GTK_WINDOW(runErrorDialog), "Error");
			
			GtkWidget *errorLabel = gtk_label_new(runErrorMessage);
			gtk_container_add(GTK_CONTAINER(GTK_DIALOG(runErrorDialog)->vbox), errorLabel);
			gtk_widget_show_all(runErrorDialog);
				gtk_dialog_run(GTK_DIALOG(runErrorDialog));
				gtk_widget_destroy (GTK_WIDGET(runErrorDialog));
			}
		} else {
			gtk_dialog_response(runWindow, 1);
		}
		gdk_threads_leave();
	}
}

GtkWidget* run_shell_command(const gchar* application, const gchar* windowTitle, const gchar* errorMessage) {

	runWindow = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(runWindow), windowTitle);
	gtk_window_set_modal(GTK_WINDOW(runWindow), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(runWindow), GTK_WINDOW(mainWindow)); 
	gtk_window_set_position(GTK_WINDOW(runWindow), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_default_size(GTK_WINDOW(runWindow), 300, 70);

	gtk_container_set_border_width(GTK_CONTAINER(runWindow), 10);

	GtkWidget* runHBox = GTK_DIALOG(runWindow)->vbox; 

	GtkWidget* runLabelWait = gtk_label_new("Please Wait ...");
	gtk_box_pack_start(GTK_BOX(runHBox), runLabelWait, TRUE, TRUE, 5);

//	GtkWidget* runProgress = gtk_progress_bar_new();
//	gtk_box_pack_start(GTK_BOX(runHBox), runProgress, TRUE, TRUE, 5);
//	runTagProgessBar = g_timeout_add(200, run_shell_command_progress_pulse, runProgress);

	GtkWidget* runTerminal = vte_terminal_new();
	write_config_file();
	runApplicationPid = vte_terminal_fork_command(VTE_TERMINAL(runTerminal),application, NULL, NULL, "", FALSE, FALSE, FALSE);
	GObject* runVteReaper = G_OBJECT(vte_reaper_get());
	runErrorMessage = g_strdup(errorMessage);
	if (connectedToReaper == 0 ) {
		g_signal_connect(G_OBJECT(runVteReaper), "child-exited", G_CALLBACK(run_shell_reaper_child_exited), NULL);
		connectedToReaper = 1;
	}

	return runWindow;
}
