#!/bin/bash

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DEVICETEMPLATE_App_Server=26
DEVICETEMPLATE_Generic_PC_as_Core=7

DEVICEDATA_DisklessBoot=9

#<-mkr_b_via_b->
Q="
	SELECT PK_Device
	FROM Device
	WHERE FK_DeviceTemplate='$DEVICETEMPLATE_Generic_PC_as_Core'
"
CoreID=$(RunSQL "$Q")
if [[ -z "$CoreID" ]]; then
	exit 1
fi

DisklessBoot=$(GetDeviceData "$PK_Device" "$DEVICEDATA_DisklessBoot")

if [[ "$DisklessBoot" != 1 ]]; then
	awk 'NR == 1 { print "0"; next; } {print}' /boot/grub/default >/boot/grub/default.new
	mv /boot/grub/default{.new,}
else
	AppServerID=$(FindDevice_Template "$CoreID" "$DEVICETEMPLATE_App_Server")
	if [[ -z "$AppServerID" ]]; then
		exit 1
	fi

	Tab=$'\t'
	/usr/pluto/bin/MessageSend $DCERouter 0 "$AppServerID" 1 67 13 "/usr/pluto/bin/Diskless_ChangeDisklessBootImage.sh" 51 "${PK_Device}${Tab}ubuntu" 241 1
fi

reboot
#<-mkr_b_via_e->
