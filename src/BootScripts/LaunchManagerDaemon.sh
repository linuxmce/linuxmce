#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Utils.sh

if ! Lock "Launch_Manager" "LM"; then                           
	# LM is running
        echo "LM already running"
        wmctrl -a "Linux MCE Launch Manager"
        exit
fi

trap 'Unlock "LM" "LMCE_Launch_Manager"' EXIT
Lock "LM" "LMCE_Launch_Manager"

LM_BINARY=/usr/pluto/bin/LMCE_Launch_Manager
LM_EXIT_CODE=-1

while [[ "$LM_EXIT_CODE" != "0" ]] ;do
	$LM_BINARY --nofork --nocrashhandler
	LM_EXIT_CODE=$?
done

function on_exit {

	echo -n "Waiting for the Launch Manager to shut down"
	while "$(pidof LMCE_Launch_Manager)" != "" ;do
		kill LMCE_Launch_Manager
		sleep 0.5
		echo -n "."
	fi
	echo 

	Unlock "LM" "LMCE_Launch_Manager"
}

trap 'Unlock "LMCE_Launch_Manager" "LM"' EXIT

exit 0

