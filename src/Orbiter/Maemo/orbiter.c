/*
 *	Maemo wrapper for LinuxMCE/Plutohome Orbiter. For more information
 *	about home automation system LinuxMCE, please, visit
 *	www.linuxmce.com and for Plutohome - www.plutohome.com
 *
 * 
 * (c) 2007 - 2008 by Michael Stepanov <stepanov.michael@gmail.com>
 *  
 * It may be distributed under the terms of the GNU Public Licence
 *
 */

#ifdef OS2008

#include <hildon/hildon-program.h>

#else

#include <hildon-widgets/hildon-program.h>

#endif

#include <gtk/gtk.h> 

#include <libosso.h>

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

#include <conicevent.h>
#include <coniciap.h>
#include <conicconnection.h>
#include <conicconnectionevent.h>

#define PACKAGE_VERSION "0.1"
#define VERSION "1404"

#ifdef PLUTO
#define SYSTEM_TITLE "Plutohome"
#define SYSTEM_URL "www.plutohome.com"
#define SYSTEM_CAPTION "plutohome"
#else
#define SYSTEM_TITLE "LinuxMCE"
#define SYSTEM_URL "www.linuxmce.com"
#define SYSTEM_CAPTION "linuxmce"
#endif


GtkWidget *coreIP, *orbiterID, *lCheckBox;
HildonWindow *window;
GConfClient *gc_client;

gint* gloggin_prefs;
gchar* gorbiter_id;
gchar* gcore_ip;
gint* gRetsart;

static ConIcConnection *cnx = NULL;

static void window_destroy (GtkWidget* widget, gpointer data)
{
	  gtk_main_quit ();
}

static void on_connection_event (ConIcConnection *cnx, ConIcConnectionEvent *event, gpointer user_data)
{
    printf ("%s\n", __FUNCTION__);

    /* Handle errors: */
    switch (con_ic_connection_event_get_error(event)) {
    case CON_IC_CONNECTION_ERROR_NONE:
        break;
    case CON_IC_CONNECTION_ERROR_INVALID_IAP:
        g_warning ("conic: IAP is invalid");
        break;
    case CON_IC_CONNECTION_ERROR_CONNECTION_FAILED:
        g_warning ("conic: connection failed");
        break;
    case CON_IC_CONNECTION_ERROR_USER_CANCELED:
        g_warning ("conic: user cancelled");
        break;
    default:
        g_warning ("conic: unexpected error");
        break;
    }

    /* Handle status changes: */
    switch (con_ic_connection_event_get_status(event)) {
    case CON_IC_STATUS_CONNECTED:
        printf ("DEBUG: %s: Connected.\n", __FUNCTION__);
    
        break;
    case CON_IC_STATUS_DISCONNECTED:
        printf ("DEBUG: %s: Disconnected.\n", __FUNCTION__);
        break;
    case CON_IC_STATUS_DISCONNECTING:
        printf ("DEBUG: %s: new status: DISCONNECTING.\n", __FUNCTION__);

        break;
    default:
        printf ("DEBUG: %s: Unexpected status.\n", __FUNCTION__);
        break;
    }

}

static gboolean check_wifi_connection(void)
{
    printf ("%s: Creating ConIcConnection.\n", __FUNCTION__);

    cnx = con_ic_connection_new ();
    if (!cnx) {
        g_warning ("con_ic_connection_new failed.");
        return FALSE;
    }

    const int connection = g_signal_connect (cnx, "connection-event", G_CALLBACK(on_connection_event), NULL);

    g_object_set (cnx, "automatic-connection-events", TRUE, NULL);

    printf ("%s: Attempting automatic connect. Waiting for signal.\n", __FUNCTION__);
    
    if (!con_ic_connection_connect (cnx, CON_IC_CONNECT_FLAG_AUTOMATICALLY_TRIGGERED))
    {
        g_warning ("could not send connect dbus message");
        return FALSE;
    }

    printf ("%s: After automatic connect. Waiting for signal.\n", __FUNCTION__);

    return TRUE;
}

/*
 * A simple way to check if WiFi connection is up
 */
static gboolean check_is_online(void)
{
    gboolean ret = check_wifi_connection();
        
    if (!g_file_test ("/var/run/resolv.conf", G_FILE_TEST_EXISTS))
        if (!g_file_test ("/tmp/resolv.conf.wlan0", G_FILE_TEST_EXISTS))
            if (!g_file_test ("/tmp/resolv.conf.ppp0", G_FILE_TEST_EXISTS))
                return FALSE;

    return TRUE;
}

/*
 * Check if MS TrueType fonts  present in the device
 */
static gboolean check_msttf_fonts(void) 
{
    if (!g_file_test ("/usr/share/fonts/truetype/msttcorefonts", G_FILE_TEST_EXISTS)) 
        return FALSE;

    return TRUE;
}        
        

