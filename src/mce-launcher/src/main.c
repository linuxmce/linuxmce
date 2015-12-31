/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Paul Webber 2007 <paul.webber@linuxmce.com>
 * 
 * main.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * main.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with main.c.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <glade/glade.h>
#include <vte/vte.h>
#include <pthread.h>

#include "listener.h"

/* For testing propose use the local (not installed) glade file */
#define GLADE_FILE PACKAGE_DATA_DIR"/mce-launcher/glade/mce-launcher.glade"
#define LOGO_FILE  PACKAGE_DATA_DIR"/mce-launcher/images/mce-logo.png"
#define SCRIPT_DIR PACKAGE_DATA_DIR"/mce-launcher/scripts/"
#define MCE_LAUNCHER "/usr/bin/mce-launcher"
/*
#define GLADE_FILE "mce-launcher.glade" 
#define LOGO_FILE "mce-logo.png"
#define SCRIPT_DIR ""
*/

static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data) {
	gtk_main_quit ();
	return FALSE;
}
void run_as_root(void) {
	if (getuid() == 0 ) {
		return;
	}

	printf("Try to run gksu ...");
	execl("/usr/bin/gksu","gksu",MCE_LAUNCHER,NULL);
	execl("/usr/bin/kdesu","kdesu","-u","root","-c",MCE_LAUNCHER,NULL);
	execl("/bin/su","su","-u","root","-c",MCE_LAUNCHER, NULL);
	exit(1);
}


int
main (int argc, char *argv[])
{

	run_as_root();

	GtkWidget *mainWindow;
	GtkWidget *expanderMain;
	GtkWidget *progressMain;
	GtkWidget *labelMain;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	gtk_set_locale ();

	/* init threads */
	g_thread_init (NULL);
	gdk_threads_init ();
	gdk_threads_enter ();

	/* init gtk */
	gtk_init (&argc, &argv);

	/* init glade */
	GladeXML  *gxml = glade_xml_new (GLADE_FILE, NULL, NULL);
	glade_xml_signal_autoconnect (gxml);

	/* customize main window widgets */
	mainWindow = glade_xml_get_widget (gxml, "windowMain");
	g_signal_connect (G_OBJECT (mainWindow), "delete_event", G_CALLBACK (delete_event), NULL);

	expanderMain = glade_xml_get_widget(gxml, "expanderMain");
	GtkWidget *terminal = vte_terminal_new();
	g_signal_connect(G_OBJECT(terminal),"child-exited", G_CALLBACK(delete_event), NULL);
	gtk_container_add(GTK_CONTAINER(expanderMain), terminal);

	labelMain = glade_xml_get_widget(gxml, "labelMain");
	progressMain = glade_xml_get_widget(gxml, "progressMain");

	vte_terminal_fork_command(VTE_TERMINAL(terminal),SCRIPT_DIR"/mce-launcher-core.sh",NULL, NULL, "", FALSE, FALSE, FALSE);
	gtk_widget_show_all(GTK_WIDGET(terminal));

	GtkWidget* imageMain = glade_xml_get_widget(gxml, "imageMain");
	gtk_image_set_from_file(imageMain, LOGO_FILE);

	/* init listener thread */	
	listener_args args;
	args.progressMain = progressMain;
	args.labelMain = labelMain;

	pthread_t listener_tid;
	pthread_create(&listener_tid, NULL, listener_thread, &args);

	/* enter gtk main loop */
	gtk_main ();
	gdk_threads_leave ();

	return 0;
}
