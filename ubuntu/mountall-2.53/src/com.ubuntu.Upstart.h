/* mountall
 *
 * Copyright (C) 2010 Scott James Remnant <scott@netsplit.com>.
 *
 * This file was automatically generated; see the source for copying
 * conditions.
 */

#ifndef MOUNTALL_COM_UBUNTU_UPSTART_H
#define MOUNTALL_COM_UBUNTU_UPSTART_H

#include <dbus/dbus.h>

#include <stdint.h>

#include <nih/macros.h>

#include <nih-dbus/dbus_interface.h>
#include <nih-dbus/dbus_message.h>
#include <nih-dbus/dbus_pending_data.h>
#include <nih-dbus/dbus_proxy.h>


typedef struct upstart_properties {
	char *version;
	char *log_priority;
} UpstartProperties;


typedef void (*UpstartReloadConfigurationReply) (void *data, NihDBusMessage *message);

typedef void (*UpstartGetJobByNameReply) (void *data, NihDBusMessage *message, const char *job);

typedef void (*UpstartGetAllJobsReply) (void *data, NihDBusMessage *message, char * const *jobs);

typedef void (*UpstartEmitEventReply) (void *data, NihDBusMessage *message);

typedef void (*UpstartJobAddedHandler) (void *data, NihDBusMessage *message, const char *job);

typedef void (*UpstartJobRemovedHandler) (void *data, NihDBusMessage *message, const char *job);

typedef void (*UpstartGetVersionReply) (void *data, NihDBusMessage *message, const char *value);

typedef void (*UpstartGetLogPriorityReply) (void *data, NihDBusMessage *message, const char *value);

typedef void (*UpstartSetLogPriorityReply) (void *data, NihDBusMessage *message);

typedef void (*UpstartGetAllReply) (void *data, NihDBusMessage *message, const UpstartProperties *properties);


NIH_BEGIN_EXTERN

extern const NihDBusInterface  upstart_com_ubuntu_Upstart0_6;
extern const NihDBusInterface *upstart_interfaces[];


DBusPendingCall *upstart_reload_configuration      (NihDBusProxy *proxy, UpstartReloadConfigurationReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_reload_configuration_sync (const void *parent, NihDBusProxy *proxy)
	__attribute__ ((warn_unused_result));
DBusPendingCall *upstart_get_job_by_name           (NihDBusProxy *proxy, const char *name, UpstartGetJobByNameReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_get_job_by_name_sync      (const void *parent, NihDBusProxy *proxy, const char *name, char **job)
	__attribute__ ((warn_unused_result));
DBusPendingCall *upstart_get_all_jobs              (NihDBusProxy *proxy, UpstartGetAllJobsReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_get_all_jobs_sync         (const void *parent, NihDBusProxy *proxy, char ***jobs)
	__attribute__ ((warn_unused_result));
DBusPendingCall *upstart_emit_event                (NihDBusProxy *proxy, const char *name, char * const *env, int wait, UpstartEmitEventReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_emit_event_sync           (const void *parent, NihDBusProxy *proxy, const char *name, char * const *env, int wait)
	__attribute__ ((warn_unused_result));
DBusPendingCall *upstart_get_version               (NihDBusProxy *proxy, UpstartGetVersionReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_get_version_sync          (const void *parent, NihDBusProxy *proxy, char **value)
	__attribute__ ((warn_unused_result));
DBusPendingCall *upstart_get_log_priority          (NihDBusProxy *proxy, UpstartGetLogPriorityReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_get_log_priority_sync     (const void *parent, NihDBusProxy *proxy, char **value)
	__attribute__ ((warn_unused_result));
DBusPendingCall *upstart_set_log_priority          (NihDBusProxy *proxy, const char *value, UpstartSetLogPriorityReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_set_log_priority_sync     (const void *parent, NihDBusProxy *proxy, const char *value)
	__attribute__ ((warn_unused_result));
DBusPendingCall *upstart_get_all                   (NihDBusProxy *proxy, UpstartGetAllReply handler, NihDBusErrorHandler error_handler, void *data, int timeout)
	__attribute__ ((warn_unused_result));
int              upstart_get_all_sync              (const void *parent, NihDBusProxy *proxy, UpstartProperties **properties)
	__attribute__ ((warn_unused_result));

NIH_END_EXTERN

#endif /* MOUNTALL_COM_UBUNTU_UPSTART_H */