static void start_po (GtkButton * button, gpointer data)
{
    char snd[4];
    char swp[4];

    snd[0] = 0;
    swp[0] = 0;

    if(!check_is_online())    
    {
        GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							"Please connect to WiFi access point!");
	    gtk_dialog_run (GTK_DIALOG (dialog));
	    gtk_widget_destroy (dialog);
	    return;
    }    


    if (strcmp ("", (char *) gtk_entry_get_text (GTK_ENTRY (coreIP))) == 0)
	{
	    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							"Please specify a valid address for " SYSTEM_TITLE " core!");
	    gtk_dialog_run (GTK_DIALOG (dialog));
	    gtk_widget_destroy (dialog);
	    return;
	}

	if (strcmp ("", (char *) gtk_entry_get_text (GTK_ENTRY (orbiterID))) == 0)	
	{
	    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							"Please specify an Orbiter ID!");
	    gtk_dialog_run (GTK_DIALOG (dialog));
	    gtk_widget_destroy (dialog);
	    return;
	}    

    if (!check_msttf_fonts()) {
        GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							"Please copy MS TrueType fonts from your core - /usr/share/fonts/truetype/msttcorefonts to the same place in your device!");
	    gtk_dialog_run (GTK_DIALOG (dialog));
	    gtk_widget_destroy (dialog);
        return;
    }
    
    int can_restart = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (lCheckBox));
    
	/* Store user input data  using GConf */
	gconf_client_set_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/core_ip", gtk_entry_get_text (GTK_ENTRY (coreIP)), NULL);
	gconf_client_set_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/orbiter_id", gtk_entry_get_text (GTK_ENTRY (orbiterID)), NULL);
	/*gconf_client_set_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/l_restart", (char)can_restart, NULL);*/
  
    while(1) {
        int status = start_orbiter();
        int restart = 0;
	
        if (status == -1) { perror("system"); exit(EXIT_FAILURE); }
        if (WIFEXITED(status)) {
            printf("exited, status=%d\n", WEXITSTATUS(status));
            if(WEXITSTATUS(status) != 0)
                restart = 1;            
        } else if (WIFSIGNALED(status)) {
            printf("killed by signal %d\n", WTERMSIG(status));
                restart = 1;
        } else if (WIFSTOPPED(status)) {
            printf("stopped by signal %d\n", WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("continued\n");
        }

        if((restart == 0) || (restart != 0 && can_restart != 1))
            break;       
        else
            sleep(30);        
    }
    
}


int start_orbiter(void) 
{
	/* Start Orbiter */
    char buffer[250];
	sprintf (buffer, "Orbiter -r %s -d %s > /dev/null 2>&1",
		     gtk_entry_get_text (GTK_ENTRY (coreIP)),
		     gtk_entry_get_text (GTK_ENTRY (orbiterID)));
	puts (buffer);
	int status = system (buffer);

    return status;
}

static void quit_po (GtkButton * button, gpointer data)
{
	//gtk_widget_destroy (GTK_WIDGET (data));
	gtk_main_quit();
}


static void display_help (GtkButton * button, gpointer data)
{
    GtkWidget *dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW (window),
							    GTK_DIALOG_DESTROY_WITH_PARENT,
							    GTK_MESSAGE_INFO,
							    GTK_BUTTONS_OK,
							    "<small><b>" SYSTEM_TITLE " Obiter for Nokia770/N800/N810</b>.\n"
#ifdef OS2008
                                "Situated for <b>OS2008</b> only!\n"
#else
                                "Situated for OS2006/OS2007/OS2007HE.\n"
#endif
							    "To start the Orbiter just specify core IP and Orbiter's ID.\n"
							    "<i><b><u>Note:</u></b> you have to connect your Nokia to the WLAN before run the Orbiter!</i>\n"
                                "Author: Michael Stepanov &lt;stepanov.michael@gmail.com&gt;\n"
								"For more information, please, visit " SYSTEM_URL " and \n pluto-nokia.garage.maemo.org</small>");
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

