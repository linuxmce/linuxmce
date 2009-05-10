#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh


DEVICEDATA_Diskless_Boot=9

IsDiskless=$(GetDeviceData "$PK_Device" "$DEVICEDATA_Diskless_Boot")

if [[ "$IsDiskless" == 1 ]]; then
	TheChosenOne=root
else
	Old_IFS="$IFS"
	IFS=:
	while read user x uid gid rest; do
		if ((uid < 1000 || uid >= 10000)); then
			continue
		fi
		TheChosenOne="$user"
		break
	done </etc/passwd
	IFS="$Old_IFS"
fi


if su - "$TheChosenOne" -c "DISPLAY=:$Display kcheckrunning"; then
	exit
fi

su - "$TheChosenOne" -c "DISPLAY=:$Display /usr/pluto/bin/startkde-keepdcop" &>/dev/null </dev/null &
disown -a
