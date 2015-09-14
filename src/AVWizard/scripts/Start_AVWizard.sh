#!/bin/bash
### BEGIN INIT INFO
# Provides:          avwizard
# Required-Start:    check_avwizard
# Required-Stop:
# Should-Start
# Default-Start:     2 3 4 5
# Default-Stop:      1 6
# Short-Description: AVWizard
# Description:       This script starts the AV Wizard
### END INIT INFO #

# Source fuction library
. /lib/lsb/init-functions

CheckAVWizard () {
	log_daemon_msg "Starting AVWizard" "AVWizard_Run.sh"

	. /usr/pluto/bin/Config_Ops.sh
	. /usr/pluto/bin/Utils.sh

	if [[ "$AutostartMedia" == "1" ]] ; then
		if [[ "$AVWizardOverride" != "0" || "$AVWizardDone" != "1" ]] ; then
			## Install driver based on the type of video card used
			CheckVideoDriver

			## Run the AVWizard
			/usr/pluto/bin/AVWizard_Run.sh 1>/dev/null 2>/dev/null </dev/null
			if [[ "$?" == "0" ]]; then
				ConfSet "AVWizardOverride" "0"
			fi
		else
			beep -l 50 -f 1000 -n -l 50 -f 1200 -n -l 50 -f 1400 -n -l 50 -f 1600 -n -l 50 -f 1800 -n -l 50 -f 2000 -n -l 50 -f 2200 &
		fi
	fi
	killall WatchGyroRemote 2>/dev/null

	err=0
	log_end_msg $err
	return $err
}

start() {
	CheckAVWizard
	return $?
}

stop() {
	: # noop
	return $?
}

status() {
	status_of_proc AVWizard_Run.sh AVWizard
	return $?
}

### main logic ###
case "$1" in
	start)
		$1
		retval=$?
		;;
	stop)
		$1
		retval=$?
	        ;;
	status)
		$1
		retval=$?
		;;
	restart|reload)
		: # noop
		retval=$?
		;;
	*)
		echo $"Usage: $0 {start|stop|restart|reload|status}"
		retval=1
esac
exit $retval