HildonWindow *create_form (void) 
{
	GtkWidget *label, *btnStart, *btnHelp, *btnQuit, *imgLogo, *fix, *frmPrefs;
	
	/* Create HildonWindow and set it to HildonProgram */
    window = HILDON_WINDOW(hildon_window_new());

	fix = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fix);

	imgLogo = gtk_image_new_from_file ("/usr/share/" SYSTEM_CAPTION "orbiter/" SYSTEM_CAPTION "logo.png");
    gtk_fixed_put (GTK_FIXED (fix), imgLogo, 20, 10);
    
    label = gtk_label_new (NULL);
	/* Info Label */
    gtk_label_set_markup (GTK_LABEL (label),
			  "<small>" SYSTEM_TITLE " Orbiter <b>" VERSION "</b>, built <b>" __DATE__ "</b></small>\n");
	gtk_fixed_put(GTK_FIXED (fix), label, 200, 20);

    /* Frame Preferences */
    frmPrefs = gtk_frame_new ("");
    gtk_fixed_put (GTK_FIXED (fix), frmPrefs, 10, 100);
    gtk_widget_set_size_request (frmPrefs, 650, 300);

    /* Enter Core IP/host name */
    gtk_fixed_put (GTK_FIXED (fix), gtk_label_new (SYSTEM_TITLE " Core IP:"), 80, 150);
    coreIP = gtk_entry_new_with_max_length (200);
	gtk_widget_set_size_request(coreIP, 200, 30);
    gtk_fixed_put (GTK_FIXED (fix), coreIP, 290, 150);
    gtk_entry_set_text(GTK_ENTRY(coreIP),gcore_ip ? gcore_ip : "");

    /* Enter Orbiter ID */
    gtk_fixed_put (GTK_FIXED (fix), gtk_label_new ("Orbiter ID:"), 80, 200);
    orbiterID = gtk_entry_new_with_max_length (10);	
	gtk_widget_set_size_request(orbiterID, 60, 30);
    gtk_fixed_put (GTK_FIXED (fix), orbiterID, 290, 200);
    gtk_entry_set_text(GTK_ENTRY(orbiterID),gorbiter_id ? gorbiter_id : "");

    /* Checkbuttons to restart Orbiter automatically */
	lCheckBox = gtk_check_button_new_with_label ("Restart if crash");
    gtk_fixed_put (GTK_FIXED (fix), lCheckBox, 240, 240);	
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (lCheckBox), gloggin_prefs ? 1 : 0);
    
    /* Three Buttons: Start, Info and Quit */
    btnStart = gtk_button_new_with_label ("Start Orbiter");
    gtk_widget_set_size_request (btnStart, 240, 60);
    gtk_fixed_put (GTK_FIXED (fix), btnStart, 20, 330);
    g_signal_connect (G_OBJECT (btnStart), "clicked", G_CALLBACK (start_po), window);

    btnHelp = gtk_button_new_with_label ("Info");
    gtk_widget_set_size_request (btnHelp, 100, 60);
    gtk_fixed_put (GTK_FIXED (fix), btnHelp, 290, 330);
    g_signal_connect (G_OBJECT (btnHelp), "clicked", G_CALLBACK (display_help), window);

    btnQuit = gtk_button_new_with_label ("Quit");
    gtk_widget_set_size_request (btnQuit, 240, 60);
    gtk_fixed_put (GTK_FIXED (fix), btnQuit, 410, 330);
    g_signal_connect (G_OBJECT (btnQuit), "clicked", G_CALLBACK (quit_po), window);
	
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	
    /* Begin the main application */
    gtk_widget_show_all(GTK_WIDGET(window));
	
    return window;
}

void init_properties (void) 
{
	/*
	 * Restore properties from GConf
	 */
	gorbiter_id 	= gconf_client_get_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/orbiter_id", NULL);
	gcore_ip 		= gconf_client_get_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/core_ip", NULL);
	gRetsart 		= (int)gconf_client_get_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/l_restart", NULL);
}

gint dbus_callback (const gchar * interface, const gchar * method,
		            GArray * arguments, gpointer data, osso_rpc_t * retval)
{
	printf ("linuxmceorbiter dbus: %s, %s\n", interface, method);

	if (!strcmp (method, "top_application"))
				    gtk_window_present (GTK_WINDOW (data));

	retval->type = DBUS_TYPE_INVALID;
	return OSSO_OK;
}


int main(int argc, char *argv[])
{
    HildonProgram *program;
    HildonWindow *window;
	osso_context_t *osso_context;
	osso_return_t ret;

	osso_context = osso_initialize(SYSTEM_CAPTION "orbiter", PACKAGE_VERSION, TRUE, NULL);
	if (osso_context == NULL)
	{
	    return OSSO_ERROR;
	}
	
    gtk_init(&argc, &argv);
	
	/* Init CGonf */
	gc_client = gconf_client_get_default();
	if (gc_client == NULL) {
	    return FALSE;
	}
	init_properties();

    program = HILDON_PROGRAM(hildon_program_get_instance());
    g_set_application_name(SYSTEM_TITLE " Orbiter");
	
	window = create_form();
    hildon_program_add_window(program, window);

	ret = osso_rpc_set_default_cb_f (osso_context, dbus_callback, window);
	if (ret != OSSO_OK)
	{
		fprintf (stderr, "osso_rpc_set_default_cb_f failed: %d.\n", ret);
		exit (1);
	}	  
    
    gtk_main();

    return 0;
}
