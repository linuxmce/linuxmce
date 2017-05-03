/* mountall
 *
 * com.ubuntu.Upstart.c - auto-generated D-Bus bindings
 *
 * Copyright (C) 2010 Scott James Remnant <scott@netsplit.com>.
 *
 * This file was automatically generated; see the source for copying
 * conditions.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */


#include <dbus/dbus.h>

#include <stdint.h>
#include <string.h>

#include <nih/macros.h>
#include <nih/alloc.h>
#include <nih/string.h>
#include <nih/logging.h>
#include <nih/error.h>

#include <nih-dbus/dbus_error.h>
#include <nih-dbus/dbus_message.h>
#include <nih-dbus/dbus_pending_data.h>
#include <nih-dbus/dbus_proxy.h>
#include <nih-dbus/errors.h>

#include "com.ubuntu.Upstart.h"


/* Prototypes for static functions */
static void              upstart_com_ubuntu_Upstart0_6_ReloadConfiguration_notify (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);
static void              upstart_com_ubuntu_Upstart0_6_GetJobByName_notify        (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);
static void              upstart_com_ubuntu_Upstart0_6_GetAllJobs_notify          (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);
static void              upstart_com_ubuntu_Upstart0_6_EmitEvent_notify           (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);
static DBusHandlerResult upstart_com_ubuntu_Upstart0_6_JobAdded_signal            (DBusConnection *connection, DBusMessage *signal, NihDBusProxySignal *proxied);
static DBusHandlerResult upstart_com_ubuntu_Upstart0_6_JobRemoved_signal          (DBusConnection *connection, DBusMessage *signal, NihDBusProxySignal *proxied);
static void              upstart_com_ubuntu_Upstart0_6_version_get_notify         (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);
static void              upstart_com_ubuntu_Upstart0_6_log_priority_get_notify    (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);
static void              upstart_com_ubuntu_Upstart0_6_log_priority_set_notify    (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);
static void              upstart_com_ubuntu_Upstart0_6_get_all_notify             (DBusPendingCall *pending_call, NihDBusPendingData *pending_data);


static const NihDBusArg upstart_com_ubuntu_Upstart0_6_ReloadConfiguration_method_args[] = {
	{ NULL }
};

static const NihDBusArg upstart_com_ubuntu_Upstart0_6_GetJobByName_method_args[] = {
	{ "name", "s", NIH_DBUS_ARG_IN  },
	{ "job",  "o", NIH_DBUS_ARG_OUT },
	{ NULL }
};

static const NihDBusArg upstart_com_ubuntu_Upstart0_6_GetAllJobs_method_args[] = {
	{ "jobs", "ao", NIH_DBUS_ARG_OUT },
	{ NULL }
};

static const NihDBusArg upstart_com_ubuntu_Upstart0_6_EmitEvent_method_args[] = {
	{ "name", "s",  NIH_DBUS_ARG_IN  },
	{ "env",  "as", NIH_DBUS_ARG_IN  },
	{ "wait", "b",  NIH_DBUS_ARG_IN  },
	{ NULL }
};

static const NihDBusMethod upstart_com_ubuntu_Upstart0_6_methods[] = {
	{ "ReloadConfiguration", upstart_com_ubuntu_Upstart0_6_ReloadConfiguration_method_args, NULL },
	{ "GetJobByName",        upstart_com_ubuntu_Upstart0_6_GetJobByName_method_args,        NULL },
	{ "GetAllJobs",          upstart_com_ubuntu_Upstart0_6_GetAllJobs_method_args,          NULL },
	{ "EmitEvent",           upstart_com_ubuntu_Upstart0_6_EmitEvent_method_args,           NULL },
	{ NULL }
};

static const NihDBusArg upstart_com_ubuntu_Upstart0_6_JobAdded_signal_args[] = {
	{ "job", "o", NIH_DBUS_ARG_OUT },
	{ NULL }
};

static const NihDBusArg upstart_com_ubuntu_Upstart0_6_JobRemoved_signal_args[] = {
	{ "job", "o", NIH_DBUS_ARG_OUT },
	{ NULL }
};

static const NihDBusSignal upstart_com_ubuntu_Upstart0_6_signals[] = {
	{ "JobAdded",   upstart_com_ubuntu_Upstart0_6_JobAdded_signal_args,   upstart_com_ubuntu_Upstart0_6_JobAdded_signal   },
	{ "JobRemoved", upstart_com_ubuntu_Upstart0_6_JobRemoved_signal_args, upstart_com_ubuntu_Upstart0_6_JobRemoved_signal },
	{ NULL }
};

static const NihDBusProperty upstart_com_ubuntu_Upstart0_6_properties[] = {
	{ "version",      "s", NIH_DBUS_READ,      NULL, NULL },
	{ "log_priority", "s", NIH_DBUS_READWRITE, NULL, NULL },
	{ NULL }
};

const NihDBusInterface upstart_com_ubuntu_Upstart0_6 = {
	"com.ubuntu.Upstart0_6",
	upstart_com_ubuntu_Upstart0_6_methods,
	upstart_com_ubuntu_Upstart0_6_signals,
	upstart_com_ubuntu_Upstart0_6_properties
};

const NihDBusInterface *upstart_interfaces[] = {
	&upstart_com_ubuntu_Upstart0_6,
	NULL
};


