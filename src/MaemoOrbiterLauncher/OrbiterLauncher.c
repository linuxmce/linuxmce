/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifdef MAEMO2
#include <hildon-widgets/hildon-program.h>
#else
#include <hildon-widgets/hildon-app.h> 
#endif

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>

int orbiterPid = 0;
gchar* routerIp;
gchar* deviceId;


GtkWidget *entryRouterIp;
GtkWidget *entryDeviceId;

const gchar* detect_router_ip_address(void) 
{
	printf("Searching for dcerouter's ip address...");
	struct sockaddr_in si_other;
	int s, i, slen=sizeof(si_other);
	int on=1;
	char buf[3]="xxx";
	gchar *ret = "";

	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	fcntl(s, F_SETFL, O_NONBLOCK);
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	memset((char *) &si_other, sizeof(si_other), 0);
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(33333);
	
	inet_aton("255.255.255.255", &si_other.sin_addr);
	sendto(s, buf, 3, 0, &si_other, slen);
	
	time_t start_time = time(0);
	while ((time(0) - start_time) < 2) {
		recvfrom(s, buf, 3, 0, &si_other, &slen);
	}
	close(s);

	
	ret = inet_ntoa(si_other.sin_addr);
	if (g_ascii_strncasecmp(ret,"255.255.255.255",16)) {
		printf("   %s\n",ret);
		return ret;
	} else {
		printf("   not found\n");
		return "";
	}
}


int start_orbiter(void) 
{
	int pid = fork();
	if (pid != 0) {
#ifdef MAEMO2
		printf("Executing /var/lib/install/bin/startOrbiter.sh -r %s -d deviceId %s\n",routerIp, deviceId);
		execl("/var/lib/install/bin/startOrbiter.sh", "0", "-r", routerIp, "-d", deviceId, NULL);	
#else
		printf("Executing /var/lib/install/bin/startOrbiter.sh -r %s -d deviceId %s\n",routerIp, deviceId);
		execl("/var/lib/install/bin/startOrbiter.sh", "0", "-r", routerIp, "-d", deviceId, NULL);	
#endif
	} else {
		orbiterPid = pid;
	}
}

static void on_buttonStart_cliked(GtkWidget *widget,gpointer data )
{
	routerIp = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryRouterIp)));
	deviceId = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryDeviceId)));

	start_orbiter();
}


int main(int argc, char *argv[]) 
{

	gtk_init(&argc, &argv);

#ifdef MAEMO2
	HildonProgram *program;
	program = HILDON_PROGRAM(hildon_program_get_instance());
	g_set_application_name("Pluto Orbiter");
	
	HildonWindow  *window;
	window = HILDON_WINDOW(hildon_window_new());
	hildon_program_add_window(program, window);
#else
	HildonApp *program;
	HildonAppView* window;

	program = HILDON_APP(hildon_app_new());
	hildon_app_set_title(program, "Pluto Orbiter"); 
	hildon_app_set_two_part_title(program, TRUE);

	window = HILDON_APPVIEW(hildon_appview_new("Pluto Orbiter"));
	hildon_app_set_appview(program, window);
	
#endif	
	GtkWidget *vbox = gtk_vbox_new(TRUE, 20);
	GtkWidget *table = gtk_table_new(2,2,TRUE);
	
	// Router IP Entry and Label
	GtkWidget *labelRouterIp;
	labelRouterIp = gtk_label_new("Router IP :");
	gtk_table_attach_defaults(GTK_TABLE(table),labelRouterIp,0,1,0,1);
	
	entryRouterIp = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryRouterIp), detect_router_ip_address());
	gtk_table_attach_defaults(GTK_TABLE(table),entryRouterIp,1,2,0,1);
	

	// Device ID Entry and Label
	GtkWidget *labelDeviceId;
	labelDeviceId = gtk_label_new("Orbiter No :");
	gtk_table_attach_defaults(GTK_TABLE(table), labelDeviceId,0,1,1,2);
		
	entryDeviceId = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table),entryDeviceId,1,2,1,2);
		
	gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);
	
	// Start Button
	GtkWidget *buttonStart = gtk_button_new_with_label("Start");
	gtk_box_pack_start(GTK_BOX(vbox), buttonStart, TRUE, FALSE, 0);
		
	// Image
	GdkPixbuf *pixbufBanner;
	GtkWidget *pixmapBanner;
	pixbufBanner = gdk_pixbuf_new_from_file("/var/lib/install/usr/share/pixmaps/pluto_baner.png",NULL);
	pixmapBanner = gtk_image_new_from_pixbuf(pixbufBanner);

	
	GtkWidget *hbox = gtk_hbox_new(TRUE, 10);	
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(vbox), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(pixmapBanner), TRUE, FALSE, 0);
	

	GtkWidget* frame = gtk_frame_new("");
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_container_add(GTK_CONTAINER(window), frame);
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_widget_show_all(GTK_WIDGET(program));

	// Connect signals
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(buttonStart), "clicked", G_CALLBACK (on_buttonStart_cliked), NULL);
	gtk_main();	
	
	return 0;
}

