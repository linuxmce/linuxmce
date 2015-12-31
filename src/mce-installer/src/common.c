/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

static void remove_gtk_widget(GtkWidget *widget, gpointer data) {
	GtkWidget *container = (GtkWidget *) data;

	//delete only top level widgets, their children will be deleted automatically.
	gtk_container_remove(GTK_CONTAINER(container), widget);	
}

void cleanupContainer (GtkWidget *container) {

	if (GTK_IS_CONTAINER(container)) {
		gtk_container_foreach(GTK_CONTAINER(container), remove_gtk_widget, container);
	}	
}

void run_as_root(void) {
	if (getuid() == 0 ) {
		return;
	}

	printf("Try to run gksu ...");
	execl("/usr/bin/gksu","gksu","./mce-installer",NULL);
	execl("/usr/bin/kdesu","kdesu","-u","root","-c","./mce-installer",NULL);
	execl("/bin/su","su","-u","root","-c","./mce-installer", NULL);
	exit(1);
}

gboolean has_nv_video_driver(void) {
	pid_t grep_pid;
	int grep_status = -1;

	grep_pid = fork();
	if (grep_pid == 0) {
		execl("./mce-installer-NeedsNvidiaDrivers.sh","./mce-installer-NeedsNvidiaDrivers.sh", NULL);
		exit(1);
	} else if (grep_pid > 0) {
		waitpid (grep_pid, &grep_status, 0);
	}

	if (grep_status == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void on_back_clicked(GObject object, gpointer data) {
	if (!g_queue_is_empty(history) != 0) {
		gint prevScreen = (gint) g_queue_pop_head(history);
		switch (prevScreen){
			case STEP0A:
				displayStep0A();
				break;
			case STEP0B:
				displayStep0B();
				break;
			case STEP1A:
				displayStep1A();
				break;
			case STEP1B:
				displayStep1B();
				break;
			case STEP1C:
				displayStep1C();
				break;
			case STEP1D:
				displayStep1D();
				break;
			case STEP2A:
				displayStep2A();
				break;
			case STEP2C:
				displayStep2C();
				break;
			case STEP2E:
				displayStep2E();
				break;
			case STEP3:
				displayStep3();
				break;
			case STEP4:
				displayStep4();
				break;
			case STEP5:
				displayStep5();
				break;
			case STEPAPTMIRROR:
				displayStepAptMirror();
				break;
			case STEPUI:
				displayStepUI();
				break;
			case STEPUBUTUEXTRACD:
				displayStepUbuntuExtraCD();
				break;
			case STEPCONF:
				displayStepConfFiles();
				break;
			case STEPDVDCSS:
				displayStepDvdCss();
				break;
			case STEPCREATEMD:
				displayStepCreateDiskedDevice();
				break;
			case STEPNVIDIADRIVERS:
				displayStepNvidiaDrivers();
				break;
			case STEPNOTIFICATION:
				displayStepNotification();
				break;
			case STEPCHECKSYSTEM:
				displayStepCheckSystem();
				break;
			case STEPUBUNTULIVECD:
				displayStepUbuntuExtraCD();
				break;
			case STEPLINUXMCECD:
				displayStepLinuxMceCD();
				break;
			default:
				printf("Unknown screen in history [%d]", prevScreen);
		}
	} else {
		printf("No screen in history\n");
	}
}

gchar* detectCoreIpAddress(void) {
	printf("Searching for dcerouter's ip address...");
	struct sockaddr_in si_other;
	int s, slen=sizeof(si_other);
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
	if (g_ascii_strncasecmp(ret, "255.255.255.255",16)) {
		printf("   %s\n",ret);
		return ret;
	} else {
		printf("   not found\n");
		return NULL;
	}
}

GtkWidget* gtk_label_new_for_wizard(const gchar *text) {
	GtkWidget *label;

	label = gtk_label_new(text);
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
//	gtk_widget_set_size_request(label, 450,-1);

	return label;
}

void write_config_file(void) {
	gchar *output;
	if (setting_deviceType != DT_MEDIA_DIRECTOR) {
		output = g_strdup_printf(
				"c_deviceType=%d\n"
				"c_netIfaceNo=%d\n"
				"c_netExtName='%s'\n"
				"c_netExtIP='%s'\n"
				"c_netExtMask='%s'\n"
				"c_netExtGateway='%s'\n"
				"c_netExtDNS1='%s'\n"
				"c_netExtDNS2='%s'\n"
				"c_netExtUseDhcp=%d\n"
				"c_runDhcpServer=%d\n"
				"c_netIntName='%s'\n"
				"c_netIntIPN='%s'\n"
				"c_startupType=%d\n"
				"c_installType=%d\n"
				"c_installMirror='%s'\n"
				"c_netExtKeep='%s'\n"
				"c_installUI=%d\n"
				"c_linuxmceCdFrom=%d\n"
				"c_linuxmceCdIsoPath='%s'\n"
				"c_ubuntuExtraCdFrom=%d\n"
				"c_ubuntuExtraCdPath='%s'\n"
				"c_ubuntuLiveCdFrom=%d\n"
                                "c_ubuntuLiveCdPath='%s'\n"
				"c_installDvdCss=%d\n"
				,setting_deviceType
				,setting_netIfaceNo
				,setting_netExtName
				,setting_netExtIP
				,setting_netExtMask
				,setting_netExtGateway
				,setting_netExtDNS1
				,setting_netExtDNS2
				,setting_netExtUseDhcp
				,setting_runDhcpServer
				,setting_netIntName
				,setting_netIntIPN
				,setting_startupType
				,setting_installType
				,setting_installMirror
				,setting_netExtKeep?"true":"false"
				,setting_UI
				,setting_linuxmceCdFrom
				,setting_linuxmceCdIsoPath
				,setting_ubuntuExtraCdFrom
				,setting_ubuntuExtraCdIsoPath
				,setting_ubuntuLiveCdFrom
				,setting_ubuntuLiveCdIsoPath
				,setting_installDvdCss
		);
	} else {
		output = g_strdup_printf(
				"c_deviceType=%d\n"
				"c_coreIpAddress=%s\n"
				"c_deviceID=%s\n"
				"c_startupType=%d\n"
				"c_installType=%d\n"
				"c_installUI=%d\n"
				,setting_deviceType
				,setting_coreIP
				,setting_deviceID
				,setting_startupType
				,setting_installType
				,setting_UI
		);
	}

	FILE *conf_file;

	conf_file = fopen("/tmp/mce_wizard_data.sh", "w");
	g_fprintf(conf_file, "%s", output);
	fclose(conf_file);
}