DBusPendingCall *
upstart_reload_configuration (NihDBusProxy *                  proxy,
                              UpstartReloadConfigurationReply handler,
                              NihDBusErrorHandler             error_handler,
                              void *                          data,
                              int                             timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;

	nih_assert (proxy != NULL);
	nih_assert ((handler == NULL) || (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "ReloadConfiguration");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Handle a fire-and-forget message */
	if (! error_handler) {
		dbus_message_set_no_reply (method_call, TRUE);
		if (! dbus_connection_send (proxy->connection, method_call, NULL)) {
			dbus_message_unref (method_call);
			nih_return_no_memory_error (NULL);
		}

		dbus_message_unref (method_call);
		return (DBusPendingCall *)TRUE;
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_ReloadConfiguration_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_ReloadConfiguration_notify (DBusPendingCall *   pending_call,
                                                          NihDBusPendingData *pending_data)
{
	DBusMessage *   reply;
	DBusMessageIter iter;
	NihDBusMessage *message;
	DBusError       error;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	do {
		__label__ enomem;

		/* Create a message context for the reply, and iterate
		 * over its arguments.
		 */
		message = nih_dbus_message_new (pending_data, pending_data->connection, reply);
		if (! message)
			goto enomem;

		dbus_message_iter_init (message->message, &iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

	enomem: __attribute__ ((unused));
	} while (! message);

	/* Call the handler function */
	if (pending_data->handler) {
		nih_error_push_context ();
		((UpstartReloadConfigurationReply)pending_data->handler) (pending_data->data, message);
		nih_error_pop_context ();
	}

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_reload_configuration_sync (const void *  parent,
                                   NihDBusProxy *proxy)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusError       error;
	DBusMessage *   reply;

	nih_assert (proxy != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "ReloadConfiguration");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	dbus_message_unref (method_call);

	/* Iterate the arguments of the reply */
	dbus_message_iter_init (reply, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_unref (reply);

	return 0;
}


DBusPendingCall *
upstart_get_job_by_name (NihDBusProxy *           proxy,
                         const char *             name,
                         UpstartGetJobByNameReply handler,
                         NihDBusErrorHandler      error_handler,
                         void *                   data,
                         int                      timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;

	nih_assert (proxy != NULL);
	nih_assert (name != NULL);
	nih_assert ((handler == NULL) || (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "GetJobByName");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Marshal a char * onto the message */
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &name)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Handle a fire-and-forget message */
	if (! error_handler) {
		dbus_message_set_no_reply (method_call, TRUE);
		if (! dbus_connection_send (proxy->connection, method_call, NULL)) {
			dbus_message_unref (method_call);
			nih_return_no_memory_error (NULL);
		}

		dbus_message_unref (method_call);
		return (DBusPendingCall *)TRUE;
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_GetJobByName_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_GetJobByName_notify (DBusPendingCall *   pending_call,
                                                   NihDBusPendingData *pending_data)
{
	DBusMessage *   reply;
	DBusMessageIter iter;
	NihDBusMessage *message;
	DBusError       error;
	char *          job;
	const char *    job_dbus;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	do {
		__label__ enomem;

		/* Create a message context for the reply, and iterate
		 * over its arguments.
		 */
		message = nih_dbus_message_new (pending_data, pending_data->connection, reply);
		if (! message)
			goto enomem;

		dbus_message_iter_init (message->message, &iter);

		/* Demarshal a char * from the message */
		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_OBJECT_PATH) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_get_basic (&iter, &job_dbus);

		job = nih_strdup (message, job_dbus);
		if (! job) {
			nih_free (message);
			message = NULL;
			goto enomem;
		}

		dbus_message_iter_next (&iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

	enomem: __attribute__ ((unused));
	} while (! message);

	/* Call the handler function */
	if (pending_data->handler) {
		nih_error_push_context ();
		((UpstartGetJobByNameReply)pending_data->handler) (pending_data->data, message, job);
		nih_error_pop_context ();
	}

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_get_job_by_name_sync (const void *  parent,
                              NihDBusProxy *proxy,
                              const char *  name,
                              char **       job)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusError       error;
	DBusMessage *   reply;
	char *          job_local;
	const char *    job_local_dbus;

	nih_assert (proxy != NULL);
	nih_assert (name != NULL);
	nih_assert (job != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "GetJobByName");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Marshal a char * onto the message */
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &name)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	dbus_message_unref (method_call);

	/* Iterate the arguments of the reply */
	dbus_message_iter_init (reply, &iter);

	do {
		__label__ enomem;

		/* Demarshal a char * from the message */
		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_OBJECT_PATH) {
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_get_basic (&iter, &job_local_dbus);

		job_local = nih_strdup (parent, job_local_dbus);
		if (! job_local) {
			*job = NULL;
			goto enomem;
		}

		dbus_message_iter_next (&iter);

		*job = job_local;
	enomem: __attribute__ ((unused));
	} while (! *job);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		nih_free (job_local);
		*job = NULL;
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_unref (reply);

	return 0;
}


DBusPendingCall *
upstart_get_all_jobs (NihDBusProxy *         proxy,
                      UpstartGetAllJobsReply handler,
                      NihDBusErrorHandler    error_handler,
                      void *                 data,
                      int                    timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;

	nih_assert (proxy != NULL);
	nih_assert ((handler == NULL) || (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "GetAllJobs");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Handle a fire-and-forget message */
	if (! error_handler) {
		dbus_message_set_no_reply (method_call, TRUE);
		if (! dbus_connection_send (proxy->connection, method_call, NULL)) {
			dbus_message_unref (method_call);
			nih_return_no_memory_error (NULL);
		}

		dbus_message_unref (method_call);
		return (DBusPendingCall *)TRUE;
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_GetAllJobs_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_GetAllJobs_notify (DBusPendingCall *   pending_call,
                                                 NihDBusPendingData *pending_data)
{
	DBusMessage *   reply;
	DBusMessageIter iter;
	NihDBusMessage *message;
	DBusError       error;
	char **         jobs;
	DBusMessageIter jobs_iter;
	size_t          jobs_size;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	do {
		__label__ enomem;

		/* Create a message context for the reply, and iterate
		 * over its arguments.
		 */
		message = nih_dbus_message_new (pending_data, pending_data->connection, reply);
		if (! message)
			goto enomem;

		dbus_message_iter_init (message->message, &iter);

		/* Demarshal an array from the message */
		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_ARRAY) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_recurse (&iter, &jobs_iter);

		jobs_size = 0;
		jobs = NULL;

		jobs = nih_alloc (message, sizeof (char *));
		if (! jobs) {
			nih_free (message);
			message = NULL;
			goto enomem;
		}

		jobs[jobs_size] = NULL;

		while (dbus_message_iter_get_arg_type (&jobs_iter) != DBUS_TYPE_INVALID) {
			const char *jobs_element_dbus;
			char **     jobs_tmp;
			char *      jobs_element;

			/* Demarshal a char * from the message */
			if (dbus_message_iter_get_arg_type (&jobs_iter) != DBUS_TYPE_OBJECT_PATH) {
				if (jobs)
					nih_free (jobs);
				nih_error_push_context ();
				nih_error_raise (NIH_DBUS_INVALID_ARGS,
				                 _(NIH_DBUS_INVALID_ARGS_STR));
				pending_data->error_handler (pending_data->data, message);
				nih_error_pop_context ();

				nih_free (message);
				dbus_message_unref (reply);
				return;
			}

			dbus_message_iter_get_basic (&jobs_iter, &jobs_element_dbus);

			jobs_element = nih_strdup (jobs, jobs_element_dbus);
			if (! jobs_element) {
				if (jobs)
					nih_free (jobs);
				nih_free (message);
				message = NULL;
				goto enomem;
			}

			dbus_message_iter_next (&jobs_iter);

			if (jobs_size + 2 > SIZE_MAX / sizeof (char *)) {
				if (jobs)
					nih_free (jobs);
				nih_error_push_context ();
				nih_error_raise (NIH_DBUS_INVALID_ARGS,
				                 _(NIH_DBUS_INVALID_ARGS_STR));
				pending_data->error_handler (pending_data->data, message);
				nih_error_pop_context ();

				nih_free (message);
				dbus_message_unref (reply);
				return;
			}

			jobs_tmp = nih_realloc (jobs, message, sizeof (char *) * (jobs_size + 2));
			if (! jobs_tmp) {
				if (jobs)
					nih_free (jobs);
				nih_free (message);
				message = NULL;
				goto enomem;
			}

			jobs = jobs_tmp;
			jobs[jobs_size] = jobs_element;
			jobs[jobs_size + 1] = NULL;

			jobs_size++;
		}

		dbus_message_iter_next (&iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

	enomem: __attribute__ ((unused));
	} while (! message);

	/* Call the handler function */
	if (pending_data->handler) {
		nih_error_push_context ();
		((UpstartGetAllJobsReply)pending_data->handler) (pending_data->data, message, jobs);
		nih_error_pop_context ();
	}

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_get_all_jobs_sync (const void *  parent,
                           NihDBusProxy *proxy,
                           char ***      jobs)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusError       error;
	DBusMessage *   reply;
	char **         jobs_local;
	DBusMessageIter jobs_local_iter;
	size_t          jobs_local_size;

	nih_assert (proxy != NULL);
	nih_assert (jobs != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "GetAllJobs");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	dbus_message_unref (method_call);

	/* Iterate the arguments of the reply */
	dbus_message_iter_init (reply, &iter);

	do {
		__label__ enomem;

		/* Demarshal an array from the message */
		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_ARRAY) {
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_recurse (&iter, &jobs_local_iter);

		jobs_local_size = 0;
		jobs_local = NULL;

		jobs_local = nih_alloc (parent, sizeof (char *));
		if (! jobs_local) {
			*jobs = NULL;
			goto enomem;
		}

		jobs_local[jobs_local_size] = NULL;

		while (dbus_message_iter_get_arg_type (&jobs_local_iter) != DBUS_TYPE_INVALID) {
			const char *jobs_local_element_dbus;
			char **     jobs_local_tmp;
			char *      jobs_local_element;

			/* Demarshal a char * from the message */
			if (dbus_message_iter_get_arg_type (&jobs_local_iter) != DBUS_TYPE_OBJECT_PATH) {
				if (jobs_local)
					nih_free (jobs_local);
				dbus_message_unref (reply);
				nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
				                  _(NIH_DBUS_INVALID_ARGS_STR));
			}

			dbus_message_iter_get_basic (&jobs_local_iter, &jobs_local_element_dbus);

			jobs_local_element = nih_strdup (jobs_local, jobs_local_element_dbus);
			if (! jobs_local_element) {
				if (jobs_local)
					nih_free (jobs_local);
				*jobs = NULL;
				goto enomem;
			}

			dbus_message_iter_next (&jobs_local_iter);

			if (jobs_local_size + 2 > SIZE_MAX / sizeof (char *)) {
				if (jobs_local)
					nih_free (jobs_local);
				dbus_message_unref (reply);
				nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
				                  _(NIH_DBUS_INVALID_ARGS_STR));
			}

			jobs_local_tmp = nih_realloc (jobs_local, parent, sizeof (char *) * (jobs_local_size + 2));
			if (! jobs_local_tmp) {
				if (jobs_local)
					nih_free (jobs_local);
				*jobs = NULL;
				goto enomem;
			}

			jobs_local = jobs_local_tmp;
			jobs_local[jobs_local_size] = jobs_local_element;
			jobs_local[jobs_local_size + 1] = NULL;

			jobs_local_size++;
		}

		dbus_message_iter_next (&iter);

		*jobs = jobs_local;
	enomem: __attribute__ ((unused));
	} while (! *jobs);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		nih_free (jobs_local);
		*jobs = NULL;
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_unref (reply);

	return 0;
}


DBusPendingCall *
upstart_emit_event (NihDBusProxy *        proxy,
                    const char *          name,
                    char * const *        env,
                    int                   wait,
                    UpstartEmitEventReply handler,
                    NihDBusErrorHandler   error_handler,
                    void *                data,
                    int                   timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;
	DBusMessageIter     env_iter;

	nih_assert (proxy != NULL);
	nih_assert (name != NULL);
	nih_assert ((handler == NULL) || (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "EmitEvent");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Marshal a char * onto the message */
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &name)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Marshal an array onto the message */
	if (! dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s", &env_iter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	if (env) {
		for (size_t env_i = 0; env[env_i]; env_i++) {
			const char *env_element;

			env_element = env[env_i];

			/* Marshal a char * onto the message */
			if (! dbus_message_iter_append_basic (&env_iter, DBUS_TYPE_STRING, &env_element)) {
				dbus_message_iter_abandon_container (&iter, &env_iter);
				dbus_message_unref (method_call);
				nih_return_no_memory_error (NULL);
			}
		}
	}

	if (! dbus_message_iter_close_container (&iter, &env_iter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Marshal a int onto the message */
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_BOOLEAN, &wait)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Handle a fire-and-forget message */
	if (! error_handler) {
		dbus_message_set_no_reply (method_call, TRUE);
		if (! dbus_connection_send (proxy->connection, method_call, NULL)) {
			dbus_message_unref (method_call);
			nih_return_no_memory_error (NULL);
		}

		dbus_message_unref (method_call);
		return (DBusPendingCall *)TRUE;
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_EmitEvent_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_EmitEvent_notify (DBusPendingCall *   pending_call,
                                                NihDBusPendingData *pending_data)
{
	DBusMessage *   reply;
	DBusMessageIter iter;
	NihDBusMessage *message;
	DBusError       error;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	do {
		__label__ enomem;

		/* Create a message context for the reply, and iterate
		 * over its arguments.
		 */
		message = nih_dbus_message_new (pending_data, pending_data->connection, reply);
		if (! message)
			goto enomem;

		dbus_message_iter_init (message->message, &iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

	enomem: __attribute__ ((unused));
	} while (! message);

	/* Call the handler function */
	if (pending_data->handler) {
		nih_error_push_context ();
		((UpstartEmitEventReply)pending_data->handler) (pending_data->data, message);
		nih_error_pop_context ();
	}

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_emit_event_sync (const void *  parent,
                         NihDBusProxy *proxy,
                         const char *  name,
                         char * const *env,
                         int           wait)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusError       error;
	DBusMessage *   reply;
	DBusMessageIter env_iter;

	nih_assert (proxy != NULL);
	nih_assert (name != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "com.ubuntu.Upstart0_6", "EmitEvent");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	/* Marshal a char * onto the message */
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &name)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Marshal an array onto the message */
	if (! dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s", &env_iter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	if (env) {
		for (size_t env_i = 0; env[env_i]; env_i++) {
			const char *env_element;

			env_element = env[env_i];

			/* Marshal a char * onto the message */
			if (! dbus_message_iter_append_basic (&env_iter, DBUS_TYPE_STRING, &env_element)) {
				dbus_message_iter_abandon_container (&iter, &env_iter);
				dbus_message_unref (method_call);
				nih_return_no_memory_error (-1);
			}
		}
	}

	if (! dbus_message_iter_close_container (&iter, &env_iter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Marshal a int onto the message */
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_BOOLEAN, &wait)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	dbus_message_unref (method_call);

	/* Iterate the arguments of the reply */
	dbus_message_iter_init (reply, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_unref (reply);

	return 0;
}


static DBusHandlerResult
upstart_com_ubuntu_Upstart0_6_JobAdded_signal (DBusConnection *    connection,
                                               DBusMessage *       signal,
                                               NihDBusProxySignal *proxied)
{
	DBusMessageIter iter;
	NihDBusMessage *message;
	char *          job;
	const char *    job_dbus;

	nih_assert (connection != NULL);
	nih_assert (signal != NULL);
	nih_assert (proxied != NULL);
	nih_assert (connection == proxied->proxy->connection);

	if (! dbus_message_is_signal (signal, proxied->interface->name, proxied->signal->name))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (! dbus_message_has_path (signal, proxied->proxy->path))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (proxied->proxy->name)
		if (! dbus_message_has_sender (signal, proxied->proxy->owner))
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	message = nih_dbus_message_new (NULL, connection, signal);
	if (! message)
		return DBUS_HANDLER_RESULT_NEED_MEMORY;

	/* Iterate the arguments to the signal and demarshal into arguments
	 * for our own function call.
	 */
	dbus_message_iter_init (message->message, &iter);

	/* Demarshal a char * from the message */
	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_OBJECT_PATH) {
		nih_free (message);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	dbus_message_iter_get_basic (&iter, &job_dbus);

	job = nih_strdup (message, job_dbus);
	if (! job) {
		nih_free (message);
		return DBUS_HANDLER_RESULT_NEED_MEMORY;
	}

	dbus_message_iter_next (&iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		nih_free (message);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	/* Call the handler function */
	nih_error_push_context ();
	((UpstartJobAddedHandler)proxied->handler) (proxied->data, message, job);
	nih_error_pop_context ();
	nih_free (message);

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


static DBusHandlerResult
upstart_com_ubuntu_Upstart0_6_JobRemoved_signal (DBusConnection *    connection,
                                                 DBusMessage *       signal,
                                                 NihDBusProxySignal *proxied)
{
	DBusMessageIter iter;
	NihDBusMessage *message;
	char *          job;
	const char *    job_dbus;

	nih_assert (connection != NULL);
	nih_assert (signal != NULL);
	nih_assert (proxied != NULL);
	nih_assert (connection == proxied->proxy->connection);

	if (! dbus_message_is_signal (signal, proxied->interface->name, proxied->signal->name))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (! dbus_message_has_path (signal, proxied->proxy->path))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (proxied->proxy->name)
		if (! dbus_message_has_sender (signal, proxied->proxy->owner))
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	message = nih_dbus_message_new (NULL, connection, signal);
	if (! message)
		return DBUS_HANDLER_RESULT_NEED_MEMORY;

	/* Iterate the arguments to the signal and demarshal into arguments
	 * for our own function call.
	 */
	dbus_message_iter_init (message->message, &iter);

	/* Demarshal a char * from the message */
	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_OBJECT_PATH) {
		nih_free (message);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	dbus_message_iter_get_basic (&iter, &job_dbus);

	job = nih_strdup (message, job_dbus);
	if (! job) {
		nih_free (message);
		return DBUS_HANDLER_RESULT_NEED_MEMORY;
	}

	dbus_message_iter_next (&iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		nih_free (message);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	/* Call the handler function */
	nih_error_push_context ();
	((UpstartJobRemovedHandler)proxied->handler) (proxied->data, message, job);
	nih_error_pop_context ();
	nih_free (message);

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


DBusPendingCall *
upstart_get_version (NihDBusProxy *         proxy,
                     UpstartGetVersionReply handler,
                     NihDBusErrorHandler    error_handler,
                     void *                 data,
                     int                    timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;
	const char *        interface;
	const char *        property;

	nih_assert (proxy != NULL);
	nih_assert ((handler != NULL) && (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "Get");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	property = "version";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &property)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_version_get_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_version_get_notify (DBusPendingCall *   pending_call,
                                                  NihDBusPendingData *pending_data)
{
	DBusMessage *   reply;
	DBusMessageIter iter;
	DBusMessageIter variter;
	NihDBusMessage *message;
	DBusError       error;
	const char *    value_dbus;
	char *          value;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	do {
		__label__ enomem;

		/* Create a message context for the reply, and iterate
		 * over and recurse into the arguments.
		 */
		message = nih_dbus_message_new (pending_data, pending_data->connection, reply);
		if (! message)
			goto enomem;

		dbus_message_iter_init (message->message, &iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_VARIANT) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_recurse (&iter, &variter);

		/* Demarshal a char * from the message */
		if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_get_basic (&variter, &value_dbus);

		value = nih_strdup (message, value_dbus);
		if (! value) {
			nih_free (message);
			message = NULL;
			goto enomem;
		}

		dbus_message_iter_next (&variter);

		dbus_message_iter_next (&iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

	enomem: __attribute__ ((unused));
	} while (! message);

	/* Call the handler function */
	nih_error_push_context ();
	((UpstartGetVersionReply)pending_data->handler) (pending_data->data, message, value);
	nih_error_pop_context ();

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_get_version_sync (const void *  parent,
                          NihDBusProxy *proxy,
                          char **       value)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusMessageIter variter;
	DBusError       error;
	DBusMessage *   reply;
	const char *    interface;
	const char *    property;
	const char *    local_dbus;
	char *          local;

	nih_assert (proxy != NULL);
	nih_assert (value != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "Get");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	property = "version";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &property)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	dbus_message_unref (method_call);

	/* Iterate the method arguments, recursing into the variant */
	dbus_message_iter_init (reply, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_VARIANT) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_iter_recurse (&iter, &variter);

	dbus_message_iter_next (&iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	do {
		__label__ enomem;

		/* Demarshal a char * from the message */
		if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_get_basic (&variter, &local_dbus);

		local = nih_strdup (parent, local_dbus);
		if (! local) {
			*value = NULL;
			goto enomem;
		}

		dbus_message_iter_next (&variter);

		*value = local;
	enomem: __attribute__ ((unused));
	} while (! *value);

	dbus_message_unref (reply);

	return 0;
}


DBusPendingCall *
upstart_get_log_priority (NihDBusProxy *             proxy,
                          UpstartGetLogPriorityReply handler,
                          NihDBusErrorHandler        error_handler,
                          void *                     data,
                          int                        timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;
	const char *        interface;
	const char *        property;

	nih_assert (proxy != NULL);
	nih_assert ((handler != NULL) && (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "Get");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	property = "log_priority";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &property)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_log_priority_get_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_log_priority_get_notify (DBusPendingCall *   pending_call,
                                                       NihDBusPendingData *pending_data)
{
	DBusMessage *   reply;
	DBusMessageIter iter;
	DBusMessageIter variter;
	NihDBusMessage *message;
	DBusError       error;
	const char *    value_dbus;
	char *          value;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	do {
		__label__ enomem;

		/* Create a message context for the reply, and iterate
		 * over and recurse into the arguments.
		 */
		message = nih_dbus_message_new (pending_data, pending_data->connection, reply);
		if (! message)
			goto enomem;

		dbus_message_iter_init (message->message, &iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_VARIANT) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_recurse (&iter, &variter);

		/* Demarshal a char * from the message */
		if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_get_basic (&variter, &value_dbus);

		value = nih_strdup (message, value_dbus);
		if (! value) {
			nih_free (message);
			message = NULL;
			goto enomem;
		}

		dbus_message_iter_next (&variter);

		dbus_message_iter_next (&iter);

		if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

	enomem: __attribute__ ((unused));
	} while (! message);

	/* Call the handler function */
	nih_error_push_context ();
	((UpstartGetLogPriorityReply)pending_data->handler) (pending_data->data, message, value);
	nih_error_pop_context ();

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_get_log_priority_sync (const void *  parent,
                               NihDBusProxy *proxy,
                               char **       value)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusMessageIter variter;
	DBusError       error;
	DBusMessage *   reply;
	const char *    interface;
	const char *    property;
	const char *    local_dbus;
	char *          local;

	nih_assert (proxy != NULL);
	nih_assert (value != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "Get");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	property = "log_priority";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &property)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	dbus_message_unref (method_call);

	/* Iterate the method arguments, recursing into the variant */
	dbus_message_iter_init (reply, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_VARIANT) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_iter_recurse (&iter, &variter);

	dbus_message_iter_next (&iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	do {
		__label__ enomem;

		/* Demarshal a char * from the message */
		if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_get_basic (&variter, &local_dbus);

		local = nih_strdup (parent, local_dbus);
		if (! local) {
			*value = NULL;
			goto enomem;
		}

		dbus_message_iter_next (&variter);

		*value = local;
	enomem: __attribute__ ((unused));
	} while (! *value);

	dbus_message_unref (reply);

	return 0;
}

DBusPendingCall *
upstart_set_log_priority (NihDBusProxy *             proxy,
                          const char *               value,
                          UpstartSetLogPriorityReply handler,
                          NihDBusErrorHandler        error_handler,
                          void *                     data,
                          int                        timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusMessageIter     variter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;
	const char *        interface;
	const char *        property;

	nih_assert (proxy != NULL);
	nih_assert (value != NULL);
	nih_assert ((handler == NULL) || (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "Set");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	property = "log_priority";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &property)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	if (! dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "s", &variter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Marshal a char * onto the message */
	if (! dbus_message_iter_append_basic (&variter, DBUS_TYPE_STRING, &value)) {
		dbus_message_iter_abandon_container (&iter, &variter);
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	if (! dbus_message_iter_close_container (&iter, &variter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Handle a fire-and-forget message */
	if (! error_handler) {
		dbus_message_set_no_reply (method_call, TRUE);
		if (! dbus_connection_send (proxy->connection, method_call, NULL)) {
			dbus_message_unref (method_call);
			nih_return_no_memory_error (NULL);
		}

		dbus_message_unref (method_call);
		return (DBusPendingCall *)TRUE;
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_log_priority_set_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_log_priority_set_notify (DBusPendingCall *   pending_call,
                                                       NihDBusPendingData *pending_data)
{
	DBusMessage *   reply;
	DBusMessageIter iter;
	NihDBusMessage *message;
	DBusError       error;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	/* Create a message context for the reply, and check
	 * there are no arguments.
	 */
	message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));
	dbus_message_iter_init (message->message, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		nih_error_push_context ();
		nih_error_raise (NIH_DBUS_INVALID_ARGS,
		                 _(NIH_DBUS_INVALID_ARGS_STR));
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	/* Call the handler function */
	if (pending_data->handler) {
		nih_error_push_context ();
		((UpstartSetLogPriorityReply)pending_data->handler) (pending_data->data, message);
		nih_error_pop_context ();
	}

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_set_log_priority_sync (const void *  parent,
                               NihDBusProxy *proxy,
                               const char *  value)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusMessageIter variter;
	DBusError       error;
	DBusMessage *   reply;
	const char *    interface;
	const char *    property;

	nih_assert (proxy != NULL);
	nih_assert (value != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "Set");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	property = "log_priority";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &property)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	if (! dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "s", &variter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Marshal a char * onto the message */
	if (! dbus_message_iter_append_basic (&variter, DBUS_TYPE_STRING, &value)) {
		dbus_message_iter_abandon_container (&iter, &variter);
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	if (! dbus_message_iter_close_container (&iter, &variter)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	/* Check the reply has no arguments */
	dbus_message_unref (method_call);
	dbus_message_iter_init (reply, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_unref (reply);

	return 0;
}


DBusPendingCall *
upstart_get_all (NihDBusProxy *      proxy,
                 UpstartGetAllReply  handler,
                 NihDBusErrorHandler error_handler,
                 void *              data,
                 int                 timeout)
{
	DBusMessage *       method_call;
	DBusMessageIter     iter;
	DBusPendingCall *   pending_call;
	NihDBusPendingData *pending_data;
	const char *        interface;

	nih_assert (proxy != NULL);
	nih_assert ((handler != NULL) && (error_handler != NULL));

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "GetAll");
	if (! method_call)
		nih_return_no_memory_error (NULL);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	/* Send the message and set up the reply notification. */
	pending_data = nih_dbus_pending_data_new (NULL, proxy->connection,
	                                          (NihDBusReplyHandler)handler,
	                                          error_handler, data);
	if (! pending_data) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (NULL);
	}

	pending_call = NULL;
	if (! dbus_connection_send_with_reply (proxy->connection, method_call,
	                                       &pending_call, timeout)) {
		dbus_message_unref (method_call);
		nih_free (pending_data);
		nih_return_no_memory_error (NULL);
	}

	dbus_message_unref (method_call);

	if (! pending_call) {
		nih_dbus_error_raise (DBUS_ERROR_DISCONNECTED,
		                      "Connection is closed");
		nih_free (pending_data);
		return NULL;
	}

	NIH_MUST (dbus_pending_call_set_notify (pending_call, (DBusPendingCallNotifyFunction)upstart_com_ubuntu_Upstart0_6_get_all_notify,
	                                        pending_data, (DBusFreeFunction)nih_discard));

	return pending_call;
}

static void
upstart_com_ubuntu_Upstart0_6_get_all_notify (DBusPendingCall *   pending_call,
                                              NihDBusPendingData *pending_data)
{
	DBusMessage *      reply;
	DBusMessageIter    iter;
	DBusMessageIter    arrayiter;
	DBusMessageIter    dictiter;
	DBusMessageIter    variter;
	NihDBusMessage *   message;
	DBusError          error;
	const char *       property;
	UpstartProperties *properties;
	size_t             property_count;
	char *             version;
	const char *       version_dbus;
	char *             log_priority;
	const char *       log_priority_dbus;

	nih_assert (pending_call != NULL);
	nih_assert (pending_data != NULL);

	nih_assert (dbus_pending_call_get_completed (pending_call));

	/* Steal the reply from the pending call. */
	reply = dbus_pending_call_steal_reply (pending_call);
	nih_assert (reply != NULL);

	/* Handle error replies */
	if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR) {
		message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, message->message);

		nih_error_push_context ();
		nih_dbus_error_raise (error.name, error.message);
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		dbus_error_free (&error);
		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	nih_assert (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);

	/* Create a message context for the reply, and iterate
	 * over and recurse into the arguments.
	 */
	message = NIH_MUST (nih_dbus_message_new (pending_data, pending_data->connection, reply));

	/* Iterate the method arguments, recursing into the array */
	dbus_message_iter_init (reply, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_ARRAY) {
		nih_error_push_context ();
		nih_error_raise (NIH_DBUS_INVALID_ARGS,
		                 _(NIH_DBUS_INVALID_ARGS_STR));
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	properties = NIH_MUST (nih_new (message, UpstartProperties));
	property_count = 0;

	dbus_message_iter_recurse (&iter, &arrayiter);

	while (dbus_message_iter_get_arg_type (&arrayiter) != DBUS_TYPE_INVALID) {
		__label__ enomem;

		if (dbus_message_iter_get_arg_type (&arrayiter) != DBUS_TYPE_DICT_ENTRY) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_recurse (&arrayiter, &dictiter);

		if (dbus_message_iter_get_arg_type (&dictiter) != DBUS_TYPE_STRING) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_get_basic (&dictiter, &property);

		dbus_message_iter_next (&dictiter);

		if (dbus_message_iter_get_arg_type (&dictiter) != DBUS_TYPE_VARIANT) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_recurse (&dictiter, &variter);

		if (! strcmp (property, "version")) {
			/* Demarshal a char * from the message */
			if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
				nih_error_push_context ();
				nih_error_raise (NIH_DBUS_INVALID_ARGS,
				                 _(NIH_DBUS_INVALID_ARGS_STR));
				pending_data->error_handler (pending_data->data, message);
				nih_error_pop_context ();

				nih_free (message);
				dbus_message_unref (reply);
				return;
			}

			dbus_message_iter_get_basic (&variter, &version_dbus);

			version = nih_strdup (properties, version_dbus);
			if (! version) {
				goto enomem;
			}

			dbus_message_iter_next (&variter);

			properties->version = version;

			nih_assert (++property_count);
		}

		if (! strcmp (property, "log_priority")) {
			/* Demarshal a char * from the message */
			if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
				nih_error_push_context ();
				nih_error_raise (NIH_DBUS_INVALID_ARGS,
				                 _(NIH_DBUS_INVALID_ARGS_STR));
				pending_data->error_handler (pending_data->data, message);
				nih_error_pop_context ();

				nih_free (message);
				dbus_message_unref (reply);
				return;
			}

			dbus_message_iter_get_basic (&variter, &log_priority_dbus);

			log_priority = nih_strdup (properties, log_priority_dbus);
			if (! log_priority) {
				goto enomem;
			}

			dbus_message_iter_next (&variter);

			properties->log_priority = log_priority;

			nih_assert (++property_count);
		}

		dbus_message_iter_next (&dictiter);

		if (dbus_message_iter_get_arg_type (&dictiter) != DBUS_TYPE_INVALID) {
			nih_error_push_context ();
			nih_error_raise (NIH_DBUS_INVALID_ARGS,
			                 _(NIH_DBUS_INVALID_ARGS_STR));
			pending_data->error_handler (pending_data->data, message);
			nih_error_pop_context ();

			nih_free (message);
			dbus_message_unref (reply);
			return;
		}

		dbus_message_iter_next (&arrayiter);
	enomem: __attribute__ ((unused));
	}

	dbus_message_iter_next (&iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		nih_error_push_context ();
		nih_error_raise (NIH_DBUS_INVALID_ARGS,
		                 _(NIH_DBUS_INVALID_ARGS_STR));
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	if (property_count < 2) {
		nih_error_push_context ();
		nih_error_raise (NIH_DBUS_INVALID_ARGS,
		                 _(NIH_DBUS_INVALID_ARGS_STR));
		pending_data->error_handler (pending_data->data, message);
		nih_error_pop_context ();

		nih_free (message);
		dbus_message_unref (reply);
		return;
	}

	/* Call the handler function */
	nih_error_push_context ();
	((UpstartGetAllReply)pending_data->handler) (pending_data->data, message, properties);
	nih_error_pop_context ();

	nih_free (message);
	dbus_message_unref (reply);
}

int
upstart_get_all_sync (const void *        parent,
                      NihDBusProxy *      proxy,
                      UpstartProperties **properties)
{
	DBusMessage *   method_call;
	DBusMessageIter iter;
	DBusMessageIter arrayiter;
	DBusMessageIter dictiter;
	DBusMessageIter variter;
	DBusError       error;
	DBusMessage *   reply;
	size_t          property_count;
	const char *    interface;
	const char *    property;
	char *          version;
	const char *    version_dbus;
	char *          log_priority;
	const char *    log_priority_dbus;

	nih_assert (proxy != NULL);
	nih_assert (properties != NULL);

	/* Construct the method call message. */
	method_call = dbus_message_new_method_call (proxy->name, proxy->path, "org.freedesktop.DBus.Properties", "GetAll");
	if (! method_call)
		nih_return_no_memory_error (-1);

	dbus_message_set_auto_start (method_call, proxy->auto_start);

	dbus_message_iter_init_append (method_call, &iter);

	interface = "com.ubuntu.Upstart0_6";
	if (! dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &interface)) {
		dbus_message_unref (method_call);
		nih_return_no_memory_error (-1);
	}

	/* Send the message, and wait for the reply. */
	dbus_error_init (&error);

	reply = dbus_connection_send_with_reply_and_block (proxy->connection, method_call, -1, &error);
	if (! reply) {
		dbus_message_unref (method_call);

		if (dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY)) {
			nih_error_raise_no_memory ();
		} else {
			nih_dbus_error_raise (error.name, error.message);
		}

		dbus_error_free (&error);
		return -1;
	}

	dbus_message_unref (method_call);

	/* Iterate the method arguments, recursing into the array */
	dbus_message_iter_init (reply, &iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_ARRAY) {
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	*properties = NIH_MUST (nih_new (parent, UpstartProperties));
	property_count = 0;

	dbus_message_iter_recurse (&iter, &arrayiter);

	while (dbus_message_iter_get_arg_type (&arrayiter) != DBUS_TYPE_INVALID) {
		__label__ enomem;

		if (dbus_message_iter_get_arg_type (&arrayiter) != DBUS_TYPE_DICT_ENTRY) {
			nih_free (*properties);
			*properties = NULL;
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_recurse (&arrayiter, &dictiter);

		if (dbus_message_iter_get_arg_type (&dictiter) != DBUS_TYPE_STRING) {
			nih_free (*properties);
			*properties = NULL;
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_get_basic (&dictiter, &property);

		dbus_message_iter_next (&dictiter);

		if (dbus_message_iter_get_arg_type (&dictiter) != DBUS_TYPE_VARIANT) {
			nih_free (*properties);
			*properties = NULL;
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_recurse (&dictiter, &variter);

		if (! strcmp (property, "version")) {
			/* Demarshal a char * from the message */
			if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
				nih_free (*properties);
				*properties = NULL;
				dbus_message_unref (reply);
				nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
				                  _(NIH_DBUS_INVALID_ARGS_STR));
			}

			dbus_message_iter_get_basic (&variter, &version_dbus);

			version = nih_strdup (*properties, version_dbus);
			if (! version) {
				goto enomem;
			}

			dbus_message_iter_next (&variter);

			(*properties)->version = version;

			nih_assert (++property_count);
		}

		if (! strcmp (property, "log_priority")) {
			/* Demarshal a char * from the message */
			if (dbus_message_iter_get_arg_type (&variter) != DBUS_TYPE_STRING) {
				nih_free (*properties);
				*properties = NULL;
				dbus_message_unref (reply);
				nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
				                  _(NIH_DBUS_INVALID_ARGS_STR));
			}

			dbus_message_iter_get_basic (&variter, &log_priority_dbus);

			log_priority = nih_strdup (*properties, log_priority_dbus);
			if (! log_priority) {
				goto enomem;
			}

			dbus_message_iter_next (&variter);

			(*properties)->log_priority = log_priority;

			nih_assert (++property_count);
		}

		dbus_message_iter_next (&dictiter);

		if (dbus_message_iter_get_arg_type (&dictiter) != DBUS_TYPE_INVALID) {
			nih_free (*properties);
			*properties = NULL;
			dbus_message_unref (reply);
			nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
			                  _(NIH_DBUS_INVALID_ARGS_STR));
		}

		dbus_message_iter_next (&arrayiter);
	enomem: __attribute__ ((unused));
	}

	dbus_message_iter_next (&iter);

	if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_INVALID) {
		nih_free (*properties);
		*properties = NULL;
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	if (property_count < 2) {
		nih_free (*properties);
		*properties = NULL;
		dbus_message_unref (reply);
		nih_return_error (-1, NIH_DBUS_INVALID_ARGS,
		                  _(NIH_DBUS_INVALID_ARGS_STR));
	}

	dbus_message_unref (reply);

	return 0;
}
