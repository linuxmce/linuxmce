#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

DD_BLOCK_DEVICE=152
DT_APP_SERVER=26

DeviceID="$1"

DeviceName=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData='$DD_BLOCK_DEVICE' AND FK_Device='$DeviceID'");
DeviceName=$(Field 1 "$DeviceName")

DeviceComputer=$(RunSQL "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device='$DeviceID'")
DeviceComputer=$(Field 1 "$DeviceComputer")

if [[ "$DeviceComputer" != "$PK_Device" ]] ;then
	DeviceAppServer=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$DeviceComputer' AND FK_DeviceTemplate='$DT_APP_SERVER'")
	DeviceAppServer=$(Field 1 "$DeviceAppServer")

	/usr/pluto/bin/MessageSend "$DCERouter" -targetType device 0 "$DeviceAppServer" 1 67 13 "/usr/pluto/bin/monitoring_ZFS.sh" 51 "WEB_ADMIN_REFRESH	$DeviceName"
else
	/usr/pluto/bin/monitoring_ZFS.sh "WEB_ADMIN_REFRESH" "$DeviceName"
fi
