#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/LockUtils.sh

if [[ "$Display" == "" ]] ;then
	if [[ "$DISPLAY" == "" ]] ;then
		export DISPLAY=:0
	fi
else
	export DISPLAY=":${Display}"
fi

if ! Lock "Launch_Manager" "LM"; then
	# LM is running
	echo "LM already running"
	wmctrl -a "Linux MCE Launch Manager"
	exit
fi

trap 'Unlock "Launch_Manager" "LM"' EXIT

KDE_DEBUG=1
LM_BINARY=/usr/pluto/bin/lmce_launch_manager
LM_EXIT_CODE=-1

while [[ "$LM_EXIT_CODE" != "0" ]] ;do
	$LM_BINARY --nofork --nocrashhandler
	LM_EXIT_CODE=$?
done
