#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

set -x
exec >>/var/log/pluto/RestartLocalX.log 2>&1
date -R

if [[ -f /etc/event.d/pluto || "$PK_Distro" == 1 ]] || grep -q Startup_Core-Hybrid /etc/rc2.d/S99kdm; then
	PidOfX=$(pidof X)
	if [[ -n "$PidOfX" ]]; then
		kill $PidOfX

		# wait for X to die
		Count=5
		while [[ -d /proc/$PidOfX && "$Count" -gt 0 ]]; do
			sleep 1
			((Count--))
		done
		if [[ -d /proc/$PidOfX ]]; then
			# it won't die, so let's give it a hand
			kill -9 $PidOfX
		fi
	fi

	if [[ "$PK_Distro" == 1 ]]; then
		sleep 1
		/usr/pluto/bin/Start_X.sh
	fi
else
	export TERM=linux
	service kdm restart &>/dev/null </dev/null
	echo "kdm restart exit code: $?"
fi
