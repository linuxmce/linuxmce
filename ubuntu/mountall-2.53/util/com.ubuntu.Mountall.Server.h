/* mountall
 *
 * Copyright (C) 2010 Scott James Remnant <scott@netsplit.com>.
 *
 * This file was automatically generated; see the source for copying
 * conditions.
 */

#ifndef MOUNTALL_COM_UBUNTU_MOUNTALL_SERVER_H
#define MOUNTALL_COM_UBUNTU_MOUNTALL_SERVER_H

#include <dbus/dbus.h>

#include <stdint.h>

#include <nih/macros.h>

#include <nih-dbus/dbus_interface.h>
#include <nih-dbus/dbus_message.h>
#include <nih-dbus/dbus_pending_data.h>
#include <nih-dbus/dbus_proxy.h>


typedef struct mountall_server_properties {
	char *version;
} MountallServerProperties;


typedef void (*MountallServerStopTimerReply) (void *data, NihDBusMessage *message);

typedef void (*MountallServerRestartTimerReply) (void *data, NihDBusMessage *message);

typedef void (*MountallServerChangeMountDeviceReply) (void *data, NihDBusMessage *message);

typedef void (*MountallServerGetVersionReply) (void *data, NihDBusMessage *message, const char *value);

typedef void (*MountallServerGetAllReply) (void *data, NihDBusMessage *message, const MountallServerProperties *properties);


NIH_BEGIN_EXTERN

extern const NihDBusInterface  mountall_server_com_ubuntu_Mountall0_1_Server;
extern const NihDBusInterface *mountall_server_interfaces[];


DBusPendingCall *mountall_server_stop_timer               (NihDBusProxy *proxy, const char *mountpoint, MountallServerStopTimerReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              mountall_server_stop_timer_sync          (const void *parent, NihDBusProxy *proxy, const char *mountpoint)
	__attribute__ ((warn_unused_result));
DBusPendingCall *mountall_server_restart_timer            (NihDBusProxy *proxy, const char *mountpoint, MountallServerRestartTimerReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              mountall_server_restart_timer_sync       (const void *parent, NihDBusProxy *proxy, const char *mountpoint)
	__attribute__ ((warn_unused_result));
DBusPendingCall *mountall_server_change_mount_device      (NihDBusProxy *proxy, const char *devname, const char *path, MountallServerChangeMountDeviceReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              mountall_server_change_mount_device_sync (const void *parent, NihDBusProxy *proxy, const char *devname, const char *path)
	__attribute__ ((warn_unused_result));
DBusPendingCall *mountall_server_get_version              (NihDBusProxy *proxy, MountallServerGetVersionReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              mountall_server_get_version_sync         (const void *parent, NihDBusProxy *proxy, char **value)
	__attribute__ ((warn_unused_result));
DBusPendingCall *mountall_server_get_all                  (NihDBusProxy *proxy, MountallServerGetAllReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              mountall_server_get_all_sync             (const void *parent, NihDBusProxy *proxy, MountallServerProperties **properties)
	__attribute__ ((warn_unused_result));

NIH_END_EXTERN

#endif /* MOUNTALL_COM_UBUNTU_MOUNTALL_SERVER_H */
