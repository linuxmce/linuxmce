#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/pluto.func

DEVICETEMPLATE_WIIMOTE=1954
DEVICEDATA_Configuration=47
WIIMOTES=$2

modprobe uinput

killall DiscoverWiiMote.sh

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); Device_ID="${Params[$i]}" ;;
		-i) ((i++)); Device_IP="${Params[$i]}" ;;
		-m) ((i++)); Device_MAC="${Params[$i]}" ;;
	esac
done

CONFIG=$(GetDeviceData "$i" "$DEVICEDATA_Configuration")

	screen -d -m -S "wminput-$i" /usr/bin/wminput -d -c $CONFIG $Device_MAC 

