/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Paul Webber 2007 <webpaul1@gmail.com>
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <gtk/gtk.h>
#include <glade/glade.h>



/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

/* For testing propose use the local (not installed) glade file */
/* #define GLADE_FILE PACKAGE_DATA_DIR"/mce-installer/glade/mce-installer.glade" */
#define GLADE_FILE "mce-installer.glade"

#include "common.h"

static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data) {
	gtk_main_quit ();
	return FALSE;
}

int
main (int argc, char *argv[])
{

	run_as_root();
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_set_locale ();

	g_thread_init (NULL);
	gdk_threads_init ();
	gdk_threads_enter ();

	gtk_init (&argc, &argv);

	gxml = glade_xml_new (GLADE_FILE, NULL, NULL);


	/* This is important */
	glade_xml_signal_autoconnect (gxml);
	mainWindow = glade_xml_get_widget (gxml, "mainWindow");
	mainBox = glade_xml_get_widget (gxml, "mainBox");
	mainButtonBox = glade_xml_get_widget (gxml, "mainButtonBox");
	GtkImage *mainImage = GTK_IMAGE(glade_xml_get_widget(gxml, "mainImage"));

       	g_signal_connect (G_OBJECT (mainWindow), "delete_event", G_CALLBACK (delete_event), NULL);	 
	gtk_image_set_from_file(mainImage, "pluto.png");

	// Initializations
	setting_coreIP = detectCoreIpAddress();
//	setting_coreIP = "10.0.0.86";
	setting_deviceType = DT_HYBRID;
	setting_runDhcpServer = TRUE;
	setting_netExtKeep = TRUE;
	setting_netIntIPN = "192.168.80";
	setting_installType = INSTALL_TYPE_CD;

	setting_ubuntuExtraCdFrom = FROM_CD;
	setting_linuxmceCdFrom = FROM_CD;
	history = g_queue_new();

#if 0
displayStepCheckSystem();
//	displayStepLinuxMceCD();
#else
	displayStepCheckSystem();
#endif

	gtk_main ();
	gdk_threads_leave();
	return 0;
}
