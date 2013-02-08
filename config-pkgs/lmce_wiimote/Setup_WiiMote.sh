#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICETEMPLATE_WIIMOTE=1954
DEVICEDATA_Configuration=47
WIIMOTES=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_WIIMOTE" "" "" "All")

modprobe uinput

if [[ -z "$WIIMOTES" ]]; then
	# No Wiimotes found, nothing to do.
	exit 0
fi

for i in $WIIMOTES; 
do
	Q="SELECT MACAddress FROM Device WHERE PK_Device = $i"
	MACADDRESS=$(RunSQL "$Q")
	CONFIG=$(GetDeviceData "$i" "$DEVICEDATA_Configuration")
	screen -d -m -S "wminput-$i" /usr/bin/wminput -d -c $CONFIG $MACADDRESS
done

