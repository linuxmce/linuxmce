/*
 * gnome_screensaver.c v0.0.7
 *
 * Enable/Disable the GNOME screensaver
 * Supports GNOME screensaver API 2.14 and 2.15
 *
 * Call gnome_screensaver_control(1) to enable and
 * gnome_screensaver_control(0) to disable
 *
 */
/*
 * Orginally written for mplayer by Piotr Kaczuba
 *   (http://lists.mplayerhq.hu/pipermail/mplayer-dev-eng/2006-April/042661.html)
 *
 * Modified for xineliboutput by Alex Stansfield
 *   (http://www.linuxtv.org/pipermail/vdr/2007-July/013458.html)
 */

#include <stdlib.h>
#include <unistd.h>
#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOG_MODULENAME "[vdr-fe]    "
#include "../logdefs.h"

#include "gnome_screensaver.h"

#define GS_SERVICE   "org.gnome.ScreenSaver"
#define GS_PATH      "/org/gnome/ScreenSaver"
#define GS_INTERFACE "org.gnome.ScreenSaver"

#define GS_APPLICATION_NAME     "vdr-sxfe"
#define GS_REASON_FOR_INHIBIT   "Watching TV"

/* Log Messages */
#define MSG_OpenBusConnectionError "Failed to open connection to bus: %s"
#define MSG_RemoteMethodException "Caught remote method exception %s: %s"
#define MSG_GnomeAPI215Failed "GNOME screensaver 2.15 API failed, trying 2.14 API"
#define MSG_GError "Error: %s"
#define MSG_GNOMEScreensaverEnabled "GNOME screensaver enabled"
#define MSG_GNOMEScreensaverDisabled "GNOME screensaver disabled"

static guint32 cookie;

void gnome_screensaver_control(int enable)
{
  DBusGConnection *connection;
  GError *error;
  DBusGProxy *proxy;
  gboolean ret;

  g_type_init();

  /* Get a connection to the session bus */
  error = NULL;
  connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
  if (!connection) {
    LOGERR(MSG_OpenBusConnectionError, error ? error->message : "<null>");
    g_error_free(error);
    return;
  }

  /* Create a proxy object */
  proxy = dbus_g_proxy_new_for_name(connection,
                                    GS_SERVICE, GS_PATH, GS_INTERFACE);
  if (!proxy) {
    LOGDBG("Failed to get a proxy for gnome-screensaver");
    return;
  }

  /* Enable the screensaver */
  if (enable) {
    /* First call the GNOME screensaver 2.15 API method */
    error = NULL;
    ret =
        dbus_g_proxy_call(proxy, "UnInhibit", &error,
                          G_TYPE_UINT, cookie,
                          G_TYPE_INVALID, G_TYPE_INVALID);

    /* If this fails, try the GNOME screensaver 2.14 API */
    if (!ret && error->domain == DBUS_GERROR
        && error->code == DBUS_GERROR_UNKNOWN_METHOD) {
      LOGERR(MSG_GnomeAPI215Failed);
      g_error_free(error);
      error = NULL;
      ret =
          dbus_g_proxy_call(proxy, "AllowActivation", &error,
                            G_TYPE_INVALID, G_TYPE_INVALID);
    }
  }
  /* Disable the screensaver */
  else {
    /* First call the GNOME screensaver 2.15 API method */
    error = NULL;
    ret =
        dbus_g_proxy_call(proxy, "Inhibit", &error,
                          G_TYPE_STRING, GS_APPLICATION_NAME,
                          G_TYPE_STRING, GS_REASON_FOR_INHIBIT,
                          G_TYPE_INVALID,
                          G_TYPE_UINT, &cookie,
                          G_TYPE_INVALID);

    /* If this fails, try the GNOME screensaver 2.14 API */
    if (!ret && error->domain == DBUS_GERROR
        && error->code == DBUS_GERROR_UNKNOWN_METHOD) {
      LOGERR(MSG_GnomeAPI215Failed);
      g_error_free(error);
      error = NULL;
      ret =
          dbus_g_proxy_call(proxy, "InhibitActivation", &error,
                            G_TYPE_STRING, GS_REASON_FOR_INHIBIT,
                            G_TYPE_INVALID, G_TYPE_INVALID);
    }
  }

  if (!ret) {
    /* Check if it's a remote exception or a regular GError */
    if (error->domain == DBUS_GERROR
        && error->code == DBUS_GERROR_REMOTE_EXCEPTION) {
      LOGERR(MSG_RemoteMethodException, dbus_g_error_get_name(error), error->message);
    }
    else {
      LOGERR(MSG_GError, error->message);
    }
    g_error_free(error);
  }
  else {
    LOGMSG(enable ? MSG_GNOMEScreensaverEnabled : MSG_GNOMEScreensaverDisabled);
  }

  g_object_unref(proxy);
}
