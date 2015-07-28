#!/bin/sh
### BEGIN INIT INFO
# Provides:          squeezelite
# Required-Start:    $network $remote_fs
# Required-Stop:     $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Lightweight headless Squeezebox emulator
# Description:       Lightweight headless streaming audio player for Logitech's
#                    Squeezebox audio server
### END INIT INFO

# Author: Chris Boot <debian@bootc.net>

PATH=/sbin:/usr/sbin:/bin:/usr/bin
DESC="Squeezebox client"
NAME=squeezelite
DAEMON=/usr/bin/$NAME
PIDFILE=/run/$NAME.pid
SCRIPTNAME=/etc/init.d/$NAME

# Exit if the package is not installed
[ -x "$DAEMON" ] || exit 0

# Read configuration variable file if it is present
[ -r /etc/default/$NAME ] && . /etc/default/$NAME

# Load the VERBOSE setting and other rcS variables
. /lib/init/vars.sh

# Define LSB log_* functions.
. /lib/lsb/init-functions

#
# Function that starts the daemon/service
#
do_start()
{
	DAEMON_ARGS=""

	# add squeezelite name if set
	if [ -n "$SL_NAME" ]; then
		DAEMON_ARGS="${DAEMON_ARGS} -n ${SL_NAME}"
	fi

	# add souncard setting if set
	if [ -n "$SL_SOUNDCARD" ]; then
		DAEMON_ARGS="${DAEMON_ARGS} -o ${SL_SOUNDCARD}"
	fi

	# add squeezebox server ip address if set
	if [ -n "$SB_SERVER_IP" ]; then
		DAEMON_ARGS="${DAEMON_ARGS} -s ${SB_SERVER_IP}"
	fi

	# add any other options given by the user
	if [ -n "$SB_EXTRA_ARGS" ]; then
		DAEMON_ARGS="${DAEMON_ARGS} ${SB_EXTRA_ARGS}"
	fi

	# Return
	#   0 if daemon has been started
	#   1 if daemon was already running
	#   2 if daemon could not be started
	start-stop-daemon --start --quiet --pidfile $PIDFILE --exec $DAEMON \
		--test > /dev/null || return 1
	start-stop-daemon --start --quiet --pidfile $PIDFILE --exec $DAEMON \
		--background --make-pidfile -- $DAEMON_ARGS || return 2
}

#
# Function that stops the daemon/service
#
do_stop()
{
	# Return
	#   0 if daemon has been stopped
	#   1 if daemon was already stopped
	#   2 if daemon could not be stopped
	#   other if a failure occurred
	start-stop-daemon --stop --quiet --retry=TERM/30/KILL/5 \
		--pidfile $PIDFILE --exec $DAEMON
}

case "$1" in
  start)
	[ "$VERBOSE" != no ] && log_daemon_msg "Starting $DESC" "$NAME"
	do_start; RET=$?
	case $RET in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0; exit 0 ;;
		*) [ "$VERBOSE" != no ] && log_end_msg 1; exit 1 ;;
	esac
	;;
  stop)
	[ "$VERBOSE" != no ] && log_daemon_msg "Stopping $DESC" "$NAME"
	do_stop; RET=$?
	case $RET in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0; exit 0 ;;
		*) [ "$VERBOSE" != no ] && log_end_msg 1; exit 1 ;;
	esac
	;;
  status)
	status_of_proc "$DAEMON" "$NAME"
	;;
  restart|force-reload)
	log_daemon_msg "Restarting $DESC" "$NAME"
	do_stop; RET=$?
	case $RET in
	  0|1)
		do_start; RET=$?
		case $RET in
			0) log_end_msg 0; exit 0 ;;
			1) log_end_msg 1; exit 1 ;; # Old process is still running
			*) log_end_msg 1; exit 1 ;; # Failed to start
		esac
		;;
	  *)
		# Failed to stop
		log_end_msg 1; exit 1
		;;
	esac
	;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop|status|restart|force-reload}" >&2
	exit 3
	;;
esac

