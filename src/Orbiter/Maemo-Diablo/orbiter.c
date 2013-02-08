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

#define PACKAGE_VERSION "1.0"

#define SYSTEM_TITLE "LinuxMCE"
#define SYSTEM_URL "linuxmce.org"
#define SYSTEM_CAPTION "LinuxMCE"

int orbiterPid = 0;
int isConnected = 0;

GtkWidget *coreIP, *orbiterID, *lCheckBox;
HildonWindow *window;
GConfClient *gc_client;

gchar* gOrbiterID;
gchar* gCoreIP;
gint* gRestart;

static ConIcConnection *cnx = NULL;

static void window_destroy (GtkWidget* widget, gpointer data)
{
	  gtk_main_quit ();
}


static void my_connection_handler(ConIcConnection *connection,
                                  ConIcConnectionEvent *event,
                                  gpointer user_data)
{
    ConIcConnectionStatus status = con_ic_connection_event_get_status(event);
    ConIcConnectionError error;
    const gchar *iap_id = con_ic_event_get_iap_id(CON_IC_EVENT(event));
    const gchar *bearer = con_ic_event_get_bearer_type(CON_IC_EVENT(event));
    switch(status) {
        case CON_IC_STATUS_CONNECTED:
            g_debug("We are connected to IAP %s with bearer %s!", iap_id, bearer);
	    	isConnected = 1;
            break;
        case CON_IC_STATUS_DISCONNECTING:
            g_debug("We are disconnecting...");
	    	isConnected = 0;
            break;
        case CON_IC_STATUS_DISCONNECTED:
            g_debug("And we are disconnected. Let's see what went wrong...");
	    	isConnected = 0;
            error = con_ic_connection_event_get_error(event);
            switch(error) {
                case CON_IC_CONNECTION_ERROR_NONE:
                    g_debug("Libconic thinks there was nothing wrong.");
                    break;
                case CON_IC_CONNECTION_ERROR_INVALID_IAP:
                    g_debug("Invalid (non-existing?) IAP was requested.");
                    break;
                case CON_IC_CONNECTION_ERROR_CONNECTION_FAILED:
                    g_debug("Connection just failed.");
                    break;
                case CON_IC_CONNECTION_ERROR_USER_CANCELED:
                    g_debug("User canceled the connection attempt");
                    break;
            }
            break;
        default:
            g_debug("Unknown connection status received");
    }
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

    if(isConnected == 0)    
    {
        GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							"Make sure you have open Internet connection via WiFi, 3G or GPRS!");
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
	gconf_client_set_int(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/l_restart", can_restart, NULL);
  
    start_orbiter(NULL, NULL, NULL, NULL, NULL, 800, 480, NULL);
}

int start_orbiter(void) 
{
	int pid = fork();
	if (pid != 0) {
		execl("/opt/linuxmce-orbiter/bin/orbiter-launcher.sh", "Orbiter", "-r", gtk_entry_get_text (GTK_ENTRY (coreIP)), "-d",  gtk_entry_get_text (GTK_ENTRY (orbiterID)), NULL);
	} else {
		orbiterPid = pid;
	}
		
}

static void quit_po (GtkButton * button, gpointer data)
{
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

	imgLogo = gtk_image_new_from_file ("/usr/share/" SYSTEM_CAPTION "Orbiter/" SYSTEM_CAPTION "Logo.png");
    gtk_fixed_put (GTK_FIXED (fix), imgLogo, 10, 0);
    
    label = gtk_label_new (NULL);
	/* Info Label */
    gtk_label_set_markup (GTK_LABEL (label),
			  "<small>" SYSTEM_TITLE " Maemo Orbiter, built " __DATE__ "</small>\n");
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
    gtk_entry_set_text(GTK_ENTRY(coreIP),gCoreIP ? gCoreIP : "");

    /* Enter Orbiter ID */
    gtk_fixed_put (GTK_FIXED (fix), gtk_label_new ("Orbiter ID:"), 80, 200);
    orbiterID = gtk_entry_new_with_max_length (10);	
	gtk_widget_set_size_request(orbiterID, 60, 30);
    gtk_fixed_put (GTK_FIXED (fix), orbiterID, 290, 200);
    gtk_entry_set_text(GTK_ENTRY(orbiterID),gOrbiterID ? gOrbiterID : "");

    /* Checkbuttons to restart Orbiter automatically */
	lCheckBox = gtk_check_button_new_with_label ("Restart if crash");
    gtk_fixed_put (GTK_FIXED (fix), lCheckBox, 240, 240);	
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (lCheckBox), gRestart ? 1 : 0);
    
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
	gOrbiterID	= gconf_client_get_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/orbiter_id", NULL);
	gCoreIP 	= gconf_client_get_string(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/core_ip", NULL);
	gRestart	= gconf_client_get_int(gc_client, "/apps/maemo/" SYSTEM_CAPTION "orbiter/l_restart", NULL);
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

	gboolean success = FALSE;
	/* Create connection object */
	ConIcConnection *connection = con_ic_connection_new();
	/* Connect signal to receive connection events */
	g_signal_connect(G_OBJECT(connection), "connection-event", 
						                     G_CALLBACK(my_connection_handler), NULL);
	/* Request connection and check for the result */
	success = con_ic_connection_connect(connection, CON_IC_CONNECT_FLAG_NONE);
	if (!success) g_warning("Request for connection failed");
	else g_debug("Connection is ok!");

	gtk_main();
	
    return 0;
}
