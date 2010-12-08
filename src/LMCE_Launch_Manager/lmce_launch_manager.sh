#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/LockUtils.sh
ConfEval

if ["$AutostartMedia" -eq "1"] ; then
	# Only start X if we want to autostart the media.
	if [[ "$Display" == "" ]] ;then
		if [[ "$DISPLAY" == "" ]] ;then
			export DISPLAY=:0
		fi
	else
		export DISPLAY=":${Display}"
	fi
fi
if ! Lock "Launch_Manager" "LM"; then
	# LM is running
	echo "LM already running"
#	wmctrl -a "Linux MCE Launch Manager"
	exit
fi

trap 'Unlock "Launch_Manager" "LM"' EXIT
KDE_DEBUG=1
LM_BINARY=/usr/pluto/bin/LMCE_Launch_Manager
LM_EXIT_CODE=-1

while [ 1 ]
do

	while [[ "$LM_EXIT_CODE" != "0" ]] ;do
		/usr/bin/screen -d -m -S LMCE_Launch_Manager $LM_BINARY --nofork --nocrashhandler
		LM_EXIT_CODE=$?
	done

done

