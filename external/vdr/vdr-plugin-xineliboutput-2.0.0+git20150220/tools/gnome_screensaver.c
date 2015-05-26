/*
 * gnome_screensaver.c v0.0.7
 *
 * Enable/Disable the GNOME screensaver
 * Supports GNOME screensaver API 2.14 and 2.15
 * Supports GNOME SessionManager API
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
 *
 * GNOME SessionManager support by Timo Eskola.
 */

#include <stdlib.h>
#include <unistd.h>
#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "../features.h"
#ifdef HAVE_MCE_DBUS_NAMES
# include <mce/dbus-names.h>
#endif

#define LOG_MODULENAME "[scrnsaver] "
#include "../logdefs.h"

#include "gnome_screensaver.h"

#define GS_SERVICE   "org.gnome.ScreenSaver"
#define GS_PATH      "/org/gnome/ScreenSaver"
#define GS_INTERFACE "org.gnome.ScreenSaver"

#define SM_SERVICE   "org.gnome.SessionManager"
#define SM_PATH      "/org/gnome/SessionManager"
#define SM_INTERFACE "org.gnome.SessionManager"

#define GS_APPLICATION_NAME     "vdr-sxfe"
#define GS_REASON_FOR_INHIBIT   "Watching TV"

/* Log Messages */
#define MSG_OpenBusConnectionError "Failed to open connection to bus: %s"
#define MSG_RemoteMethodException "Caught remote method exception %s: %s"
#define MSG_GnomeAPI215Failed "GNOME screensaver 2.15 API failed, trying 2.14 API"
#define MSG_GError "Error: %s"
#define MSG_GNOMEScreensaverEnabled "GNOME screensaver enabled"
#define MSG_GNOMEScreensaverDisabled "GNOME screensaver disabled"
#define MSG_MCEScreensaverEnabled "MCE screensaver enabled"
#define MSG_MCEScreensaverDisabled "MCE screensaver disabled"

static guint32 cookie;

static int gnome_sessionmanager_control(DBusGConnection *connection, int enable)
{
  GError *error;
  DBusGProxy *proxy;
  gboolean ret;

  /* Create a proxy object */
  proxy = dbus_g_proxy_new_for_name(connection,
                                    SM_SERVICE, SM_PATH, SM_INTERFACE);
  if (!proxy) {
    LOGDBG("Failed to get a proxy for " SM_SERVICE);
    return 0;
  }

  error = NULL;
  if (enable) {
    ret = dbus_g_proxy_call(proxy, "Uninhibit", &error,
                            G_TYPE_UINT, cookie,
                            G_TYPE_INVALID, G_TYPE_INVALID);
  } else {
    ret = dbus_g_proxy_call(proxy, "Inhibit", &error,
                            G_TYPE_STRING, GS_APPLICATION_NAME,
                            G_TYPE_UINT,   0,
                            G_TYPE_STRING, GS_REASON_FOR_INHIBIT,
                            G_TYPE_UINT,   12,
                            G_TYPE_INVALID,
                            G_TYPE_UINT, &cookie,
                            G_TYPE_INVALID);
  }

  g_object_unref(proxy);

  if (!ret) {
    /* Check if it's a remote exception or a regular GError */
    if (error->domain == DBUS_GERROR &&
        error->code   == DBUS_GERROR_REMOTE_EXCEPTION) {
      LOGMSG(MSG_RemoteMethodException, dbus_g_error_get_name(error), error->message);
    } else {
      LOGMSG(MSG_GError, error->message);
    }
    g_error_free(error);

    return 0;
  }

  LOGMSG(enable ? MSG_GNOMEScreensaverEnabled : MSG_GNOMEScreensaverDisabled);
  return 1;
}

#ifdef HAVE_MCE_DBUS_NAMES
static int mce_control(DBusGConnection *connection, int enable)
{
  GError *error;
  DBusGProxy *proxy;
  gboolean ret = 1;

  /* Create a proxy object */
  proxy = dbus_g_proxy_new_for_name(connection,
                                    MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF);
  if (!proxy) {
    LOGDBG("Failed to get a proxy for " SM_SERVICE);
    return 0;
  }

  error = NULL;
  if (enable) {
    ret = dbus_g_proxy_call(proxy, MCE_CANCEL_PREVENT_BLANK_REQ, &error,
                            G_TYPE_INVALID, G_TYPE_INVALID);
  } else {
    ret = dbus_g_proxy_call(proxy, MCE_PREVENT_BLANK_REQ, &error,
                            G_TYPE_INVALID, G_TYPE_INVALID);
  }

  g_object_unref(proxy);

  if (!ret) {
    /* Check if it's a remote exception or a regular GError */
    if (error->domain == DBUS_GERROR &&
        error->code   == DBUS_GERROR_REMOTE_EXCEPTION) {
      LOGMSG(MSG_RemoteMethodException, dbus_g_error_get_name(error), error->message);
    } else {
      LOGMSG(MSG_GError, error->message);
    }
    g_error_free(error);

    return 0;
  }

  LOGMSG(enable ? MSG_MCEScreensaverEnabled : MSG_MCEScreensaverDisabled);
  return 1;
}
#endif

void gnome_screensaver_control(int enable)
{
  DBusGConnection *connection;
  GError *error;
  DBusGProxy *proxy;
  gboolean ret;

#ifdef GLIB_CHECK_VERSION
#if !GLIB_CHECK_VERSION(2,35,0)
  g_type_init();
#endif
#endif

#ifdef HAVE_MCE_DBUS_NAMES
  error = NULL;
  connection = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
  if (!connection) {
    LOGMSG(MSG_OpenBusConnectionError, error ? error->message : "<null>");
    g_error_free(error);
  } else if (mce_control(connection, enable)) {
    return;
  }
#endif

  /* Get a connection to the session bus */
  error = NULL;
  connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
  if (!connection) {
    LOGMSG(MSG_OpenBusConnectionError, error ? error->message : "<null>");
    g_error_free(error);
    return;
  }

  /* try session manager interface first */
  if (gnome_sessionmanager_control(connection, enable))
    return;

  /* Create a proxy object */
  proxy = dbus_g_proxy_new_for_name(connection,
                                    GS_SERVICE, GS_PATH, GS_INTERFACE);
  if (!proxy) {
    LOGDBG("Failed to get a proxy for " GS_SERVICE);
    return;
  }

  error = NULL;

  /* Enable the screensaver */
  if (enable) {
    /* First call the GNOME screensaver 2.15 API method */
    ret =
        dbus_g_proxy_call(proxy, "UnInhibit", &error,
                          G_TYPE_UINT, cookie,
                          G_TYPE_INVALID, G_TYPE_INVALID);

    /* If this fails, try the GNOME screensaver 2.14 API */
    if (!ret && error->domain == DBUS_GERROR
        && error->code == DBUS_GERROR_UNKNOWN_METHOD) {
      LOGMSG(MSG_GnomeAPI215Failed);
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
      LOGMSG(MSG_GnomeAPI215Failed);
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
      LOGMSG(MSG_RemoteMethodException, dbus_g_error_get_name(error), error->message);
    }
    else {
      LOGMSG(MSG_GError, error->message);
    }
    g_error_free(error);
  }
  else {
    LOGMSG(enable ? MSG_GNOMEScreensaverEnabled : MSG_GNOMEScreensaverDisabled);
  }

  g_object_unref(proxy);
}
