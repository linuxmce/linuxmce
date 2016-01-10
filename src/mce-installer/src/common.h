/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __COMMON_H_ 
#define __COMMON_H_

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib.h>

#include <vte/vte.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>

#include "RunShellCommand.h"

#include "Step0A.h"
#include "Step0B.h"
#include "Step1A.h"
#include "Step1B.h"
#include "Step1C.h"
#include "Step1D.h"
#include "Step2E.h"
#include "Step2F.h"
#include "Step2C.h"
#include "Step2A.h"
#include "Step3.h"
#include "Step4.h"
#include "Step5.h"
#include "Step6.h"
#include "StepAptMirror.h"
#include "StepUbuntuExtraCD.h"
#include "StepInstallDependencies.h"
#include "StepUI.h"
#include "StepDvdCss.h"
#include "StepNvidiaDrivers.h"
#include "StepNvidiaDriversEnd.h"
#include "StepConfFiles.h"
#include "StepCreateDiskedDevice.h"
#include "StepNotification.h"
#include "StepCheckSystem.h"
#include "StepUbuntuLiveCD.h"

#include "network-common.h"

#define STEP1A 1
#define STEP1B 2
#define STEP1C 3
#define STEP1D 4
#define STEP1E 5
#define STEP2A 6
#define STEP2C 7
#define STEP2E 8
#define STEP3  9
#define STEP4  10
#define STEP5  11
#define STEP2F 12
#define STEP0A 13
#define STEP0B 14
#define STEPAPTMIRROR 15
#define STEPUI 16 
#define STEPCONF 17
#define STEPUBUTUEXTRACD 19
#define STEPLINUXMCECD 20
#define STEPDVDCSS 21
#define STEPNVIDIADRIVERS 22
#define STEPCREATEMD 23
#define STEPNOTIFICATION 24
#define STEPCHECKSYSTEM 25
#define STEPUBUNTULIVECD 26

#define INSTALL_TYPE_CD   1
#define INSTALL_TYPE_ISO  2
#define INSTALL_TYPE_NET  3

#define FROM_CD 1
#define FROM_ISO 2
#define FROM_NET 3


GladeXML  *gxml;
GtkWidget *mainWindow;
GtkWidget *mainBox;
GtkWidget *mainButtonBox;
GQueue    *history;

void		cleanupContainer(GtkWidget *widget);
gchar*		detectCoreIpAddress(void);
void		write_config_file(void);
void		on_back_clicked(GObject object, gpointer data);
GtkWidget*	gtk_label_new_for_wizard(const gchar *text);
void		run_as_root(void);
gboolean	has_nv_video_driver(void);

#define DT_CORE 1
#define DT_HYBRID 2
#define DT_MEDIA_DIRECTOR 3

gchar*	 setting_coreIP;
gint	 setting_deviceType;
gint     setting_netIfaceNo;
gchar*   setting_netExtName;
gchar*   setting_netExtIP;
gchar*   setting_netExtMask;
gchar*   setting_netExtGateway;
gchar*   setting_netExtDNS1;
gchar*   setting_netExtDNS2;
gchar*   setting_netExtMac;
gboolean setting_netExtUseDhcp;
gboolean setting_netExtKeep;

gboolean setting_runDhcpServer;
gchar*   setting_netIntName;
gchar*   setting_netIntIPN;

gchar*   setting_deviceID;

gint     setting_startupType;
gint	 setting_installType;

gint	 setting_UI;

gchar*   setting_installMirror;

gint     setting_ubuntuExtraCdFrom;
gchar*   setting_ubuntuExtraCdIsoPath;

gint     setting_linuxmceCdFrom;
gchar*   setting_linuxmceCdIsoPath;

gint	 flag_mirror;
gchar	 *other_mirror;

gint     setting_ubuntuLiveCdFrom;
gchar*   setting_ubuntuLiveCdIsoPath;

gint	 setting_installDvdCss;
#endif
