#!/bin/bash
# UpdateMediaDaemonControl.sh - control UpdateMediaDaemon.sh script
#
# DESCRIPTION:
#  This script will control the daemon for UpdateMedia plugin.
#
# USAGE :
#  UpdateMediaDaemonControl.sh [ -enable | -disable | -status ]
#

function LogMsg ()
	{
		echo $(date -R) $*
	}

# wrong 

#if [[ [ -z "$1" ] || [[ "$1" != "-enable" ]] || [[ "$1" != "-disable" ]] || [[ "$1" != "-status" ]] ]]; then
#	LogMsg ": UpdateMediaDaemonControl : Wrong parameter supplied !"
#fi

if [[ "$1" == "-enable" ]];then
# enable option
        if [[ ! -z "$(pidof -x UpdateMediaDaemon.sh)" ]]; then
                LogMsg "UpdateMedia is already running (PID $(pidof -x UpdateMediaDaemon.sh))"
        else
					
		LogMsg ": Enabling UpdateMediaDaemon on the system(s)"
		screen -d -m -S "BkgSS-UpdateMediaDaemon.sh" "/usr/pluto/bin/UpdateMediaDaemon.sh"
	fi

elif [[ "$1" == "-disable" ]];then
# disable option

	if [[ -z "$(pidof -x UpdateMediaDaemon.sh)" ]]; then
                LogMsg "UpdateMedia is stopped"
        else
							
		LogMsg ": Disabling UpdateMediaDaemon on the system(s)"
		kill -9 $(pidof -x UpdateMediaDaemon.sh)
	fi

elif [[ "$1" == "-status" ]]; then
# status option

	if [[ -z "$(pidof -x UpdateMediaDaemon.sh)" ]]; then
		LogMsg "UpdateMedia is stopped"
	else 
		LogMsg "UpdateMedia is running (PID - $(pidof -x UpdateMediaDaemon.sh))"
		
	fi
else 
# wrong param

	LogMsg ": UpdateMediaDaemonControl : Wrong parameter supplied !"
	LogMsg "Usage : UpdateMediaDaemonControl.sh [ -enable|-disable|-status ]"
fi

