#!/bin/bash

## Includes
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null || exit 1  ## So the orbiter can get the environment variable for the mysql server
. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null || exit 1
. /usr/pluto/bin/Utils.sh 2>/dev/null || exit 1
. /usr/pluto/bin/pluto.func 2>/dev/null || exit 1

## Read command line params
Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); DeviceID="${Params[$i]}" ;;
        esac
done

DD_DISTRO_Raspbian_CONST=19
Executable=/usr/pluto/bin/qorbiter-core-gl

cd /usr/pluto/bin

killall WatchGyroRemote  # In case it was running it will be grabbing the hid device

PLATFORM=""
if [[ "$PK_Distro" == "$DD_DISTRO_Raspbian_CONST" ]] ;then
	# FIXME: this should be done globally - export the rpi library path
	export LD_LIBRARY_PATH=/opt/vc/lib

	PLATFORM="-platform eglfs"
fi

## Run qOrbiter

if [[ "$Executable" == *"$Valgrind"* ]]; then
	$VGcmd "$Executable" "$@" $PLATFORM
	qOrbiter_RetCode=$?
else
	"$Executable" "$@" $PLATFORM
	qOrbiter_RetCode=$?
fi

exit ${qOrbiter_RetCode}
